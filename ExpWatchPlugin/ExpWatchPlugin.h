/**************************************************************************
	created		:	2011-06-04
	filename	: 	ExpWatchPlugin.h
	author		:	Xebeth`
	copyright	:	North Edge (2011)
	purpose		:	ExpWatch plugin
**************************************************************************/
#ifndef __EXP_WATCH_PLUGIN_H__
#define __EXP_WATCH_PLUGIN_H__

namespace Windower
{
	class ExpWatchPlugin : public IGameChatPlugin, public CommandHandler
	{
		//! IDs of the commands registered with the plugin
		enum CommandMap
		{
			CMD_START = 0,	//!< start expwatch
			CMD_STOP,		//!< stop expwatch
			CMD_RESET,		//!< reset data
			CMD_REPORT,		//!< give stats report
			CMD_COUNT		//!< number of registered commands
		};
	public:
		explicit ExpWatchPlugin(PluginFramework::IPluginServices *pServices_in);
		~ExpWatchPlugin();

		static PluginFramework::IPlugin* Create(PluginFramework::IPluginServices *pServices_in);
		static void Destroy(PluginFramework::IPlugin *pInstance_in);
		static void Query(PluginFramework::PluginInfo& PluginInfo_out);
		
		bool Report(std::string *pFeedback_in_out = NULL);
		bool Reset(std::string *pFeedback_in_out = NULL);
		bool Start(std::string *pFeedback_in_out = NULL);		
		bool Stop(std::string *pFeedback_in_out = NULL);		

		DWORD OnChatMessage(USHORT MessageType_in, const char* pSender_in, DWORD MsgSize_in, const char *pOriginalMsg_in,
							char **pModifiedMsg_in_out, DWORD ModifiedSize_in, DWORD &MessageFlags_out);

		// const char* OnCreateTextNode(const char *pText_in, bool &Unsubscribe_out);

		bool ExecuteCommand(INT_PTR CmdID_in, const WindowerCommand &Command_in, std::string &Feedback_out);

	protected:
		bool UnregisterCommands();
		bool RegisterCommands();
		void UpdateStats();

		//! the time at which data collection started
		DWORD m_StartTime;
		//! flag specifying if the data collection started
		bool m_bStarted;
		//! the number of defeated enemies since the data collection started
		long m_KillCounter;
		//! the total amount of experience gained since the data collection started
		float m_TotalExp;
		//! the total amount of bonus experience from chains
		float m_BonusExp;
		//! the average experience rate per kill
		float m_AvgExpPerKill;
		//! the average experience rate per hour
		float m_AvgExpPerHour;
		//! duration of exp collection
		float m_ExpPeriod;
		//! hours for display
		unsigned short m_iHours;
		//! minutes for display
		unsigned short m_iMinutes;
		//! the scanning string to collect the experience data
		std::string m_ScanStr;
		//! the experience rate message added to the chat log
		std::string m_ExpRateMsg;
	};
}

#endif//__EXP_WATCH_PLUGIN_H__