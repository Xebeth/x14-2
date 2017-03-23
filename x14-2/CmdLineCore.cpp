/**************************************************************************
	created		:	2013-03-24
	filename	: 	CmdLineCore.cpp
	author		:	Xebeth`
	copyright	:	North Edge (2013)
	purpose		:	Command line module
**************************************************************************/
#include "stdafx.h"
#include <fstream>

#include "WindowerCore.h"
#include "CmdLineCore.h"

#include "WindowerEngine.h"
#include "CommandDispatcher.h"
#include "CommandParser.h"

#include "StringNode.h"

#include "ModuleService.h"
#include "FormatChatMsgService.h"

namespace Windower
{
	CmdLineCore::CallingContext<CmdLineCore> CmdLineCore::m_Context;

	/*! \brief CmdLineCore constructor
		\param[in,out] Engine_in_out : the windower engine
		\param[in,out] HookManager_in_out : the hook manager
	*/
	CmdLineCore::CmdLineCore()
		: WindowerCore(_T(CMD_LINE_MODULE)), m_pCommandDispatcher(nullptr), m_pTextCmdUnknown(nullptr),
		  m_pProcessCmdTrampoline(nullptr), m_pCommandParser(nullptr), m_pTextCmd(nullptr), m_pCondition(nullptr)
	{
		// set the calling context for the hooks
		m_Context.Set(this);
	}

	//! \brief CmdLineCore destructor
	CmdLineCore::~CmdLineCore()
	{
		// unregister the commands
		UnregisterCommands();

		if (m_pCommandParser != nullptr)
		{
			delete m_pCommandParser;
			m_pCommandParser = nullptr;
		}

		if (m_pCommandDispatcher != nullptr)
		{
			delete m_pCommandDispatcher;
			m_pCommandDispatcher = nullptr;
		}
	}

	/*! \brief Process a command typed by the player
		\param[in,out] pThis_in_out : the caller object
		\param[in,out] pCmd_in_out : a string node containing the command typed
		\param[in] pUnknown_in : unknown object (contains pThis_in_out at offset 0x41950)
	*/
	int WINAPI CmdLineCore::ProcessCmdHook(LPVOID pThis_in_out, StringNode* pCmd_in_out, LPVOID pUnknown_in)
	{
		int Result = 0;

		if (pUnknown_in != nullptr)
			m_Context->m_pTextCmdUnknown = pUnknown_in;
		if (pThis_in_out != nullptr)
			m_Context->m_pTextCmd = pThis_in_out;

		// update the text command pointer
		if (m_Context->m_pProcessCmdTrampoline != nullptr)
		{
			StringNode FeedbackNode, *pNode = pCmd_in_out;
			std::string Feedback;

			if (m_Context->FilterCommands(pCmd_in_out, Feedback))
			{
				if (Feedback.empty() == false)
				{
					pNode = &FeedbackNode;
					Feedback.insert(0, "/echo ");
					InitStringNode(FeedbackNode, Feedback.c_str());
				}
				else
					return true;
			}
			
			if (pThis_in_out != nullptr && pUnknown_in != nullptr && m_pEngine->IsExiting() == false)
			{
				// call the trampoline to process the command
				m_pEngine->LockMacroThread();
				Result = m_Context->m_pProcessCmdTrampoline(pThis_in_out, pNode, pUnknown_in);
				m_pEngine->UnlockMacroThread();
			}
		}

		return Result;
	}

