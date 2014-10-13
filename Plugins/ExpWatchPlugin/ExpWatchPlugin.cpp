/**************************************************************************
	created		:	2011-06-04
	filename	: 	ExpWatchPlugin.cpp
	author		:	Xebeth`
	copyright	:	North Edge (2011)
	purpose		:	ExpWatch plugin
**************************************************************************/
#include "stdafx.h"
#include "version.h"

#include "ExpWatchPlugin.h"

namespace Windower
{
	/*! \brief ExpWatchPlugin constructor
		\param[in] pServices_in : a pointer to the plugin services
	*/
	ExpWatchPlugin::ExpWatchPlugin(PluginFramework::IPluginServices *pServices_in)
		: IGameChatPlugin(pServices_in), CommandHandler(0xEB71A021, "ExpWatchPlugin"),
		  m_bStarted(false), m_AvgExpPerKill(0.f), m_AvgExpPerHour(0.f), m_TotalExp(0.f),
		  m_BonusExp(0.f), m_ExpPeriod(0.f), m_StartTime(0UL),  m_KillCounter(0L),
		  m_iHours(0U), m_iMinutes(0U)  {}

	//! \brief ExpWatchPlugin destructor
	ExpWatchPlugin::~ExpWatchPlugin()
	{ Stop(); }

	/*! \brief Creates an instance of ExpWatchPlugin
		\return a pointer to the new ExpWatchPlugin instance
	*/
	PluginFramework::IPlugin* ExpWatchPlugin::Create(PluginFramework::IPluginServices *pServices_in)
	{
		return new ExpWatchPlugin(pServices_in);
	}

	/*! \brief Destroys an instance of ExpWatchPlugin
		\param[in] pInstance_in : an instance of ExpWatchPlugin
	*/
	void ExpWatchPlugin::Destroy(IPlugin *pInstance_in)
	{
		if (pInstance_in != NULL)
		{
			delete pInstance_in;
			pInstance_in = NULL;
		}
	}

	/*! \brief Fills a VersionInfo structure with the plugin information
		\param[out] Info_out : a VersionInfo structure
	*/
	void ExpWatchPlugin::Query(PluginFramework::PluginInfo& PluginInfo_out)
	{
		PluginInfo_out.SetDesc(_T("This plugin will gather statistics on experience points earned"));
		PluginInfo_out.SetIdentifier(_T("6FA271DC-DB0A-4B71-80D3-FE0B5DBF3BBF"));
		PluginInfo_out.SetCompatibilityFlags(PLUGIN_COMPATIBILITY_WINDOWER);
		PluginInfo_out.SetVersion(PLUGIN_VERSION);
		PluginInfo_out.SetAuthor(_T("Xebeth`"));
		PluginInfo_out.SetName(_T("ExpWatch"));
	}
	
	/*! \brief Registers the commands of the plugin with the command dispatcher
		\return true if all the commands were registered successfully; false otherwise
	*/
	bool ExpWatchPlugin::RegisterCommands()
	{
		bool Result = true;

		// register the commands
		Result &= (RegisterCommand(CMD_START, "expwatch::start", "starts gathering statistics on experience points earned") != NULL);
		Result &= (RegisterCommand(CMD_RESET, "expwatch::reset", "resets gathering statistics on experience points earned") != NULL);
		Result &= (RegisterCommand(CMD_STOP, "expwatch::stop", "stops gathering statistics on experience points earned") != NULL);
		Result &= (RegisterCommand(CMD_REPORT, "expwatch::report", "gives a report on the statistics collected") != NULL);

		return Result;
	}

	/*! \brief Unregisters the commands of the plugin with the command dispatcher
		\return true if all the commands were unregistered successfully; false otherwise
	*/
	bool ExpWatchPlugin::UnregisterCommands()
	{
		bool Result = true;

		// revoke the commands
		Result &= RevokeCommand(CMD_REPORT);
		Result &= RevokeCommand(CMD_START);
		Result &= RevokeCommand(CMD_RESET);
		Result &= RevokeCommand(CMD_STOP);

		return Result;
	}

