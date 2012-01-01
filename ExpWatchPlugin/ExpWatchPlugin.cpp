/**************************************************************************
	created		:	2011-06-04
	filename	: 	ExpWatchPlugin.cpp
	author		:	Xebeth`
	copyright	:	North Edge (2011)
	purpose		:	ExpWatch plugin
**************************************************************************/
#include "stdafx.h"
#include <PluginFramework.h>

#include <ICreateTextNodePlugin.h>
#include <IGameChatPlugin.h>
#include <WindowerCommand.h>
#include <CommandHandler.h>

#include "ExpWatchPlugin.h"

using namespace PluginFramework;

namespace Windower
{
	//! \brief ExpWatchPlugin constructor
	ExpWatchPlugin::ExpWatchPlugin() : ICreateTextNodePlugin("Next"),
		CommandHandler(0xEB71A021, "ExpWatchPlugin"),
		m_TotalExp(0.f), m_bStarted(false), m_KillCounter(0L),
		m_AvgExpPerKill(0.f), m_AvgExpPerHour(0.f), m_StartTime(0UL) {}

	/*! \brief Creates an instance of ExpWatchPlugin
		\return a pointer to the new ExpWatchPlugin instance
	*/
	IPlugin* ExpWatchPlugin::Create()
	{
		ExpWatchPlugin *pNewInst = new ExpWatchPlugin;

		// initialize the plugin
		ExpWatchPlugin::Query(pNewInst->m_PluginInfo);
		// subscribe to the chat service
		pNewInst->Subscribe();
		// register the commands		
		pNewInst->RegisterCommand(CMD_START, "expwatch::start", "starts gathering statistics on experience points gained");
		pNewInst->RegisterCommand(CMD_RESET, "expwatch::reset", "resets gathering statistics on experience points gained");
		pNewInst->RegisterCommand(CMD_STOP, "expwatch::stop", "stops gathering statistics on experience points gained");

		return pNewInst;
	}

	/*! \brief Destroys an instance of ExpWatchPlugin
		\param[in] pInstance_in : an instance of ExpWatchPlugin
	*/
	void ExpWatchPlugin::Destroy(IPlugin *pInstance_in)
	{
		if (pInstance_in != NULL)
		{
			ExpWatchPlugin *pExpWatch = static_cast<ExpWatchPlugin*>(pInstance_in);

			pExpWatch->Stop();
			// revoke the commands
			pExpWatch->RevokeCommand(CMD_START);
			pExpWatch->RevokeCommand(CMD_RESET);
			pExpWatch->RevokeCommand(CMD_STOP);
			// unsubscribe from the chat service
			pExpWatch->Unsubscribe();

			delete pInstance_in;
			pInstance_in = NULL;
		}
	}

	/*! \brief Fills a VersionInfo structure with the plugin information
		\param[out] Info_out : a VersionInfo structure
	*/
	void ExpWatchPlugin::Query(PluginInfo& PluginInfo_out)
	{
		PluginInfo_out.SetDesc(_T("This plugin will gather statistics on experience points gained"));
		PluginInfo_out.SetIdentifier(_T("6FA271DC-DB0A-4B71-80D3-FE0B5DBF3BBF"));
		PluginInfo_out.SetAuthor(_T("Xebeth`"));
		PluginInfo_out.SetName(_T("ExpWatch"));
		PluginInfo_out.SetVersion(_T("1.0.0"));

		IPlugin::Query(PluginInfo_out);
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

	/*! \brief Executes the command specified by its ID
		\param[in] CmdID_in : the ID of the command to execute
		\param[in] Command_in : the command to execute
		\param[out] Feedback_out : the result of the execution
		\return true if the command was executed successfully; false otherwise
	*/
	bool ExpWatchPlugin::ExecuteCommand(INT_PTR CmdID_in, const WindowerCommand &Command_in, std::string &Feedback_out)
	{
		switch(CmdID_in)
		{
			case CMD_START:
				return Start(&Feedback_out);
			case CMD_STOP:
				return Stop(&Feedback_out);
			case CMD_RESET:
				return Reset(&Feedback_out);
		}

		return false;
	}

	/*! \brief Starts the experience counter
		\param[in,out] pFeedback_in_out : pointer to a string receiving feedback
		\return true
	*/
	bool ExpWatchPlugin::Start(std::string *pFeedback_in_out)
	{
		if (m_bStarted == false)
		{
			if (pFeedback_in_out != NULL)
				*pFeedback_in_out = "ExpWatch started gathering statistics on experience points gained.";

			m_pTargetPtr = NULL;
			m_bStarted = true;
			Subscribe();
			Reset();			
		}
		else if (pFeedback_in_out != NULL)
		{
			float Hours = (GetTickCount() - m_StartTime) / 3600000.f;
			int iHours = (int)Hours;
			int iMinutes = (int)(60 * (Hours - iHours));

			m_AvgExpPerHour = m_TotalExp / Hours;
			m_AvgExpPerKill = m_TotalExp / m_KillCounter;

			format(*pFeedback_in_out, "ExpWatch statistics on experience points gained:\n"
				   "\tTotal experience points: %.0f for %ld kills in %02d:%02d\n"
				   "\tAverage experience per hour: %.2f\n\tAverage experience per kill: %.2f",
				   m_TotalExp, m_KillCounter, iHours, iMinutes, m_AvgExpPerHour, m_AvgExpPerKill);
		}

		return true;
	}

	/*! \brief Stops the experience counter
		\param[in,out] pFeedback_in_out : pointer to a string receiving feedback
		\return true
	*/
	bool ExpWatchPlugin::Stop(std::string *pFeedback_in_out)
	{
		if (m_bStarted)
		{
			if (pFeedback_in_out != NULL)
			{
				float Hours = (GetTickCount() - m_StartTime) / 3600000.f;
				int iHours = (int)Hours;
				int iMinutes = (int)(60 * (Hours - iHours));

				format(*pFeedback_in_out, "ExpWatch stopped gathering statistics on experience points gained.\n"
					   "\tTotal experience points: %.0f for %ld kills in %02d:%02d\n"
					   "\tAverage experience per hour: %.2f\n\tAverage experience per kill: %.2f",
					   m_TotalExp, m_KillCounter, iHours, iMinutes, m_AvgExpPerHour, m_AvgExpPerKill);
			}

			m_pTargetPtr = NULL;
			m_bStarted = false;
			Unsubscribe();			
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

/*! \brief Function exposed by the plugin DLL to initialize the plugin object
	\param[in] pServices_in : services used to (un)subscribe to services and invoke them
	\return a pointer to the plugin registration parameters if successful; NULL otherwise
*/
extern "C" PLUGIN_API RegisterParams* InitPlugin(IPluginServices *pServices_in)
{
	RegisterParams *pParams = new RegisterParams;

	ExpWatchPlugin::Query(pParams->Info);
	IPlugin::SetServices(pServices_in);

	pParams->QueryFunc = ExpWatchPlugin::Query;
	pParams->CreateFunc = ExpWatchPlugin::Create;
	pParams->DestroyFunc = ExpWatchPlugin::Destroy;

	return pParams;
}