	/*! \brief Filters the commands aimed at Windower
		\param[in] pCmd_in : the command text
		\param[out] Feedback_out : feedback of the command execution
		\return true if the command was executed; false otherwise
	*/
	bool CmdLineCore::FilterCommands(const StringNode *pCmd_in, std::string &Feedback_out)
	{
		if (pCmd_in != nullptr && pCmd_in->pResBuf != nullptr
		 && m_Context->m_pCommandParser != nullptr)
		{
			// the message starts with 2 forward slashes => expect a command
			if (pCmd_in->dwSize != 0 && (strstr(pCmd_in->pResBuf, "//") == pCmd_in->pResBuf || strstr(pCmd_in->pResBuf, "/!") == pCmd_in->pResBuf))
			{
				bool Result = false;

				if (pCmd_in->dwSize > 2)
				{
					size_t dwNewSize = 0UL;
					WindowerCommand Command;
					char *pFeedbackMsg = nullptr;					
					int parseResult = m_Context->m_pCommandParser->ParseCommand(pCmd_in->pResBuf + 2, Command,
																				&pFeedbackMsg, dwNewSize);

					if (parseResult == CommandParser::PARSER_RESULT_SUCCESS)
					{
						Command.Execute(Feedback_out);
						Result = true;
					}
					else if (pFeedbackMsg != nullptr)
					{
						Feedback_out.assign(pFeedbackMsg);
						Result = true;
					}

					if (Result && strstr(pCmd_in->pResBuf, "/!") == nullptr)
					{
						std::string Cmd(pCmd_in->pResBuf);

						// add the command to the feedback
						replace<char>(Cmd, "//", ">> ");
						Cmd += '\n';
						// insert it at the beginning
						Feedback_out.insert(0, Cmd);
					}

					if (pFeedbackMsg != nullptr)
						free(pFeedbackMsg);

					return Result;
				}
				// default feedback message in case of an unknown error
				Feedback_out = "Failed to process the command.";
			}
		}

		return false;
	}

	/*! \brief Register the hooks for this module
		\param[in] HookManager_in : the hook manager
	*/
	void CmdLineCore::RegisterHooks(HookEngineLib::IHookManager &HookManager_in)
	{
 		HookManager_in.RegisterHook(PROCESS_CMD_HOOK, SIGSCAN_GAME_PROCESSA, PROCESS_CMD_OPCODES_SIGNATURE,
									PROCESS_CMD_OPCODES_SIGNATURE_OFFSET, &CmdLineCore::ProcessCmdHook,
									PROCESS_CMD_OPCODES_SIZE);
	}

	/*! \brief Callback invoked when the hooks of the module are installed
		\param[in] HookManager_in : the hook manager
	*/
	void CmdLineCore::OnHookInstall(HookEngineLib::IHookManager &HookManager_in)
	{
		m_pProcessCmdTrampoline = (fnProcessCmd)HookManager_in.GetTrampolineFunc(PROCESS_CMD_HOOK);

		if (m_pProcessCmdTrampoline != nullptr)
		{
			// create the command dispatcher and parser
			m_pCommandDispatcher = new CommandDispatcher;
			m_pCommandParser = new CommandParser(*m_pCommandDispatcher);
			// register the built-in commands
			RegisterCommands();
		}
	}

