/**************************************************************************
	created		:	2011-11-05
	filename	: 	CommandHandler.cpp
	author		:	Xebeth`
	copyright	:	North Edge (2011)
	purpose		:	Base class for objects handling windower commands
**************************************************************************/
#include "stdafx.h"
#include <PluginFramework.h>

#include "PluginEngine.h"
#include "WindowerCommand.h"
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
		\return a pointer to the command if found; NULL otherwise
	*/
	const WindowerCommand* CommandHandler::FindCommand(INT_PTR CommandID_in) const
	{
		HandlerCommands::const_iterator CmdIt = m_Commands.find(CommandID_in);

		if (CmdIt != m_Commands.end())
			return CmdIt->second;

		return NULL;
	}

	/*! \brief Verifies that the specified command is valid and is compatible with the invoker
		\param[in] pCommand_in : the command to validate
		\return true if the command is valid; false otherwise
	*/
	bool CommandHandler::IsCommandValid(const WindowerCommand *pCommand_in)
	{
		if (pCommand_in != NULL)
		{
			const WindowerCommand *pCmd = FindCommand(pCommand_in->GetID());

// 			if (pCmd != NULL)
// 				return pCmd->IsCompatible(pCommand_in);

			return (pCmd != NULL);
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
		if (CmdIt != m_Commands.end() && CmdIt->second != NULL)
		{
			UnregisterParam UnregParam(m_RegistrationKey, CmdIt->second->GetName());
			ServiceParam InvokeArg(_T("UnregisterParam"), &UnregParam);

			if (IPlugin::Services()->InvokeService(_T("CommandDispatcher"), _T("UnregisterCommand"), InvokeArg))
				delete CmdIt->second;

			m_Commands.erase(CmdIt);

			return true;
		}

		return false;
	}

	/*! \brief Registers a command with the command dispatcher through plugin services
		\param[in] CmdID_in : the ID of the command
		\param[in] CmdName_in : the name of the command
		\param[in] CmdDesc_in : the description of the command
		\return a pointer to the command if it was registered successfully; NULL otherwise
	*/
	WindowerCommand* CommandHandler::RegisterCommand(INT_PTR CmdID_in, const std::string &CmdName_in, const std::string &CmdDesc_in)
	{
		HandlerCommands::const_iterator CmdIt = m_Commands.find(CmdID_in);

		// check if the command already exists
		if (CmdIt == m_Commands.end())
		{
			// create the new command
			WindowerCommand *pCommand = new WindowerCommand(m_RegistrationKey, CmdID_in,
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
				pCommand = NULL;
			}

			return pCommand;
		}
		else
			return CmdIt->second;

		return NULL;
	}

	/*! \brief Register the specified command with the command dispatcher
		\param[in] pCommand_in : the command to register
		\return true if the command was registered successfully; false otherwise
	*/
	bool CommandHandler::RegisterCommand(WindowerCommand *pCommand_in)
	{
		return IPlugin::Services()->InvokeService(_T("CommandDispatcher"), _T("RegisterCommand"),
												  ServiceParam(_T("WindowerCommand"), pCommand_in));
	}

	//! \brief Removes all the registered commands
	void CommandHandler::Clear()
	{
		HandlerCommands::const_iterator CmdIt;

		while (m_Commands.empty() == false)
		{
			CmdIt = m_Commands.begin();

			RevokeCommand(CmdIt->first);
		}
	}
}