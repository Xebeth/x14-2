/**************************************************************************
	created		:	2011-11-05
	filename	: 	CommandHandler.cpp
	author		:	Xebeth`
	copyright	:	North Edge (2011)
	purpose		:	Base class for objects handling windower commands
**************************************************************************/
#include "stdafx.h"

#include "CommandParameter.h"
#include "WindowerCommand.h"
#include "ICommandHandler.h"
#include "CommandHandler.h"

namespace Windower
{
	/*! \brief CommandHandler constructor
		\param[in] RegistrationKey_in : the registration key of the plugin
		\param[in] InvokerName_in : the internal name of the plugin
	*/
	CommandHandler::CommandHandler(DWORD RegistrationKey_in, const std::string &InvokerName_in)
		: m_RegistrationKey(RegistrationKey_in), m_InvokerName(InvokerName_in) {}

	//! \brief CommandHandler destructor
	CommandHandler::~CommandHandler() { Clear(); }

	/*! \brief Retrieves a command given its ID
		\param[in] CommandID_in : the ID of the command to retrieve
		\return a pointer to the command if found; nullptr otherwise
	*/
	const WindowerCommand* CommandHandler::FindCommand(INT_PTR CommandID_in) const
	{
		HandlerCommands::const_iterator CmdIt = m_Commands.find(CommandID_in);

		if (CmdIt != m_Commands.cend())
			return CmdIt->second;

		return nullptr;
	}

	/*! \brief Verifies that the specified command is valid and is compatible with the invoker
		\param[in] pCommand_in : the command to validate
		\return true if the command is valid; false otherwise
	*/
	bool CommandHandler::IsCommandValid(const WindowerCommand *pCommand_in) const
	{
		if (pCommand_in != nullptr)
		{
			const WindowerCommand *pCmd = FindCommand(pCommand_in->GetID());

// 			if (pCmd != nullptr)
// 				return pCmd->IsCompatible(pCommand_in);

			return (pCmd != nullptr);
		}

		return false;
	}

	/*! \brief Revokes a command given its ID
		\param[in] CommandID_in : the ID of the command to revoke
		\return true if the command was revoked successfully; false otherwise
	*/
	bool CommandHandler::RevokeCommand(INT_PTR CommandID_in)
	{
		HandlerCommands::const_iterator CmdIt = m_Commands.find(CommandID_in);

		// check if the command exists
		if (CmdIt != m_Commands.cend() && CmdIt->second != nullptr)
		{
			if (UnregisterCommand(CmdIt->second))
				m_Commands.erase(CmdIt);

			return true;
		}

		return false;
	}

	/*! \brief Registers a command with the command dispatcher through plugin services
		\param[in] CmdID_in : the ID of the command
		\param[in] CmdName_in : the name of the command
		\param[in] CmdDesc_in : the description of the command
		\return a pointer to the command if it was registered successfully; nullptr otherwise
	*/
	WindowerCommand* CommandHandler::RegisterCommand(INT_PTR CmdID_in, const std::string &CmdName_in, const std::string &CmdDesc_in)
	{
		HandlerCommands::const_iterator CmdIt = m_Commands.find(CmdID_in);
		WindowerCommand *pCommand = nullptr;

		// check if the command already exists
		if (CmdIt == m_Commands.cend())
		{
			// create the new command
			pCommand = new WindowerCommand(m_RegistrationKey, CmdID_in,
										   CmdName_in, CmdDesc_in, this);

			// register the command with the command dispatcher
			if (RegisterCommand(pCommand))
			{
				// register the command in the plugin
				m_Commands[CmdID_in] = pCommand;
			}
			else
			{
				delete pCommand;
				pCommand = nullptr;
			}
		}
		else
			pCommand = CmdIt->second;

		return pCommand;
	}

	/*! \brief Register the specified command with the command dispatcher
		\param[in] pCommand_in : the command to register
		\return true if the command was registered successfully; false otherwise
	*/
	bool CommandHandler::RegisterCommand(WindowerCommand *pCommand_in)
	{
		if (pCommand_in != nullptr)
		{
  			return PluginFramework::IPlugin::InvokeService(_T("CommandDispatcher"), _T("RegisterCommand"),
														   PluginFramework::ServiceParam(_T("WindowerCommand"),
														   (LPVOID)pCommand_in));
		}

		return false;
	}

	/*! \brief Unregister the specified command with the command dispatcher
		\param[in] pCommand_in : the command to unregister
		\return true if the command was unregistered successfully; false otherwise
	*/
	bool CommandHandler::UnregisterCommand(WindowerCommand *pCommand_in)
	{
		if (pCommand_in != nullptr)
		{
			UnregisterParam UnregParam(m_RegistrationKey, pCommand_in->GetName());
			PluginFramework::ServiceParam InvokeArg(L"UnregisterParam", &UnregParam);

			if (PluginFramework::IPlugin::InvokeService(_T("CommandDispatcher"), _T("UnregisterCommand"), InvokeArg))
			{
				delete pCommand_in;

				return true;
			}
		}

		return false;
	}

	//! \brief Removes all the registered commands
	void CommandHandler::Clear()
	{
		HandlerCommands::const_iterator CmdIt;

		while (m_Commands.empty() == false)
		{
			CmdIt = m_Commands.cbegin();

			RevokeCommand(CmdIt->first);
		}
	}
}