	/*! \brief Registers the commands with the command dispatcher
		\return true if all the commands were registered successfully; false otherwise
	*/
	bool CmdLineCore::RegisterCommands()
	{
		if (m_pCommandDispatcher != nullptr)
		{
			bool Result = true;
			// register the "help" command
			WindowerCommand *pCommand = new WindowerCommand(ENGINE_KEY, CMD_HELP, "help",
															"Provides help with the specified command (all by default)", this);

			if (pCommand != nullptr)
			{
				// register the parameter
				pCommand->AddStringParam("command", true, "", "the name of the command for which to obtain help");

				if (RegisterCommand(pCommand) == false)
					delete pCommand;
			}

			Result &= (pCommand != nullptr);

			// register the "load" command
			pCommand = new WindowerCommand(ENGINE_KEY, CMD_LOAD_PLUGIN, "load",
										   "Loads a plugin given its name.", this);

			if (pCommand != nullptr)
			{
				pCommand->AddStringParam("plugin", false, "", "the name of the plugin to load");

				if (RegisterCommand(pCommand) == false)
				{
					delete pCommand;
					pCommand = nullptr;
				}
			}

			Result &= (pCommand != nullptr);
			// register the "unload" command
			pCommand = new WindowerCommand(ENGINE_KEY, CMD_UNLOAD_PLUGIN, "unload",
										   "Unloads a plugin given its name.", this);

			if (pCommand != nullptr)
			{
				pCommand->AddStringParam("plugin", false, "", "the name of the plugin to unload");

				if (RegisterCommand(pCommand) == false)
				{
					delete pCommand;
					pCommand = nullptr;
				}
			}

			Result &= (pCommand != nullptr);
/*
			// register the "configure" command
			pCommand = new WindowerCommand(ENGINE_KEY, CMD_CONFIGURE, "configure",
										   "Configures a plugin given its name.", this, false);

			if (pCommand != nullptr)
			{
				pCommand->AddStringParam("plugin", false, "", "the name of the plugin to configure");

				if (RegisterCommand(pCommand) == false)
				{
					delete pCommand;
					pCommand = nullptr;
				}
			}

			Result &= (pCommand != nullptr);
*/
			// register the "list" command
			pCommand = new WindowerCommand(ENGINE_KEY, CMD_LIST_PLUGINS, "list",
										   "Gives a list of the available plugins.", this);

			if (pCommand != nullptr && RegisterCommand(pCommand) == false)
			{
				delete pCommand;
				pCommand = nullptr;
			}

			Result &= (pCommand != nullptr);

			// register the "macro" command
			pCommand = new WindowerCommand(ENGINE_KEY, CMD_MACRO, "macro",
										   "Executes a macro file.", this, false);

			if (pCommand != nullptr)
			{
				pCommand->AddStringParam("file", false, "", "the file containing the macro commands");
				pCommand->AddIntegerParam("count", true, 1L, "the number of times to execute the macro");

				if (RegisterCommand(pCommand) == false)
				{
					delete pCommand;
					pCommand = nullptr;
				}
			}

			Result &= (pCommand != nullptr);

			// register the "wait" command
			pCommand = new WindowerCommand(ENGINE_KEY, CMD_WAIT, "wait",
										   "Waits the specified number of milliseconds.", this);

			if (pCommand != nullptr)
			{
				pCommand->AddIntegerParam("wait", false, 500L, "the number of milliseconds to wait");
				pCommand->SetRestricted(false);

				if (RegisterCommand(pCommand) == false)
				{
					delete pCommand;
					pCommand = nullptr;
				}
			}

			Result &= (pCommand != nullptr);

			// register the "expect" command
			pCommand = new WindowerCommand(ENGINE_KEY, CMD_WAIT_MSG, "expect",
										   "Waits until the expected message is received.", this);

			if (pCommand != nullptr)
			{
				pCommand->AddStringParam("condition", true, "*", "the message to expect (* means any)");
				pCommand->SetRestricted(false);

				if (RegisterCommand(pCommand) == false)
				{
					delete pCommand;
					pCommand = nullptr;
				}
			}

			Result &= (pCommand != nullptr);

			// register the "if" command
			pCommand = new WindowerCommand(ENGINE_KEY, CMD_MACRO_IF, "if", "Sets a conditional execution.", this);

			if (pCommand != nullptr)
			{
				pCommand->AddStringParam("condition",  false, "", "a conditional expression");
				pCommand->AddStringParam("action",     false, "", "the action to execute if the conditions are met");
				pCommand->AddIntegerParam("delay",     true,  0,  "the delay to add after executing the action");
				pCommand->AddIntegerParam("overwrite", true,  0,  "flag specifying if the next action in the macro is skipped should the conditions be met");
				pCommand->SetRestricted(true);

				if (RegisterCommand(pCommand) == false)
				{
					delete pCommand;
					pCommand = nullptr;
				}
			}

			Result &= (pCommand != nullptr);

			// register the "endif" command
			pCommand = new WindowerCommand(ENGINE_KEY, CMD_MACRO_ENDIF, "endif", "Ends a block of conditional execution.", this);

			if (pCommand != nullptr)
			{
				pCommand->SetRestricted(true);

				if (RegisterCommand(pCommand) == false)
				{
					delete pCommand;
					pCommand = nullptr;
				}
			}

			Result &= (pCommand != nullptr);

			// register the "key" command
			pCommand = new WindowerCommand(ENGINE_KEY, CMD_KEY_PRESS, "key",
										   "Simulates a key press.", this);

			if (pCommand != nullptr)
			{
				pCommand->AddIntegerParam("key", false, 0L, "the code of the key to press");
				pCommand->AddIntegerParam("count", true, 1L, "the number of times to key is pressed");
				pCommand->AddIntegerParam("delay", true, 500L, "the delay between keypresses");
				pCommand->SetRestricted(false);

				if (RegisterCommand(pCommand) == false)
				{
					delete pCommand;
					pCommand = nullptr;
				}
			}

			Result &= (pCommand != nullptr);

			// register the "abort" command
			pCommand = new WindowerCommand(ENGINE_KEY, CMD_MACRO_ABORT, "abort",
										   "Aborts the macro currently executed, if any.", this);

			if (pCommand != nullptr && RegisterCommand(pCommand) == false)
			{
				delete pCommand;
				pCommand = nullptr;
			}

			Result &= (pCommand != nullptr);
			
			// register the "exit" command
			pCommand = new WindowerCommand(ENGINE_KEY, CMD_EXIT, "exit",
										   "Forces the game to exit.", this);

			if (pCommand != nullptr && RegisterCommand(pCommand) == false)
			{
				delete pCommand;
				pCommand = nullptr;
			}

			Result &= (pCommand != nullptr);

			return Result;
		}

		return false;
	}

