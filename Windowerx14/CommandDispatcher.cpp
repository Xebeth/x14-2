/**************************************************************************
	created		:	2011-06-09
	filename	: 	CommandDispatcher.cpp
	author		:	Xebeth`
	copyright	:	North Edge (2011)
	purpose		:	Command dispatcher
**************************************************************************/
#include "stdafx.h"
#include <PluginFramework.h>
#include <NonCopyable.h>
#include <HookEngine.h>

#include "BaseEngine.h"
#include "PluginEngine.h"
#include "WindowerEngine.h"

#include "ICoreModule.h"
#include "WindowerCore.h"
#include "WindowerCommand.h"
#include "CommandDispatcher.h"

namespace Windower
{
	CommandDispatcher::CommandDispatcher(PluginEngine &Engine_in)
		: WindowerCore(Engine_in)
	{
		RegisterService(_T("RegisterCommand"), true);
		RegisterService(_T("UnregisterCommand"), true);

		m_ValidKeys.insert(0xDEADBEEF); // Windower Engine
		m_ValidKeys.insert(0xAF8B3EE1); // Timestamp 1.0.0
		m_ValidKeys.insert(0x18E5F530); // AutoLogin 1.0.0

		m_Commands["help"] = NULL;
	}

	CommandDispatcher::~CommandDispatcher()
	{
		if (m_Commands.empty() == false)
		{
			RegisteredCommands::iterator Iter;

			for (Iter = m_Commands.begin(); Iter != m_Commands.end(); ++Iter)
				delete Iter->second;
		}
	}

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

	bool CommandDispatcher::UnregisterCommand(unsigned long RegistrationKey_in, const std::string &CommandName_in)
	{
		RegisteredCommands::const_iterator Iter = m_Commands.find(CommandName_in);

		if (Iter == m_Commands.end() && Iter->second != NULL)
		{
			if (Iter->second->RegistrationKey == RegistrationKey_in)
			{
				RemoveCommand(Iter->second);

				return true;
			}
		}

		return false;
	}

	void CommandDispatcher::InsertCommand(WindowerCommand *pCommand_in)
	{
		if (pCommand_in != NULL)
			m_Commands[pCommand_in->Name] = pCommand_in;
	}

	void CommandDispatcher::RemoveCommand(WindowerCommand *pCommand_in)
	{
		if (pCommand_in != NULL)
		{
			m_Commands.erase(pCommand_in->Name);
			delete pCommand_in;
		}
	}

	bool CommandDispatcher::Invoke(const string_t &ServiceName_in,
								   const PluginFramework::ServiceParam &Params_in,
								   PluginFramework::ServiceParam &Results_out)
	{
		ModuleServices::iterator Iter = m_Services.find(ServiceName_in);

		// the service exists and can be invoked
		if (Iter != m_Services.end() && Iter->second.InvokePermission)
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

	int CommandDispatcher::Dispatch(const WindowerCommand &Command_in)
	{
		if (Command_in.CommandCallbackFunc != NULL)
			return Command_in.CommandCallbackFunc(&Command_in);
		else
			return DISPATCHER_RESULT_INVALID_CALL;
	}
}