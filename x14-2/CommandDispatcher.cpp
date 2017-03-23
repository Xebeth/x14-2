/**************************************************************************
	created		:	2011-06-09
	filename	: 	CommandDispatcher.cpp
	author		:	Xebeth`
	copyright	:	North Edge (2011)
	purpose		:	Command dispatcher
**************************************************************************/
#include "stdafx.h"

#include "WindowerEngine.h"

#include "WindowerCore.h"
#include "CommandDispatcher.h"
#include "ModuleService.h"

namespace Windower
{
	/*! \brief CommandDispatcher constructor
		\param[in] Engine_in_out : the windower engine
	*/
	CommandDispatcher::CommandDispatcher()
		: WindowerCore(_T("CommandDispatcher"))
	{
		m_ValidKeys.insert(ENGINE_KEY);	// Windower Engine
		m_ValidKeys.insert(0xAF8B3EE1);	// Timestamp
		m_ValidKeys.insert(0x18E5F530);	// AutoLogin
		m_ValidKeys.insert(0xEB71A021);	// ExpWatch
		m_ValidKeys.insert(0xBACA701F); // AutoBlacklist
	}

	//! \brief CommandDispatcher destructor
	CommandDispatcher::~CommandDispatcher()
	{
		RegisteredCommands::const_iterator CmdIt, EndIt = m_Commands.cend();

		for (CmdIt = m_Commands.cbegin(); CmdIt != EndIt; ++CmdIt)
			delete CmdIt->second;

		m_Commands.clear();
	}

	/*! \brief Registers the services of the module
		\return true if the services were registered; false otherwise
	*/
	bool CommandDispatcher::RegisterServices()
	{
		bool Result = true;

		Result &= (RegisterService(_T("UnregisterCommand"), true) != nullptr);
		Result &= (RegisterService(_T("RegisterCommand"), true) != nullptr);

		return Result;
	}

	/*! \brief Registers the specified command in the dispatcher
		\param[in] Command_in : the command to register
		\return true if the command was registered successfully; false otherwise
	*/
	bool CommandDispatcher::RegisterCommand(WindowerCommand *pCommand_in)
	{
		if (pCommand_in != nullptr)
		{
			RegisteredCommands::const_iterator CmdIt = m_Commands.find(pCommand_in->GetName());

			if (CmdIt == m_Commands.cend() || CmdIt->second == nullptr)
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
	bool CommandDispatcher::UnregisterCommand(DWORD RegistrationKey_in, const std::string& CommandName_in)
	{
		RegisteredCommands::const_iterator CmdIt = m_Commands.find(CommandName_in);

		if (CmdIt != m_Commands.cend() && CmdIt->second != nullptr
		 && CmdIt->second->IsKeyMatching(RegistrationKey_in))
		{
			return UnregisterCommand(CmdIt->second);
		}

		return false;
	}

	/*! \brief Inserts a command in the collection of registered commands
		\param[in] pCommand_in : the command to add
	*/
	void CommandDispatcher::InsertCommand(WindowerCommand *pCommand_in)
	{
		if (pCommand_in != nullptr)
		{
			m_Commands[pCommand_in->GetName()] = pCommand_in;
			pCommand_in->OnRegister();
		}
	}

	/*! \brief Removes a command from the collection of registered commands
		\param[in] pCommand_in : the command to remove
	*/
	bool CommandDispatcher::UnregisterCommand(WindowerCommand *pCommand_in)
	{
		if (pCommand_in != nullptr)
		{
			m_Commands.erase(pCommand_in->GetName());

			return pCommand_in->OnUnregister();
		}

		return false;
	}

	/*! \brief Invokes a command registered with the specified module
		\param[in] ServiceName_in : the name of the service
		\param[in] Params_in : the input parameters
		\return true if the command was invoked successfully; false otherwise
	*/
	bool CommandDispatcher::Invoke(const string_t& ServiceName_in,
								   PluginFramework::ServiceParam &Params_in)
	{
		ModuleServices::const_iterator ServiceIt = m_Services.find(ServiceName_in);

		// the service exists and can be invoked
		if (ServiceIt != m_Services.cend() && ServiceIt->second->CanInvoke())
		{
			if (ServiceName_in.compare(_T("RegisterCommand")) == 0
			 && Params_in.DataType.compare(_T("WindowerCommand")) == 0
			 && Params_in.pData != nullptr)
			{
				WindowerCommand *pCommand = reinterpret_cast<WindowerCommand*>(Params_in.pData);

				return RegisterCommand(pCommand);
			}
			else if (ServiceName_in.compare(_T("UnregisterCommand")) == 0
				  && Params_in.DataType.compare(_T("UnregisterParam")) == 0
				  && Params_in.pData != nullptr)
			{
				UnregisterParam Param = *reinterpret_cast<UnregisterParam*>(Params_in.pData);

				return UnregisterCommand(Param.RegistrationKey, Param.CommandName);
			}
		}

		return false;
	}

	/*! \brief Retrieves the command specified by its name
		\param[in] Name_in : the name of the command
		\return a pointer to the command if found; nullptr otherwise
	*/
	WindowerCommand* CommandDispatcher::FindCommand(const std::string& Name_in) const
	{
		RegisteredCommands::const_iterator CmdIter = m_Commands.find(Name_in);

		if (CmdIter != m_Commands.cend())
			return CmdIter->second;

		return nullptr;
	}

	
	/*! \brief Verifies that the specified command is valid and is compatible with the invoker
		\param[in] pCommand_in : the command to validate
		\return true if the command is valid; false otherwise
	*/
	bool CommandDispatcher::IsCommandValid(const WindowerCommand *pCommand_in) const
	{
		if (pCommand_in != nullptr)
		{
			const WindowerCommand *pCmd = FindCommand(pCommand_in->GetName());

			return (pCmd != nullptr);
		}

		return false;
	}
}