	void CmdLineCore::ParseLine(const std::string &line)
	{
		WindowerCommand command;
		std::string Feedback;
		const char *pFind;
		long wait = 0L;
		size_t s = 0UL;
		
		if (m_pCommandParser->ParseCommand(line.c_str(), command, nullptr, s) == CommandParser::PARSER_RESULT_SUCCESS)
		{
			command.Execute(Feedback);
		}
		else
		{
			m_pEngine->LockMacroThread();
			InjectCommand(line);
			m_pEngine->UnlockMacroThread();
		}

		pFind = strstr(line.c_str(), "<wait.");

		if (pFind != nullptr && sscanf_s(pFind, "<wait.%d>", &wait) == 1)
			MacroWait(wait * 1000);
	}

	void CmdLineCore::AbortMacro()
	{
		m_pEngine->UnlockMacroThread();

		if (m_pCondition != nullptr)
		{
			delete m_pCondition;
			m_pCondition = nullptr;
		}
	}

	void CmdLineCore::MacroWait(long wait) const
	{
		if (wait <= 50L)
			wait = 50L;

		::Sleep(wait + std::rand() % 15);
	}

	bool CmdLineCore::ExecuteMacroFile(const string_t &macroFile_in, unsigned long repeat)
	{
		if (m_pCondition != nullptr)
		{
			delete m_pCondition;
			m_pCondition = nullptr;
		}

		if (m_pTextCmd != nullptr)
		{
			for (unsigned long i = 1; i <= repeat && m_pEngine->IsMacroThreadActive(); ++i)
			{
				std::ifstream infile(macroFile_in);

				while (m_pEngine->IsMacroThreadSuspended())
				{
					::Sleep(250);
					continue;
				}

				if (infile.bad() == false)
				{
					bool skip = false;
					std::string line;
					
					m_pEngine->UpdateMacroProgress(i, repeat, false);

					while (std::getline(infile, line) && m_pEngine->IsMacroThreadActive())
					{
						while (m_pEngine->IsMacroThreadSuspended())
							::Sleep(250);

						skip = false;

						if (strstr(line.c_str(), "endif") == nullptr && m_pCondition != nullptr
						 && m_pCondition->IsTrue(m_pEngine->GetCraftingCondition()))
						{
							skip = m_pCondition->Overwrite;
							ParseLine(m_pCondition->Action);
						}
						
						if (skip == false)
							ParseLine(line);
					}
				}				
			}

			m_pEngine->UpdateMacroProgress(repeat + 1, repeat, false);

			return true;
		}
		
		return false;
	}

