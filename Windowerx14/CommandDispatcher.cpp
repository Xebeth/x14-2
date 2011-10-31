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

#include "WindowerSettings.h"

#include "BaseEngine.h"
#include "PluginEngine.h"
#include "WindowerEngine.h"

#include "ICoreModule.h"
#include "WindowerCore.h"
#include "WindowerCommand.h"
#include "CommandDispatcher.h"

namespace Windower
{
	/*! \brief CommandDispatcher constructor
		\param[in] Engine_in_out : the windower engine
	*/
	CommandDispatcher::CommandDispatcher(PluginEngine &Engine_in_out)
		: WindowerCore(Engine_in_out)
	{
		RegisterService(_T("RegisterCommand"), true);
		RegisterService(_T("UnregisterCommand"), true);

		m_ValidKeys.insert(0xDEADBEEF); // Windower Engine
		m_ValidKeys.insert(0xAF8B3EE1); // Timestamp 1.0.0
		m_ValidKeys.insert(0x18E5F530); // AutoLogin 1.0.0
		m_ValidKeys.insert(0xEB71A021); // ExpWatch 1.0.0
	}

	//! \brief CommandDispatcher destructor
	CommandDispatcher::~CommandDispatcher()
	{
		if (m_Commands.empty() == false)
		{
			RegisteredCommands::iterator Iter;

			for (Iter = m_Commands.begin(); Iter != m_Commands.end(); ++Iter)
				delete Iter->second;
		}
	}

	/*! \brief Registers a command with the command dispatcher
		\param[in] RegistrationKey_in : the key of the plugin registering the command
		\param[in] Name_in : the name of the command
		\param[in] Description_in : the description of the command
		\param[in] Caller_in : the caller parameters (typically a plugin instance)
		\param[in] CallbackFunc_in : function pointer to call when the command is invoked
		\param[in] MinParamsCount_in : minimal number of parameters for the command
		\param[in] MaxParamsCount_in : maximal number of parameters for the command
		\param[in] Parameters_in : a collection of parameters
		\param[in] Public_in : flag specifying if the command is listed with //help
		\param[in] Restricted_in : flag specifying if the command can be called by the user
		\return true if the command was registered successfully; false otherwise
	*/
	bool CommandDispatcher::RegisterCommand(unsigned long RegistrationKey_in, const std::string &Name_in, const std::string &Description_in,
											CallerParam Caller_in, fnCommandCallback CallbackFunc_in, unsigned int MinParamsCount_in, 
											unsigned int MaxParamsCount_in, const CommandParameters &Parameters_in, bool Public_in, bool Restricted_in)
	{
		RegisteredCommands::const_iterator Iter = m_Commands.find(Name_in);

		if (Iter == m_Commands.end() || Iter->second == NULL)
		{
			size_t ParamsCount = Parameters_in.size();

			// parameters validation
			if (MinParamsCount_in <= MaxParamsCount_in
				&& MaxParamsCount_in >= MinParamsCount_in
				&& ParamsCount >= MaxParamsCount_in
				&& IsKeyAuthorized(RegistrationKey_in))
			{
				InsertCommand(new WindowerCommand(RegistrationKey_in, Name_in, Description_in, Caller_in, CallbackFunc_in,
												  Parameters_in, MinParamsCount_in, MaxParamsCount_in, Public_in, Restricted_in));
				return true;
			}
		}

		return false;
	}

	/*! \brief Revokes a command registration given its name
		\param[in] RegistrationKey_in : the key of the plugin revoking the command registration
		\param[in] CommandName_in : the name of the command
		\return true if the command registration was revoked; false otherwise
	*/
	bool CommandDispatcher::UnregisterCommand(unsigned long RegistrationKey_in, const std::string &CommandName_in)
	{
		RegisteredCommands::const_iterator Iter = m_Commands.find(CommandName_in);

		if (Iter != m_Commands.end() && Iter->second != NULL)
		{
			if (Iter->second->RegistrationKey == RegistrationKey_in)
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
			m_Commands[pCommand_in->Name] = pCommand_in;
	}

	/*! \brief Removes a command from the collection of registered commands
		\param[in] pCommand_in : the command to remove
	*/
	void CommandDispatcher::RemoveCommand(WindowerCommand *pCommand_in)
	{
		if (pCommand_in != NULL)
		{
			m_Commands.erase(pCommand_in->Name);
			delete pCommand_in;
		}
	}

	/*! \brief Invokes a command registered with the specified module
		\param[in] ServiceName_in : the name of the service
		\param[in] Params_in : the input parameters
		\param[out] Results_out : the results from the invocation
		\return true if the command was invoked successfully; false otherwise
	*/
	bool CommandDispatcher::Invoke(const string_t &ServiceName_in,
								   const PluginFramework::ServiceParam &Params_in,
								   PluginFramework::ServiceParam &Results_out)
	{
		ModuleServices::iterator Iter = m_Services.find(ServiceName_in);

		// the service exists and can be invoked
		if (Iter != m_Services.end() && Iter->second->CanInvoke())
		{
			if (ServiceName_in.compare(_T("RegisterCommand")) == 0
			 && Params_in.DataType.compare(_T("WindowerCommand")) == 0
			 && Params_in.pData != NULL)
			{
				return RegisterCommand(*(WindowerCommand*)Params_in.pData);
			}
			else if (ServiceName_in.compare(_T("UnregisterCommand")) == 0
				  && Params_in.DataType.compare(_T("UnregisterParam")) == 0
				  && Params_in.pData != NULL)
			{
				UnregisterParam Param = *(UnregisterParam*)Params_in.pData;

				return UnregisterCommand(Param.RegistrationKey, Param.CommandName);
			}
		}

		return false;
	}

	/*! \brief Dispatches the specified command
		\param[in] Command_in : the command to dispatch
		\return DISPATCHER_RESULT_SUCCESS if the command was dispatched successfully; an error code otherwise
		\sa the DISPATCHER_RESULT enumeration
	*/
	int CommandDispatcher::Dispatch(const WindowerCommand &Command_in)
	{
		if (Command_in.CommandCallbackFunc != NULL)
			return Command_in.CommandCallbackFunc(&Command_in);
		else
			return DISPATCHER_RESULT_INVALID_CALL;
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
}