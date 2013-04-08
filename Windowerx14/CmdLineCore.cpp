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

#include "WindowerSettings.h"
#include "WindowerEngine.h"
#include "CommandDispatcher.h"
#include "CommandParser.h"

namespace Windower
{
	CommandDispatcher* CmdLineCore::m_pCommandDispatcher = NULL;
	fnProcessCmd CmdLineCore::m_pProcessCmdTrampoline = NULL;
	CommandParser * CmdLineCore::m_pCommandParser = NULL;

	/*! \brief CmdLineCore constructor
		\param[in,out] Engine_in_out : the windower engine
		\param[in,out] HookManager_in_out : the hook manager
	*/
	CmdLineCore::CmdLineCore(WindowerEngine &Engine_in_out, HookEngine &HookManager_in_out)
		: WindowerCore(_T("CmdLine"), Engine_in_out, HookManager_in_out) {}
	
	//! \brief CmdLineCore destructor
	CmdLineCore::~CmdLineCore()
	{
		// unregister the commands
		UnregisterCommands();

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
		\param[in] pArgs_in : args?
	*/
	bool WINAPI CmdLineCore::ProcessCmdHook(LPVOID pThis_in_out, StringNode* pCmd_in_out, char **pRawCmd_in)
	{
		if (m_pProcessCmdTrampoline != NULL)
		{
			std::string Feedback;

			if (FilterCommands(pCmd_in_out, Feedback))
			{
				// insert the echo command
				Feedback.insert(0U, "/echo ");
				// duplicate the buffer
				char *pFeedback = _strdup(Feedback.c_str());
				StringNode OriginalCmd = UpdateNode(pFeedback, Feedback.size() + 1, *pCmd_in_out);
				bool Result = m_pProcessCmdTrampoline(pThis_in_out, pCmd_in_out, pRawCmd_in);

				*pCmd_in_out = OriginalCmd;
				free(pFeedback);

				return Result;
			}

			return m_pProcessCmdTrampoline(pThis_in_out, pCmd_in_out, pRawCmd_in);
		}

		return false;
	}

	/*! \brief Filters the commands aimed at Windower
		\param[in] pCmd_in : the command text
		\param[out] Feedback_out : feedback of the command execution
		\return true if the command was executed; false otherwise
	*/
	bool CmdLineCore::FilterCommands(const StringNode *pCmd_in, std::string &Feedback_out)
	{
		if (pCmd_in != NULL && pCmd_in->pResBuf != NULL && m_pCommandParser != NULL)
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

					if (m_pCommandParser->ParseCommand(pCmd_in->pResBuf + 2, Command, &pFeedbackMsg, dwNewSize) == CommandParser::PARSER_RESULT_SUCCESS)
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
			m_pCommandDispatcher = new CommandDispatcher(m_Engine, m_HookManager);
			m_pCommandParser = new CommandParser(m_Engine, m_HookManager, *m_pCommandDispatcher);
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
		switch(CmdID_in)
		{
			case CMD_HELP:
				return ShowCommandHelp(Command_in.GetStringValue("command"), Feedback_out);
			break;
			case CMD_LOAD_PLUGIN:
			{
				std::string PluginName = Command_in.GetStringValue("plugin");
				string_t PluginNameW;

				if (m_Engine.LoadPlugin(convert_utf8(PluginName, PluginNameW)))
				{
					format(Feedback_out, "The plugin '%s' was loaded successfully.", PluginName.c_str());

					return true;
				}
				else
				{
					format(Feedback_out, "The plugin '%s' couldn't be loaded.", PluginName.c_str());

					return false;
				}
			}
			break;
			case CMD_UNLOAD_PLUGIN:
			{
				std::string PluginName = Command_in.GetStringValue("plugin");
				string_t PluginNameW;

				if (m_Engine.UnloadPlugin(convert_utf8(PluginName, PluginNameW)))
				{
					format(Feedback_out, "The plugin '%s' was unloaded successfully.", PluginName.c_str());

					return true;
				}
				else
				{
					format(Feedback_out, "The plugin '%s' couldn't be unloaded.", PluginName.c_str());

					return false;
				}
			}
			break;
			case CMD_LIST_PLUGINS:
			{
				return m_Engine.ListPlugins(Feedback_out);
			}
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
				RegisteredCommands::const_iterator Iter;
				WindowerCommand *pCommand;

				HelpMsg_out = "Available commands:";

				for (Iter = Commands.begin(); Iter != Commands.end(); ++Iter)
				{
					pCommand = Iter->second;

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
}