	void CmdLineCore::SetMacroDirectry(const string_t & Directory_in)
	{
		m_MacroDirectory = Directory_in;
		normalize_path(m_MacroDirectory, false, true);
	}
		
	/*! \brief Inserts a command in the collection of registered commands
		\param[in] pCommand_in : the command to add
	*/
	bool CmdLineCore::RegisterCommand(WindowerCommand *pCommand_in)
	{
		if (m_pCommandDispatcher != nullptr)
			return m_pCommandDispatcher->RegisterCommand(pCommand_in);

		return false;
	}

	/*! \brief Removes a command from the collection of registered commands
		\param[in] pCommand_in : the command to remove
	*/
	bool CmdLineCore::UnregisterCommand(WindowerCommand *pCommand_in)
	{
		bool Result = false;

		if (m_pCommandDispatcher != nullptr)
		{
			Result = m_pCommandDispatcher->UnregisterCommand(pCommand_in);
			delete pCommand_in;
		}

		return Result;
	}

	/*! \brief Unregisters the commands with the command dispatcher
		\return true if all the commands were unregistered successfully; false otherwise
	*/
	bool CmdLineCore::UnregisterCommands()
	{
		if (m_pCommandDispatcher != nullptr)
		{
			WindowerCommand *pCommand;
			bool Result = true;

			pCommand = m_pCommandDispatcher->FindCommand("help");
			Result &= UnregisterCommand(pCommand);

			pCommand = m_pCommandDispatcher->FindCommand("unload");
			Result &= UnregisterCommand(pCommand);

			pCommand = m_pCommandDispatcher->FindCommand("load");
			Result &= UnregisterCommand(pCommand);

			pCommand = m_pCommandDispatcher->FindCommand("list");
			Result &= UnregisterCommand(pCommand);

			pCommand = m_pCommandDispatcher->FindCommand("exit");
			Result &= UnregisterCommand(pCommand);

			return Result;
		}

		return false;
	}

	/*! \brief Verifies that the specified command is valid and is compatible with the invoker
		\param[in] pCommand_in : the command to validate
		\return true if the command is valid; false otherwise
	*/
	bool CmdLineCore::IsCommandValid(const WindowerCommand *pCommand_in) const
	{
		if (m_pCommandDispatcher != nullptr)
			return m_pCommandDispatcher->IsCommandValid(pCommand_in);

		return false;
	}

	bool CmdLineCore::FindMacroFile(string_t &MacroFile_in_out)
	{
		const TCHAR *pPath = MacroFile_in_out.c_str();

		if (::GetFileAttributes(pPath) != INVALID_FILE_ATTRIBUTES)
			return true;

		auto pathParts = split_path(MacroFile_in_out);
		auto filePart = pathParts.back();
		
		MacroFile_in_out = m_MacroDirectory + filePart;
		
		return (::GetFileAttributes(MacroFile_in_out.c_str()) != INVALID_FILE_ATTRIBUTES);
	}
	
