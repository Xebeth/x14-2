/**************************************************************************
	created		:	2013-03-24
	filename	: 	CmdLineCore.cpp
	author		:	Xebeth`
	copyright	:	North Edge (2013)
	purpose		:	Command line module
**************************************************************************/
#include "stdafx.h"

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
	CmdLineCore::CallingContext * CmdLineCore::m_pContext = NULL;
	LPVOID CmdLineCore::m_pTextCmd = NULL;

	/*! \brief CmdLineCore constructor
		\param[in,out] Engine_in_out : the windower engine
		\param[in,out] HookManager_in_out : the hook manager
	*/
	CmdLineCore::CmdLineCore()
		: WindowerCore(_T(CMD_LINE_MODULE)), m_pCommandDispatcher(NULL),
		  m_pProcessCmdTrampoline(NULL), m_pCommandParser(NULL) {}

	//! \brief CmdLineCore destructor
	CmdLineCore::~CmdLineCore()
	{
		// unregister the commands
		UnregisterCommands();

		if (m_pContext != NULL)
		{
			delete m_pContext;
			m_pContext = NULL;
		}

		if (m_pCommandParser != NULL)
		{
			delete m_pCommandParser;
			m_pCommandParser = NULL;
		}

		if (m_pCommandDispatcher != NULL)
		{
			delete m_pCommandDispatcher;
			m_pCommandDispatcher = NULL;
		}
	}

	/*! \brief Process a command typed by the player
		\param[in,out] pThis_in_out : the caller object
		\param[in,out] pCmd_in_out : a string node containing the command typed
		\param[in] pUnknown_in : unknown object (contains pThis_in_out at offset 0x41950)
	*/
	bool WINAPI CmdLineCore::ProcessCmdHook(LPVOID pThis_in_out, StringNode* pCmd_in_out, LPVOID pUnknown_in)
	{
		bool Result = false;

		// update the text command pointer
		if (m_pContext != NULL && m_pContext->m_pTrampoline != NULL)
		{
			std::string Feedback;

			if (FilterCommands(pCmd_in_out, Feedback))
			{
				// skip command processing
				return FormatChatMsgService::InjectMessage(Feedback);
			}
			else if (pThis_in_out != NULL)
			{
				// call the trampoline to process the command
				Result = m_pContext->m_pTrampoline(pThis_in_out, pCmd_in_out, pUnknown_in);
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
		if (pCmd_in != NULL && pCmd_in->pResBuf != NULL
		 && m_pContext != NULL && m_pContext->m_pParser != NULL)
		{
			// the message starts with 2 forward slashes => expect a command
			if (pCmd_in->dwSize != 0 && strstr(pCmd_in->pResBuf, "//") == pCmd_in->pResBuf)
			{
				bool Result = false;

				if (pCmd_in->dwSize > 2)
				{
					char *pFeedbackMsg = NULL;
					WindowerCommand Command;
					DWORD dwNewSize = 0UL;

					if (m_pContext->m_pParser->ParseCommand(pCmd_in->pResBuf + 2, Command, &pFeedbackMsg, dwNewSize) == CommandParser::PARSER_RESULT_SUCCESS)
					{
						Command.Execute(Feedback_out);
						Result = true;
					}
					else if (pFeedbackMsg != NULL)
					{
						Feedback_out.assign(pFeedbackMsg);
						Result = true;
					}

					if (Result)
					{
						std::string Cmd(pCmd_in->pResBuf);

						// add the command to the feedback
						replace<char>(Cmd, "//", ">> ");
						Cmd += '\n';
						// insert it at the beginning
						Feedback_out.insert(0, Cmd);
					}

					if (pFeedbackMsg != NULL)
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
									PROCESS_CMD_OPCODES_HOOK_SIZE);
	}

	/*! \brief Callback invoked when the hooks of the module are installed
		\param[in] HookManager_in : the hook manager
	*/
	void CmdLineCore::OnHookInstall(HookEngineLib::IHookManager &HookManager_in)
	{
		m_pProcessCmdTrampoline = (fnProcessCmd)HookManager_in.GetTrampolineFunc(PROCESS_CMD_HOOK);

		if (m_pProcessCmdTrampoline != NULL)
		{
			// create the command dispatcher and parser
			m_pCommandDispatcher = new CommandDispatcher;
			m_pCommandParser = new CommandParser(*m_pCommandDispatcher);
			// create the calling context for the hook
			m_pContext = new CallingContext(m_pProcessCmdTrampoline, m_pCommandParser);
			// register the built-in commands
			RegisterCommands();
		}
	}

	/*! \brief Registers the commands with the command dispatcher
		\return true if all the commands were registered successfully; false otherwise
	*/
	bool CmdLineCore::RegisterCommands()
	{
		if (m_pCommandDispatcher != NULL)
		{
			bool Result = true;
			// register the "help" command
			WindowerCommand *pCommand = new WindowerCommand(ENGINE_KEY, CMD_HELP, "help",
															"Provides help with the specified command (all by default)", this);

			if (pCommand != NULL)
			{
				// register the parameter
				pCommand->AddStringParam("command", true, "", "the name of the command for which to obtain help");

				if (RegisterCommand(pCommand) == false)
					delete pCommand;
			}

			Result &= (pCommand != NULL);

			// register the "load" command
			pCommand = new WindowerCommand(ENGINE_KEY, CMD_LOAD_PLUGIN, "load",
										   "Loads a plugin given its name.", this);

			if (pCommand != NULL)
			{
				pCommand->AddStringParam("plugin", false, "", "the name of the plugin to load");

				if (RegisterCommand(pCommand) == false)
				{
					delete pCommand;
					pCommand = NULL;
				}
			}

			Result &= (pCommand != NULL);
			// register the "unload" command
			pCommand = new WindowerCommand(ENGINE_KEY, CMD_UNLOAD_PLUGIN, "unload",
										   "Unloads a plugin given its name.", this);

			if (pCommand != NULL)
			{
				pCommand->AddStringParam("plugin", false, "", "the name of the plugin to unload");

				if (RegisterCommand(pCommand) == false)
				{
					delete pCommand;
					pCommand = NULL;
				}
			}

			Result &= (pCommand != NULL);

			// register the "list" command
			pCommand = new WindowerCommand(ENGINE_KEY, CMD_LIST_PLUGINS, "list",
										   "Gives a list of the available plugins.", this);

			if (pCommand != NULL && RegisterCommand(pCommand) == false)
			{
				delete pCommand;
				pCommand = NULL;
			}

			Result &= (pCommand != NULL);

			// register the "exit" command
			pCommand = new WindowerCommand(ENGINE_KEY, CMD_EXIT, "exit",
										   "Forces the game to exit.", this);

			if (pCommand != NULL && RegisterCommand(pCommand) == false)
			{
				delete pCommand;
				pCommand = NULL;
			}

			Result &= (pCommand != NULL);

			return Result;
		}

		return false;
	}

	/*! \brief Inserts a command in the collection of registered commands
		\param[in] pCommand_in : the command to add
	*/
	bool CmdLineCore::RegisterCommand(WindowerCommand *pCommand_in)
	{
		if (m_pCommandDispatcher != NULL)
			return m_pCommandDispatcher->RegisterCommand(pCommand_in);

		return false;
	}

	/*! \brief Removes a command from the collection of registered commands
		\param[in] pCommand_in : the command to remove
	*/
	bool CmdLineCore::UnregisterCommand(WindowerCommand *pCommand_in)
	{
		bool Result = false;

		if (m_pCommandDispatcher != NULL)
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
		if (m_pCommandDispatcher != NULL)
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
		if (m_pCommandDispatcher != NULL)
			return m_pCommandDispatcher->IsCommandValid(pCommand_in);

		return false;
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
				if (m_pEngine != NULL)
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
				if (m_pEngine != NULL)
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
		if (m_pCommandDispatcher != NULL)
		{
			if (CommandName_in.empty() == false)
			{
				WindowerCommand *pCommand;

				// display help for the specified command if found
				if ((pCommand = m_pCommandDispatcher->FindCommand(CommandName_in)) != NULL)
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

					if (pCommand != NULL && pCommand->IsPublic() && pCommand->IsRestricted() == false)
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

	bool CmdLineCore::InjectCommand(const std::string &Cmd_in)
	{
		StringNode Cmd;

		InitStringNode(Cmd, Cmd_in);

		return ProcessCmdHook(m_pTextCmd, &Cmd, NULL);
	}
}