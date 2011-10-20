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
	class ExpWatchPlugin : public Windower::ICreateXmlNodePlugin
	{
	public:
		ExpWatchPlugin();
		virtual ~ExpWatchPlugin() {}

		static PluginFramework::IPlugin* Create();
		static void Destroy(PluginFramework::IPlugin *pInstance_in);
		static void Query(PluginInfo& Info_out);

		static int Start(const WindowerCommand *pCommand_in);
		bool Start();

		static int Stop(const WindowerCommand *pCommand_in);
		bool Stop();

		static int Reset(const WindowerCommand *pCommand_in);
		bool Reset();

		bool FormatChatMessage(USHORT MessageType, const StringObject* pSender_in_out,
							   StringObject* pMessage_in_out, const char *pOriginalMsg_in,
							   DWORD dwOriginalMsgSize, char **pBuffer_in_out);

		const char* OnCreateXmlNode(const char *pText_in, bool &Unsubscribe_out);

	protected:
		DWORD m_StartTime;
		bool m_bStarted;
		long m_KillCounter;
		float m_TotalExp;
		float m_AvgExpPerKill;
		float m_AvgExpPerHour;
	};
}

#endif//__EXP_WATCH_PLUGIN_H__