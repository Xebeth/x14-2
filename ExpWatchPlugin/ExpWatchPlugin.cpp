/**************************************************************************
	created		:	2011-06-04
	filename	: 	ExpWatchPlugin.cpp
	author		:	Xebeth`
	copyright	:	North Edge (2011)
	purpose		:	ExpWatch plugin
**************************************************************************/
#include "stdafx.h"
#include <PluginFramework.h>
#include <FormatChatMessageHook.h>
#include <IGameChatPlugin.h>
#include "WindowerCommand.h"
#include "ExpWatchPlugin.h"

const PluginFramework::IPluginServices* PluginFramework::IPlugin::m_pPluginServices = NULL;

namespace Windower
{
	ExpWatchPlugin::ExpWatchPlugin() : IGameChatPlugin(), m_TotalExp(0.f), m_bStarted(false),
		m_KillCounter(0L), m_AvgExpPerKill(0.f), m_AvgExpPerHour(0.f), m_StartTime(0UL) {}

	/*! \brief Creates an instance of ExpWatchPlugin
		\return a pointer to the new ExpWatchPlugin instance
	*/
	PluginFramework::IPlugin* ExpWatchPlugin::Create()
	{
		ExpWatchPlugin *pNewInst = new ExpWatchPlugin;
		ExpWatchPlugin::Query(pNewInst->m_BasePluginInfo);

		if (m_pPluginServices->SubscribeService(_T("GameChat"), _T("FormatChatMessage"), pNewInst) == false)
		{
			delete pNewInst;
			pNewInst = NULL;
		}

		CommandParameters Params;
		CallerParam Caller("ExpWatchPlugin", pNewInst);
		WindowerCommand StartCommand(PLUGIN_REGKEY, "expwatch::start", "starts gathering statistics on experience points gained", Caller, Start, Params);
		PluginFramework::ServiceParam InvokeArgStart(_T("WindowerCommand"), &StartCommand);
		WindowerCommand ResetCommand(PLUGIN_REGKEY, "expwatch::reset", "resets gathering statistics on experience points gained", Caller, Reset, Params);
		PluginFramework::ServiceParam InvokeArgReset(_T("WindowerCommand"), &ResetCommand);
		WindowerCommand StopCommand(PLUGIN_REGKEY, "expwatch::stop", "stops gathering statistics on experience points gained", Caller, Stop, Params);
		PluginFramework::ServiceParam InvokeArgStop(_T("WindowerCommand"), &StopCommand);

		const char *pParamName = "pointer";

		Params[pParamName].Name = pParamName;
		Params[pParamName].Type = COMMAND_PARAM_TYPE_POINTER;
		Params[pParamName].Value = "00000000";
		Params[pParamName].Description = "pointer receiving the result of the query";
		WindowerCommand QueryCommand(PLUGIN_REGKEY, "expwatch::query", "", Caller, Query, Params, 1U, 1U, false, true);
		PluginFramework::ServiceParam InvokeArgQuery(_T("WindowerCommand"), &QueryCommand);

		if (m_pPluginServices->InvokeService(_T("CommandDispatcher"), _T("RegisterCommand"), InvokeArgStart, PluginFramework::ServiceParam()) == false
		 || m_pPluginServices->InvokeService(_T("CommandDispatcher"), _T("RegisterCommand"), InvokeArgReset, PluginFramework::ServiceParam()) == false
		 || m_pPluginServices->InvokeService(_T("CommandDispatcher"), _T("RegisterCommand"), InvokeArgStop, PluginFramework::ServiceParam()) == false
		 || m_pPluginServices->InvokeService(_T("CommandDispatcher"), _T("RegisterCommand"), InvokeArgQuery, PluginFramework::ServiceParam()) == false)
		{
			delete pNewInst;
			pNewInst = NULL;
		}

		return pNewInst;
	}