	/*! \brief Executes the command specified by its ID
		\param[in] CmdID_in : the ID of the command to execute
		\param[in] Command_in : the command to execute
		\param[out] Feedback_out : the result of the execution
		\return true if the command was executed successfully; false otherwise
	*/
	bool CmdLineCore::ExecuteCommand(INT_PTR CmdID_in, const WindowerCommand &Command_in, std::string& Feedback_out)
	{
		bool Result = false;

		switch(CmdID_in)
		{
			case CMD_HELP:
				return ShowCommandHelp(Command_in.GetStringValue("command"), Feedback_out);
			break;
			case CMD_LOAD_PLUGIN:
				if (m_pEngine != nullptr)
				{
					std::string PluginName = Command_in.GetStringValue("plugin");
					string_t PluginNameW;

					// >>> Critical section
					m_pEngine->LockEngineThread();

					if (m_pEngine->LoadPlugin(convert_utf8(PluginName, PluginNameW)))
					{
						format(Feedback_out, "The plugin '%s' was loaded successfully.", PluginName.c_str());
						Result = true;
					}
					else
					{
						format(Feedback_out, "The plugin '%s' couldn't be loaded.", PluginName.c_str());
						Result = false;
					}

					m_pEngine->UnlockEngineThread();
					// Critical section <<<

					return Result;
				}
			break;
			case CMD_UNLOAD_PLUGIN:
				if (m_pEngine != nullptr)
				{
					std::string PluginName = Command_in.GetStringValue("plugin");
					string_t PluginNameW;

					// >>> Critical section
					m_pEngine->LockEngineThread();

					if (m_pEngine->UnloadPlugin(convert_utf8(PluginName, PluginNameW)))
					{
						format(Feedback_out, "The plugin '%s' was unloaded successfully.", PluginName.c_str());
						Result = true;
					}
					else
					{
						format(Feedback_out, "The plugin '%s' couldn't be unloaded.", PluginName.c_str());
						Result = false;
					}

					m_pEngine->UnlockEngineThread();
					// Critical section <<<

					return Result;
				}
			break;
			case CMD_LIST_PLUGINS:
			{
				return (m_pEngine ? m_pEngine->ListPlugins(Feedback_out) : false);
			}
			case CMD_CONFIGURE:
			{
				if (m_pEngine != nullptr)
				{
					std::string PluginName = Command_in.GetStringValue("plugin");
					string_t PluginNameW;

					if (m_pEngine->ConfigurePlugin(convert_utf8(PluginName, PluginNameW)))
					{
						format(Feedback_out, "The plugin '%s' was configured successfully.", PluginName.c_str());
						Result = true;
					}
					else
					{
						format(Feedback_out, "The plugin '%s' couldn't be configured.", PluginName.c_str());
						Result = false;
					}

					return Result;
				}
			}
			break;
			case CMD_MACRO:
				if (m_pEngine != nullptr)
				{
					std::string MacroFile = Command_in.GetStringValue("file");
					long repeat = Command_in.GetIntegerValue("count");
					string_t MacroFileW;
						
					convert_utf8(MacroFile, MacroFileW);
					Result = false;

					if (FindMacroFile(MacroFileW))
					{
						format(Feedback_out, "The macro '%s' has been queued for execution %ld time(s).", MacroFile.c_str(), repeat);
						Result = m_pEngine->CreateMacroThread(MacroFileW, repeat);
					}
					else
						format(Feedback_out, "The macro '%s' failed to execute.", MacroFile.c_str());

					return Result;
				}
			break;
			case CMD_MACRO_ABORT:
				if (m_pEngine != nullptr)
				{
					Feedback_out = "Macro aborted.";

					return m_pEngine->AbortMacro();
				}
			break;
			case CMD_KEY_PRESS:
				if (m_pEngine != nullptr)
				{
					long repeat = Command_in.GetIntegerValue("count");
					long delay = Command_in.GetPointerValue("delay");
					long key = Command_in.GetPointerValue("key");					

					Feedback_out = "";
					Result = true;

					return m_pEngine->PressKey(key, delay, repeat);
				}
			break;
			case CMD_WAIT:
				if (m_pEngine != nullptr)
				{
					MacroWait(Command_in.GetIntegerValue("wait"));
					Feedback_out = "";

					return true;
				}
			break;
			case CMD_MACRO_IF:
			{
				std::string Condition = Command_in.GetStringValue("condition");
				const char *pFind = strstr(Condition.c_str(), CRAFTING_CONDITION_MARKER);

				if (m_pCondition != nullptr)
				{
					delete m_pCondition;
					m_pCondition = nullptr;
				}

				if (pFind != nullptr)
				{
					std::string values(pFind + CRAFTING_CONDITION_LENGTH);
					std::list<std::string> valueList;					
					size_t count = 0;

					count = tokenize(values, valueList, ",", "");

					if (count > 0UL)
					{
						std::string Action = Command_in.GetStringValue("action");

						pFind = strstr(Action.c_str(), CRAFTING_ABILITY_MARKER);

						if (pFind != nullptr)
						{
							bool Overwrite = (Command_in.GetIntegerValue("overwrite") != 0L);
							long Delay = Command_in.GetIntegerValue("delay");
							std::string GameCmd;

							format(GameCmd, "/action \"%s\" <me> <wait.%ld>", pFind + CRAFTING_ABILITY_LENGTH, Delay);
							m_pCondition = new ConditionalAction(GameCmd, Overwrite, valueList);
						}
					}
				}
			}
			break;
			case CMD_MACRO_ENDIF:
				if (m_pCondition != nullptr)
				{
					delete m_pCondition;
					m_pCondition = nullptr;
				}
			break;
			case CMD_WAIT_MSG:
				if (m_pEngine != nullptr && m_pEngine->IsMacroThreadActive())
					return m_pEngine->SuspendMacroThread(Command_in.GetStringValue("condition"));
			break;
			case CMD_EXIT:
				return (m_pEngine ? m_pEngine->Exit(Feedback_out) : false);
			break;
		}

		return false;
	}

