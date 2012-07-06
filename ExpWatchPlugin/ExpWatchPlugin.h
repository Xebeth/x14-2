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
	class ExpWatchPlugin : public ICreateTextNodePlugin, public CommandHandler
	{
		//! IDs of the commands registered with the plugin
		enum CommandMap
		{
			CMD_START = 0,	//!< start expwatch
			CMD_STOP,		//!< stop expwatch
			CMD_RESET,		//!< reset data
			CMD_COUNT		//!< number of registered commands
		};
	public:
		explicit ExpWatchPlugin(PluginFramework::IPluginServices *pServices_in);
		~ExpWatchPlugin();

		static PluginFramework::IPlugin* Create(PluginFramework::IPluginServices *pServices_in);
		static void Destroy(PluginFramework::IPlugin *pInstance_in);
		static void Query(PluginFramework::PluginInfo& PluginInfo_out);
		
		bool Reset(std::string *pFeedback_in_out = NULL);
		bool Start(std::string *pFeedback_in_out = NULL);		
		bool Stop(std::string *pFeedback_in_out = NULL);		

		bool OnChatMessage(USHORT MessageType_in, const StringNode* pSender_in_out,
						   StringNode* pMessage_in_out, const char *pOriginalMsg_in,
						   DWORD dwOriginalMsgSize_in, char **pBuffer_in_out,
						   bool &Unsubscribe_out);

		const char* OnCreateTextNode(const char *pText_in, bool &Unsubscribe_out);

		virtual bool ExecuteCommand(INT_PTR CmdID_in, const WindowerCommand &Command_in, std::string &Feedback_out);

	protected:
		bool UnregisterCommands();
		bool RegisterCommands();

		//! the time at which data collection started
		DWORD m_StartTime;
		//! flag specifying if the data collection started
		bool m_bStarted;
		//! the number of defeated enemies since the data collection started
		long m_KillCounter;
		//! the total amount of experience gained since the data collection started
		float m_TotalExp;
		//! the average experience rate per kill
		float m_AvgExpPerKill;
		//! the average experience rate per hour
		float m_AvgExpPerHour;
	};
}

#endif//__EXP_WATCH_PLUGIN_H__