	/*! \brief Destroys an instance of ExpWatchPlugin
		\param[in] pInstance_in : an instance of ExpWatchPlugin
	*/
	void ExpWatchPlugin::Destroy(PluginFramework::IPlugin *pInstance_in)
	{
		if (pInstance_in != NULL)
		{
			static_cast<ExpWatchPlugin*>(pInstance_in)->Stop();

			UnregisterParam UnregParamStart(PLUGIN_REGKEY, "timestamp::start");
			PluginFramework::ServiceParam InvokeArgStart(_T("UnregisterParam"), &UnregParamStart);
			UnregisterParam UnregParamReset(PLUGIN_REGKEY, "timestamp::reset");
			PluginFramework::ServiceParam InvokeArgReset(_T("UnregisterParam"), &UnregParamReset);
			UnregisterParam UnregParamStop(PLUGIN_REGKEY, "timestamp::stop");
			PluginFramework::ServiceParam InvokeArgStop(_T("UnregisterParam"), &UnregParamStop);
			UnregisterParam UnregParamQuery(PLUGIN_REGKEY, "timestamp::query");
			PluginFramework::ServiceParam InvokeArgQuery(_T("UnregisterParam"), &UnregParamQuery);

			m_pPluginServices->InvokeService(_T("CommandDispatcher"), _T("UnregisterCommand"), InvokeArgStart, PluginFramework::ServiceParam());
			m_pPluginServices->InvokeService(_T("CommandDispatcher"), _T("UnregisterCommand"), InvokeArgReset, PluginFramework::ServiceParam());
			m_pPluginServices->InvokeService(_T("CommandDispatcher"), _T("UnregisterCommand"), InvokeArgStop, PluginFramework::ServiceParam());
			m_pPluginServices->InvokeService(_T("CommandDispatcher"), _T("UnregisterCommand"), InvokeArgQuery, PluginFramework::ServiceParam());

			m_pPluginServices->UnsubscribeService(_T("GameChat"), _T("FormatChatMessage"), pInstance_in);

			delete pInstance_in;
			pInstance_in = NULL;
		}
	}

	/*! \brief Fills a PluginInfo structure with the plugin information
		\param[out] Info_out : a PluginInfo structure
	*/
	void ExpWatchPlugin::Query(PluginInfo& Info_out)
	{
		Info_out.Author = _T("Xebeth`");
		Info_out.Name = _T("ExpWatch");
		Info_out.PluginVersion.FromString(_T("1.0.0"));
		Info_out.FrameworkVersion.FromString(_T("1.0.0"));
		Info_out.Descritpion = _T("This plugin will gather statistics on experience points gained");
		Info_out.PluginIdentifier.FromString(_T("6FA271DC-DB0A-4B71-80D3-FE0B5DBF3BBF"));
	}

	bool ExpWatchPlugin::FormatChatMessage(USHORT MessageType, const StringObject* pSender_in_out,
										   StringObject* pMessage_in_out, const char *pOriginalMsg_in,
										   DWORD dwOriginalMsgSize, char **pBuffer_in_out)
	{
		if (m_bStarted)
		{
			// You earn 147 experience points.
			// You earn 332 (+20%) experience points.
			long ExpValue = 0, BonusValue = 0;

			if (sscanf_s(pOriginalMsg_in, "You earn %ld (+%ld%%) experience points.", &ExpValue, &BonusValue) >= 1)
			{
				m_TotalExp += ExpValue;
				++m_KillCounter;
			}
		}

		return true;
	}

	int ExpWatchPlugin::Start(const WindowerCommand *pCommand_in)
	{
		if (pCommand_in != NULL && pCommand_in->Caller.DataType.compare("ExpWatchPlugin") == 0)
		{
			ExpWatchPlugin *pExpWatch = reinterpret_cast<ExpWatchPlugin*>(pCommand_in->Caller.pData);

			if (pExpWatch != NULL && pExpWatch->Start())
				return DISPATCHER_RESULT_SUCCESS;
		}

		return DISPATCHER_RESULT_INVALID_CALL;
	}