	/*! \brief Displays the help message for the specified command
			   or all the commands if no command is specified
		\param[in] CommandName_in : the name of the command for which to obtain help
		\param[out] HelpMsg_out : a string receiving the help message
		\return true if successful; false otherwise
	*/
	bool CmdLineCore::ShowCommandHelp(const std::string& CommandName_in, std::string& HelpMsg_out)
	{
		if (m_pCommandDispatcher != nullptr)
		{
			if (CommandName_in.empty() == false)
			{
				WindowerCommand *pCommand;

				// display help for the specified command if found
				if ((pCommand = m_pCommandDispatcher->FindCommand(CommandName_in)) != nullptr)
				{
					pCommand->Output(HelpMsg_out);

					return true;
				}
				else
				{
					format(HelpMsg_out, "The command '%s' doesn't exist.", CommandName_in.c_str());

					return false;
				}
			}
			else
			{
				// display help for all the public commands
				const RegisteredCommands &Commands = m_pCommandDispatcher->GetCommands();
				RegisteredCommands::const_iterator CmdIt, EndIt = Commands.cend();
				WindowerCommand *pCommand;

				HelpMsg_out = "Available commands:";

				for (CmdIt = Commands.cbegin(); CmdIt != EndIt; ++CmdIt)
				{
					pCommand = CmdIt->second;

					if (pCommand != nullptr && pCommand->IsPublic() && pCommand->IsRestricted() == false)
					{
						if (HelpMsg_out.empty() == false)
							HelpMsg_out += '\n';

						pCommand->Output(HelpMsg_out);
					}
				}

				return true;
			}
		}

		 return false;
	}

	int CmdLineCore::InjectCommand(const std::string &Cmd_in)
	{
		if (m_Context.IsSet() == false || m_Context->m_pTextCmd == nullptr)
			return 0;

		StringNode Cmd;

		InitStringNode(Cmd, Cmd_in.c_str());

		return ProcessCmdHook(m_Context->m_pTextCmd, &Cmd, m_Context->m_pTextCmdUnknown);
	}
}