	/*! \brief Callback invoked when the game chat receives a new line
		\param[in] MessageType_in : the type of the message
		\param[in] pSender_in : the sender of the message
		\param[in] MsgSize_in : the size of the unmodified message
		\param[in] pOriginalMsg_in : a pointer to the unmodified message
		\param[in] pModifiedMsg_in_out : the resulting text modified by the plugin
		\param[in] dwNewSize_out : the new size of the message
		\param[in] DWORD ModifiedSize_in : the modified message size
		\return the new size of the message
	*/
	DWORD ExpWatchPlugin::OnChatMessage(USHORT MessageType_in, const char* pSender_in, DWORD MsgSize_in, const char *pOriginalMsg_in,
										char **pModifiedMsg_in_out, DWORD ModifiedSize_in, DWORD &MessageFlags_out)
	{
		if (MessageType_in == BATTLE_MESSAGE_TYPE_EXPERIENCE_GAIN)
		{
			if (m_bStarted)
			{
				// You earn 147 experience points.
				// You earn 332 (+20%) experience points.
				long ExpValue = 0, BonusValue = 0;

				m_ScanStr = pOriginalMsg_in;
				// purge everything but numbers and +
				purge<char>(m_ScanStr, _DIGIT, "+", true);

				if (sscanf_s(m_ScanStr.c_str(), "%ld+%ld", &ExpValue, &BonusValue) >= 1)
				{
					++m_KillCounter;
					m_TotalExp += ExpValue;

					if (BonusValue > 0.f)
						m_BonusExp += ExpValue - ExpValue / (1L + BonusValue * 0.01f);
					// update the stats
					UpdateStats();

#ifndef _DEBUG
					if (m_KillCounter >= 10 && m_iMinutes > 0)
#endif // _DEBUG
					{
						DWORD dwNewSize, MsgSize;

						// add the exp rate to the text
						format(m_ExpRateMsg, " \xee\x81\xaf exp. rate : %.2f/h", m_AvgExpPerHour);
						MsgSize = m_ExpRateMsg.length();
						// add 11 characters for the timestamp
						dwNewSize = ModifiedSize_in + MsgSize;
						// allocate a new buffer
						if (ResizeBuffer(pOriginalMsg_in, MsgSize_in, dwNewSize, pModifiedMsg_in_out, ModifiedSize_in))
						{
							// copy the exp rate message
							memcpy_s(*pModifiedMsg_in_out + ModifiedSize_in - 1, MsgSize * sizeof(char), m_ExpRateMsg.c_str(), MsgSize);					

							return dwNewSize;
						}
					}
				}
			}
		}

		return MsgSize_in;
	}

	//! \brief Updates the statistics
	void ExpWatchPlugin::UpdateStats()
	{
		m_ExpPeriod = (GetTickCount() - m_StartTime) / 3600000.f;
		m_iHours = (int)m_ExpPeriod;
		m_iMinutes = (int)(60 * (m_ExpPeriod - m_iHours));

		m_AvgExpPerHour = m_TotalExp / m_ExpPeriod;

		if (m_KillCounter > 0)
			m_AvgExpPerKill = m_TotalExp / m_KillCounter;
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
			case CMD_REPORT:
				return Report(&Feedback_out);
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
				*pFeedback_in_out = "ExpWatch started gathering statistics on experience points earned.";

			m_bStarted = true;
			Subscribe();
			Reset();

			return true;
		}

		return Report(pFeedback_in_out);
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
				*pFeedback_in_out = "ExpWatch stopped gathering statistics on experience points earned.\n";
			// report on stats
			Report(pFeedback_in_out);
			// change the flag after the report
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

			m_BonusExp = m_TotalExp = m_AvgExpPerHour = m_AvgExpPerKill = 0.f;
			m_StartTime = GetTickCount();
			m_KillCounter = 0L;
		}

		return true;
	}

	/*! \brief Gvies a report on the experience statistics
		\param[in,out] pFeedback_in_out : pointer to a string receiving feedback
		\return true
	*/
	bool ExpWatchPlugin::Report(std::string *pFeedback_in_out)
	{
		if (m_bStarted && pFeedback_in_out != NULL)
		{
			UpdateStats();

			append_format(*pFeedback_in_out, "ExpWatch statistics on experience points earned:\n"
											 "   Total experience points: %.0f for %ld kills in %02d:%02d\n"
											 "   Experience bonus from chains: %.0f\n"
											 "   Average experience per hour: %.2f\n"
											 "   Average experience per kill: %.2f",
											 m_TotalExp, m_KillCounter, m_iHours, m_iMinutes,
											 m_BonusExp, m_AvgExpPerHour, m_AvgExpPerKill);
		}

		return true;
	}
}

using Windower::ExpWatchPlugin;

/*! \brief Function exposed by the plugin DLL to initialize the plugin object
	\param[out] RegisterParams_out : Registration structure to be able to use the plugin
	\return true if the initialization succeeded; false otherwise
*/
extern "C" PLUGIN_API bool InitPlugin(PluginFramework::RegisterParams &RegisterParams_out)
{
	return PluginFramework::IPlugin::Initialize(RegisterParams_out, ExpWatchPlugin::Create, 
												ExpWatchPlugin::Destroy, ExpWatchPlugin::Query, NULL);
}