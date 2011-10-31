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
#include <ICreateTextNodePlugin.h>
#include <IGameChatPlugin.h>
#include "WindowerCommand.h"
#include "ExpWatchPlugin.h"

const PluginFramework::IPluginServices* PluginFramework::IPlugin::m_pPluginServices = NULL;

namespace Windower
{
	//! \brief ExpWatchPlugin constructor
	ExpWatchPlugin::ExpWatchPlugin() : ICreateTextNodePlugin("Next"),
		m_TotalExp(0.f), m_bStarted(false), m_KillCounter(0L),
		m_AvgExpPerKill(0.f), m_AvgExpPerHour(0.f), m_StartTime(0UL) {}

	/*! \brief Creates an instance of ExpWatchPlugin
		\return a pointer to the new ExpWatchPlugin instance
	*/
	PluginFramework::IPlugin* ExpWatchPlugin::Create()
	{
		ExpWatchPlugin *pNewInst = new ExpWatchPlugin;
		ExpWatchPlugin::Query(pNewInst->m_PluginInfo);

		if (m_pPluginServices->SubscribeService(_T("GameChat"), _T("OnChatMessage"), pNewInst) == false)
		{
			delete pNewInst;
			pNewInst = NULL;
		}

		CommandParameters Params;
		PluginFramework::ServiceParam DummyResults;
		CallerParam Caller("ExpWatchPlugin", pNewInst);
		WindowerCommand StartCommand(PLUGIN_REGKEY, "expwatch::start", "starts gathering statistics on experience points gained", Caller, Start, Params);
		PluginFramework::ServiceParam InvokeArgStart(_T("WindowerCommand"), &StartCommand);
		WindowerCommand ResetCommand(PLUGIN_REGKEY, "expwatch::reset", "resets gathering statistics on experience points gained", Caller, Reset, Params);
		PluginFramework::ServiceParam InvokeArgReset(_T("WindowerCommand"), &ResetCommand);
		WindowerCommand StopCommand(PLUGIN_REGKEY, "expwatch::stop", "stops gathering statistics on experience points gained", Caller, Stop, Params);
		PluginFramework::ServiceParam InvokeArgStop(_T("WindowerCommand"), &StopCommand);

		if (m_pPluginServices->InvokeService(_T("CommandDispatcher"), _T("RegisterCommand"), InvokeArgStart, DummyResults) == false
		 || m_pPluginServices->InvokeService(_T("CommandDispatcher"), _T("RegisterCommand"), InvokeArgReset, DummyResults) == false
		 || m_pPluginServices->InvokeService(_T("CommandDispatcher"), _T("RegisterCommand"), InvokeArgStop, DummyResults) == false)
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

			PluginFramework::ServiceParam DummyResults;
			UnregisterParam UnregParamStart(PLUGIN_REGKEY, "expwatch::start");
			PluginFramework::ServiceParam InvokeArgStart(_T("UnregisterParam"), &UnregParamStart);
			UnregisterParam UnregParamReset(PLUGIN_REGKEY, "expwatch::reset");
			PluginFramework::ServiceParam InvokeArgReset(_T("UnregisterParam"), &UnregParamReset);
			UnregisterParam UnregParamStop(PLUGIN_REGKEY, "expwatch::stop");
			PluginFramework::ServiceParam InvokeArgStop(_T("UnregisterParam"), &UnregParamStop);

			m_pPluginServices->InvokeService(_T("CommandDispatcher"), _T("UnregisterCommand"), InvokeArgStart, DummyResults);
			m_pPluginServices->InvokeService(_T("CommandDispatcher"), _T("UnregisterCommand"), InvokeArgReset, DummyResults);
			m_pPluginServices->InvokeService(_T("CommandDispatcher"), _T("UnregisterCommand"), InvokeArgStop, DummyResults);

			m_pPluginServices->UnsubscribeService(_T("GameChat"), _T("OnChatMessage"), pInstance_in);

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

	/*! \brief Callback invoked when the game chat receives a new line
		\param[in] MessageType_in : the type of the message
		\param[in] pSender_in : the sender of the message
		\param[in,out] pMessage_in_out : the message (might have been modified by other plugins)
		\param[in] pOriginalMsg_in : a pointer to the unmodified message
		\param[in] dwOriginalMsgSize_in : the size of the original message
		\param[in] pBuffer_in_out : the resulting text modified by the plugin
		\param[in] Unsubscribe_out : flag specifying if the plugin wants to revoke its subscription to the hook
		\return true if the message was logged; false otherwise
	*/
	bool ExpWatchPlugin::OnChatMessage(USHORT MessageType_in, const StringNode* pSender_in_out,
									   StringNode* pMessage_in_out, const char *pOriginalMsg_in,
									   DWORD dwOriginalMsgSize_in, char **pBuffer_in_out,
									   bool &Unsubscribe_out)
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

	/*! \brief Start command invocation
		\param[in] pCommand_in : the command received from the command dispatcher
		\return DISPATCHER_RESULT_SUCCESS if successful; DISPATCHER_RESULT_INVALID_CALL otherwise
	*/
	int ExpWatchPlugin::Start(const WindowerCommand *pCommand_in)
	{
		if (pCommand_in != NULL && pCommand_in->Caller.DataType.compare("ExpWatchPlugin") == 0)
		{
			ExpWatchPlugin *pExpWatch = reinterpret_cast<ExpWatchPlugin*>(pCommand_in->Caller.pData);

			if (pExpWatch != NULL && pExpWatch->Start(&pCommand_in->ResultMsg))
				return DISPATCHER_RESULT_SUCCESS;
		}

		return DISPATCHER_RESULT_INVALID_CALL;
	}

	/*! \brief Stop command invocation
		\param[in] pCommand_in : the command received from the command dispatcher
		\return DISPATCHER_RESULT_SUCCESS if successful; DISPATCHER_RESULT_INVALID_CALL otherwise
	*/
	int ExpWatchPlugin::Stop(const WindowerCommand *pCommand_in)
	{
		if (pCommand_in != NULL && pCommand_in->Caller.DataType.compare("ExpWatchPlugin") == 0)
		{
			ExpWatchPlugin *pExpWatch = reinterpret_cast<ExpWatchPlugin*>(pCommand_in->Caller.pData);

			if (pExpWatch != NULL && pExpWatch->Stop(&pCommand_in->ResultMsg))
				return DISPATCHER_RESULT_SUCCESS;
		}

		return DISPATCHER_RESULT_INVALID_CALL;
	}

	/*! \brief Reset command invocation
		\param[in] pCommand_in : the command received from the command dispatcher
		\return DISPATCHER_RESULT_SUCCESS if successful; DISPATCHER_RESULT_INVALID_CALL otherwise
	*/
	int ExpWatchPlugin::Reset(const WindowerCommand *pCommand_in)
	{
		if (pCommand_in != NULL && pCommand_in->Caller.DataType.compare("ExpWatchPlugin") == 0)
		{
			ExpWatchPlugin *pExpWatch = reinterpret_cast<ExpWatchPlugin*>(pCommand_in->Caller.pData);

			if (pExpWatch != NULL && pExpWatch->Reset(&pCommand_in->ResultMsg))
				return DISPATCHER_RESULT_SUCCESS;
		}

		return DISPATCHER_RESULT_INVALID_CALL;
	}

	/*! \brief Starts the experience counter
		\param[in,out] pFeedback_in_out : pointer to a string receiving feedback
		\return true
	*/
	bool ExpWatchPlugin::Start(std::string *pFeedback_in_out)
	{
		m_pTargetPtr = NULL;

		if (m_bStarted == false)
		{
			if (pFeedback_in_out != NULL)
				*pFeedback_in_out = "ExpWatch started gathering statistics on experience points gained.";

			m_pPluginServices->SubscribeService(_T("GameChat"), _T("CreateTextNode"), this);
			m_bStarted = true;
			Reset();			
		}

		return true;
	}

	/*! \brief Stops the experience counter
		\param[in,out] pFeedback_in_out : pointer to a string receiving feedback
		\return true
	*/
	bool ExpWatchPlugin::Stop(std::string *pFeedback_in_out)
	{
		m_pTargetPtr = NULL;

		if (m_bStarted)
		{
			if (pFeedback_in_out != NULL)
			{
				float Hours = (GetTickCount() - m_StartTime) / 3600000.f;
				int iHours = (int)Hours;
				int iMinutes = (int)(60 * (Hours - iHours));

				format(*pFeedback_in_out, "ExpWatch stopped gathering statistics on experience points gained.\n"
					   "\tTotal experience points: %ld for %ld kills in %02d:%02d\n"
					   "\tAverage experience per hour: %.2f\n\tAverage experience per kill: %.2f",
					   m_TotalExp, m_KillCounter, iHours, iMinutes, m_AvgExpPerHour, m_AvgExpPerKill);
			}

			m_pPluginServices->UnsubscribeService(_T("GameChat"), _T("CreateTextNode"), this);
			m_bStarted = false;
		}

		return true;
	}

	/*! \brief Resets the experience counter
		\param[in,out] pFeedback_in_out : pointer to a string receiving feedback
		\return true
	*/
	bool ExpWatchPlugin::Reset(std::string *pFeedback_in_out)
	{
		if (m_bStarted)
		{
			if (pFeedback_in_out != NULL)
				*pFeedback_in_out = "ExpWatch statistics were reset successfully.";

			m_TotalExp = m_AvgExpPerHour = m_AvgExpPerKill = 0.f;
			m_StartTime = GetTickCount();
			m_KillCounter = 0L;
		}

		return true;
	}

	/*! \brief Callback function invoked when the game creates a string object
		\param[in] pText_in : the text used to create the string node
		\param[in] Unsubscribe_out : flag specifying if the plugin wants to revoke its subscription to the hook
		\return the modified version of the text
	*/
	const char* ExpWatchPlugin::OnCreateTextNode(const char *pText_in, bool &Unsubscribe_out)
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

/*! \brief Function exposed by the plugin DLL to retrieve the plugin information
	\param[in] PluginInfo_out : the plugin information
*/
extern "C" PLUGIN_API void QueryPlugin(PluginInfo &Info_out)
{
	ExpWatchPlugin::Query(Info_out);
}

/*! \brief Function exposed by the plugin DLL to initialize the plugin object
	\param[in] PluginServices_in : the plugin services
	\return a pointer to the plugin registration parameters if successful; NULL otherwise
*/
extern "C" PLUGIN_API RegisterParams* InitPlugin(const PluginFramework::IPluginServices &PluginServices_in)
{
	RegisterParams *pParams = new RegisterParams;

	ExpWatchPlugin::Query(pParams->Info);

	pParams->QueryFunc = ExpWatchPlugin::Query;
	pParams->CreateFunc = ExpWatchPlugin::Create;
	pParams->DestroyFunc = ExpWatchPlugin::Destroy;

	ExpWatchPlugin::SetPluginServices(PluginServices_in);

	return pParams;
}