/**************************************************************************
	created		:	2011-06-09
	filename	: 	CommandDispatcher.cpp
	author		:	Xebeth`
	copyright	:	North Edge (2011)
	purpose		:	Command dispatcher
**************************************************************************/
#include "stdafx.h"
#include <PluginFramework.h>
#include <HookEngine.h>

#include "WindowerEngine.h"
#include "CommandDispatcher.h"

namespace Windower
{
	/*! \brief CommandDispatcher constructor
		\param[in] Engine_in_out : the windower engine
	*/
	CommandDispatcher::CommandDispatcher(PluginEngine &Engine_in_out)
		: WindowerCore(Engine_in_out)
	{
		CommandParams Params;

		RegisterService(_T("UnregisterCommand"), true);
		RegisterService(_T("RegisterCommand"), true);
		// register the module
		m_Engine.RegisterModule(_T("CommandDispatcher"), this);

		m_ValidKeys.insert(0xDEADBEEF);	// Windower Engine
		m_ValidKeys.insert(0xAF8B3EE1);	// Timestamp
		m_ValidKeys.insert(0x18E5F530);	// AutoLogin
		m_ValidKeys.insert(0xEB71A021);	// ExpWatch

		WindowerCommand *pCommand = new WindowerCommand(0xDEADBEEF, CMD_HELP, "help",
														"Provides help with the specified command (all by default)", this);

		if (pCommand != NULL)
		{
			// register the parameter
			pCommand->AddStringParam("command", true, "", "the name of the command for which to obtain help");

			if (RegisterCommand(pCommand) == false)
				delete pCommand;
		}
	}

	/*! \brief Registers the specified command in the dispatcher
		\param[in] Command_in : the command to register
		\return true if the command was registered successfully; false otherwise
	*/
	bool CommandDispatcher::RegisterCommand(WindowerCommand *pCommand_in)
	{
		if (pCommand_in != NULL)
		{
			RegisteredCommands::const_iterator Iter = m_Commands.find(pCommand_in->GetName());

			if (Iter == m_Commands.end() || Iter->second == NULL)
			{
				// parameters validation
				if (pCommand_in->ValidateParameters() && IsKeyAuthorized(pCommand_in->GetKey()))
				{
					InsertCommand(pCommand_in);

					return true;
				}
			}
		}

		return false;
	}

	/*! \brief Revokes a command registration given its name
		\param[in] RegistrationKey_in : the key of the plugin revoking the command registration
		\param[in] CommandName_in : the name of the command
		\return true if the command registration was revoked; false otherwise
	*/
	bool CommandDispatcher::UnregisterCommand(DWORD RegistrationKey_in, const std::string &CommandName_in)
	{
		RegisteredCommands::const_iterator Iter = m_Commands.find(CommandName_in);

		if (Iter != m_Commands.end() && Iter->second != NULL)
		{
			if (Iter->second->IsKeyMatching(RegistrationKey_in))
			{
				RemoveCommand(Iter->second);

				return true;
			}
		}

		return false;
	}

	/*! \brief Inserts a command in the collection of registered commands
		\param[in] pCommand_in : the command to add
	*/
	void CommandDispatcher::InsertCommand(WindowerCommand *pCommand_in)
	{
		if (pCommand_in != NULL)
			m_Commands[pCommand_in->GetName()] = pCommand_in;
	}

	/*! \brief Removes a command from the collection of registered commands
		\param[in] pCommand_in : the command to remove
	*/
	void CommandDispatcher::RemoveCommand(WindowerCommand *pCommand_in)
	{
		if (pCommand_in != NULL)
			m_Commands.erase(pCommand_in->GetName());
	}

	/*! \brief Invokes a command registered with the specified module
		\param[in] ServiceName_in : the name of the service
		\param[in] Params_in : the input parameters
		\return true if the command was invoked successfully; false otherwise
	*/
	bool CommandDispatcher::Invoke(const string_t &ServiceName_in,
								   const PluginFramework::ServiceParam &Params_in)
	{
		ModuleServices::iterator Iter = m_Services.find(ServiceName_in);

		// the service exists and can be invoked
		if (Iter != m_Services.end() && Iter->second->CanInvoke())
		{
			if (ServiceName_in.compare(_T("RegisterCommand")) == 0
			 && Params_in.DataType.compare(_T("WindowerCommand")) == 0
			 && Params_in.pData != NULL)
			{
				WindowerCommand *pCommand = reinterpret_cast<WindowerCommand*>(Params_in.pData);

				return RegisterCommand(pCommand);
			}
			else if (ServiceName_in.compare(_T("UnregisterCommand")) == 0
				  && Params_in.DataType.compare(_T("UnregisterParam")) == 0
				  && Params_in.pData != NULL)
			{
				UnregisterParam Param = *reinterpret_cast<UnregisterParam*>(Params_in.pData);

				return UnregisterCommand(Param.RegistrationKey, Param.CommandName);
			}
		}

		return false;
	}

	/*! \brief Retrieves the command specified by its name
		\param[in] Name_in : the name of the command
		\return a pointer to the command if found; NULL otherwise
	*/
	WindowerCommand* CommandDispatcher::FindCommand(const std::string &Name_in)
	{
		RegisteredCommands::const_iterator CmdIter = m_Commands.find(Name_in);

		if (CmdIter != m_Commands.end())
			return CmdIter->second;

		return NULL;
	}

	/*! \brief Executes the command specified by its ID
		\param[in] CmdID_in : the ID of the command to execute
		\param[in] Command_in : the command to execute
		\param[out] Feedback_out : the result of the execution
		\return true if the command was executed successfully; false otherwise
	*/
	bool CommandDispatcher::ExecuteCommand(INT_PTR CmdID_in, const WindowerCommand &Command_in, std::string &Feedback_out)
	{
		switch(CmdID_in)
		{
			case CMD_HELP:
				return ShowCommandHelp(Command_in.GetStringValue("command"), Feedback_out);
		}

		return false;
	}


	/*! \brief Displays the help message for the specified command
			   or all the commands if no command is specified
		\param[in] CommandName_in : the name of the command for which to obtain help
		\param[out] HelpMsg_out : a string receiving the help message
		\return true if successful; false otherwise
	*/
	bool CommandDispatcher::ShowCommandHelp(const std::string &CommandName_in, std::string &HelpMsg_out)
	{
		if (CommandName_in.empty() == false)
		{
			WindowerCommand *pCommand;

			if ((pCommand = FindCommand(CommandName_in)) != NULL)
			{
				pCommand->Output(HelpMsg_out);

				return true;
			}
		}

		RegisteredCommands::const_iterator Iter;

		for (Iter = m_Commands.begin(); Iter != m_Commands.end(); ++Iter)
		{
			if (Iter->second != NULL)
			{
				if (HelpMsg_out.empty() == false)
					HelpMsg_out += '\n';

				Iter->second->Output(HelpMsg_out);
			}
		}
	
		return true;
	}

	/*! \brief Verifies that the specified command is valid and is compatible with the invoker
		\param[in] pCommand_in : the command to validate
		\return true if the command is valid; false otherwise
	*/
	bool CommandDispatcher::IsCommandValid(const WindowerCommand *pCommand_in)
	{
		if (pCommand_in != NULL)
		{
			const WindowerCommand *pCmd = FindCommand(pCommand_in->GetName());

			return (pCmd != NULL);
		}

		return false;
	}
}