	int ExpWatchPlugin::Stop(const WindowerCommand *pCommand_in)
	{
		if (pCommand_in != NULL && pCommand_in->Caller.DataType.compare("ExpWatchPlugin") == 0)
		{
			ExpWatchPlugin *pExpWatch = reinterpret_cast<ExpWatchPlugin*>(pCommand_in->Caller.pData);

			if (pExpWatch != NULL && pExpWatch->Stop())
				return DISPATCHER_RESULT_SUCCESS;
		}

		return DISPATCHER_RESULT_INVALID_CALL;
	}

	int ExpWatchPlugin::Reset(const WindowerCommand *pCommand_in)
	{
		if (pCommand_in != NULL && pCommand_in->Caller.DataType.compare("ExpWatchPlugin") == 0)
		{
			ExpWatchPlugin *pExpWatch = reinterpret_cast<ExpWatchPlugin*>(pCommand_in->Caller.pData);

			if (pExpWatch != NULL && pExpWatch->Reset())
				return DISPATCHER_RESULT_SUCCESS;
		}

		return DISPATCHER_RESULT_INVALID_CALL;
	}

	int ExpWatchPlugin::Query(const WindowerCommand *pCommand_in)
	{
		if (pCommand_in != NULL && pCommand_in->Caller.DataType.compare("ExpWatchPlugin") == 0)
		{
			ExpWatchPlugin *pExpWatch = reinterpret_cast<ExpWatchPlugin*>(pCommand_in->Caller.pData);
			const WindowerCommandParam *pParam = pCommand_in->GetParameter("pointer");

			if (pExpWatch != NULL && pParam != NULL && pExpWatch->Query(reinterpret_cast<std::string*>(pParam->GetPointerValue())))
				return DISPATCHER_RESULT_SUCCESS;
		}

		return DISPATCHER_RESULT_INVALID_CALL;
	}

	bool ExpWatchPlugin::Start()
	{
		if (m_bStarted == false)
		{
			m_bStarted = true;
			Reset();
		}

		return true;
	}

	bool ExpWatchPlugin::Stop()
	{
		if (m_bStarted)
			m_bStarted = false;

		return true;
	}

	bool ExpWatchPlugin::Reset()
	{
		if (m_bStarted)
		{
			m_TotalExp = m_AvgExpPerHour = m_AvgExpPerKill = 0.f;
			m_StartTime = GetTickCount();
			m_KillCounter = 0L;
		}

		return true;
	}

	bool ExpWatchPlugin::Query(std::string *pText_out)
	{
		if (m_bStarted)
		{
			float Hours = (GetTickCount() - m_StartTime) / 3600000.f;
			int iHours = (int)Hours;
			int iMinutes = (int)(60 * (Hours - iHours));

			if (m_KillCounter > 0)
			{
				m_AvgExpPerHour = m_TotalExp / Hours;
				m_AvgExpPerKill = m_TotalExp / m_KillCounter;
				format(*pText_out, "%.2f exp/hr %.2f exp/kill (%.0f / %02d:%02d)",
					   m_AvgExpPerHour, m_AvgExpPerKill, m_TotalExp, iHours, iMinutes);
			}
			else
				format(*pText_out, "0.00 exp/hr 0.00 exp/kill (0 / %02d:%02d)", iHours, iMinutes);
		}
		else
			pText_out->empty();

		return true;
	}

}

using Windower::ExpWatchPlugin;

extern "C" PLUGIN_API void QueryPlugin(PluginInfo &Info_out)
{
	ExpWatchPlugin::Query(Info_out);
}

extern "C" PLUGIN_API int TerminatePlugin()
{
	return 0;
}

extern "C" PLUGIN_API RegisterParams* InitPlugin(const PluginFramework::IPluginServices *pServicesParams_in)
{
	if (pServicesParams_in != NULL)
	{
		RegisterParams *pParams = new RegisterParams;

		ExpWatchPlugin::Query(pParams->Info);

		pParams->QueryFunc = ExpWatchPlugin::Query;
		pParams->CreateFunc = ExpWatchPlugin::Create;
		pParams->DestroyFunc = ExpWatchPlugin::Destroy;

		ExpWatchPlugin::SetPluginServices(pServicesParams_in);

		return pParams;
	}

	return NULL;
}