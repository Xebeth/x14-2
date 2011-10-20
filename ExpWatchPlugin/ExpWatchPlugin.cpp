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
#include <ICreateXmlNodePlugin.h>
#include <IGameChatPlugin.h>
#include "WindowerCommand.h"
#include "ExpWatchPlugin.h"

const PluginFramework::IPluginServices* PluginFramework::IPlugin::m_pPluginServices = NULL;

namespace Windower
{
	ExpWatchPlugin::ExpWatchPlugin() : ICreateXmlNodePlugin("Next"),
		m_TotalExp(0.f), m_bStarted(false), m_KillCounter(0L),
		m_AvgExpPerKill(0.f), m_AvgExpPerHour(0.f), m_StartTime(0UL) {}

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

		if (m_pPluginServices->InvokeService(_T("CommandDispatcher"), _T("RegisterCommand"), InvokeArgStart, PluginFramework::ServiceParam()) == false
		 || m_pPluginServices->InvokeService(_T("CommandDispatcher"), _T("RegisterCommand"), InvokeArgReset, PluginFramework::ServiceParam()) == false
		 || m_pPluginServices->InvokeService(_T("CommandDispatcher"), _T("RegisterCommand"), InvokeArgStop, PluginFramework::ServiceParam()) == false)
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

	bool ExpWatchPlugin::Start()
	{
		m_pTargetPtr = NULL;

		if (m_bStarted == false)
		{
			m_pPluginServices->SubscribeService(_T("GameChat"), _T("CreateXmlNode"), this);
			m_bStarted = true;
			Reset();
		}

		return true;
	}

	bool ExpWatchPlugin::Stop()
	{
		m_pTargetPtr = NULL;

		if (m_bStarted)
		{
			m_pPluginServices->UnsubscribeService(_T("GameChat"), _T("CreateXmlNode"), this);
			m_bStarted = false;
		}

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

	const char* ExpWatchPlugin::OnCreateXmlNode(const char *pText_in, bool &Unsubscribe_out)
	{
		Unsubscribe_out = false;

		if (m_bStarted)
		{
			if (m_pTargetPtr == NULL && strcmp(pText_in, m_TargetText.c_str()) == 0)
				m_pTargetPtr = pText_in;

			if (pText_in == m_pTargetPtr)
			{
				float Hours = (GetTickCount() - m_StartTime) / 3600000.f;
				int iHours = (int)Hours;
				int iMinutes = (int)(60 * (Hours - iHours));

				if (m_KillCounter > 0)
				{
					m_AvgExpPerHour = m_TotalExp / Hours;
					m_AvgExpPerKill = m_TotalExp / m_KillCounter;
					format(m_InjectedText, "%s\t\t  %.2f exp/hr %.2f exp/kill (%.0f / %02d:%02d)",
						   pText_in, m_AvgExpPerHour, m_AvgExpPerKill, m_TotalExp, iHours, iMinutes);
				}
				else
					format(m_InjectedText, "%s\t\t  0.00 exp/hr 0.00 exp/kill (0 / %02d:%02d)", pText_in, iHours, iMinutes);

				return m_InjectedText.c_str();
			}
		}

		return pText_in;
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