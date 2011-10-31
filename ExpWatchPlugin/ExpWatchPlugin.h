/**************************************************************************
	created		:	2011-06-04
	filename	: 	ExpWatchPlugin.h
	author		:	Xebeth`
	copyright	:	North Edge (2011)
	purpose		:	ExpWatch plugin
**************************************************************************/
#ifndef __EXP_WATCH_PLUGIN_H__
#define __EXP_WATCH_PLUGIN_H__

#define PLUGIN_REGKEY 0xEB71A021

namespace Windower
{
	class ExpWatchPlugin : public Windower::ICreateTextNodePlugin
	{
	public:
		ExpWatchPlugin();

		static PluginFramework::IPlugin* Create();
		static void Destroy(PluginFramework::IPlugin *pInstance_in);
		static void Query(PluginInfo& Info_out);

		static int Start(const WindowerCommand *pCommand_in);
		bool Start(std::string *pFeedback_in_out = NULL);

		static int Stop(const WindowerCommand *pCommand_in);
		bool Stop(std::string *pFeedback_in_out = NULL);

		static int Reset(const WindowerCommand *pCommand_in);
		bool Reset(std::string *pFeedback_in_out = NULL);

		bool OnChatMessage(USHORT MessageType_in, const StringNode* pSender_in_out,
						   StringNode* pMessage_in_out, const char *pOriginalMsg_in,
						   DWORD dwOriginalMsgSize_in, char **pBuffer_in_out,
						   bool &Unsubscribe_out);

		const char* OnCreateTextNode(const char *pText_in, bool &Unsubscribe_out);

	protected:
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