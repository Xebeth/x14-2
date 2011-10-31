/**************************************************************************
	created		:	2011-06-04
	filename	: 	ChatLogPlugin.h
	author		:	Xebeth`
	copyright	:	North Edge (2011)
	purpose		:	Chat log plugin
**************************************************************************/
#ifndef __CHAT_LOG_PLUGIN_H__
#define __CHAT_LOG_PLUGIN_H__

namespace Windower
{
	class ChatLogPlugin : public Windower::IGameChatPlugin
	{
	public:
		ChatLogPlugin();
		virtual ~ChatLogPlugin();

		static PluginFramework::IPlugin* Create();
		static void Destroy(PluginFramework::IPlugin *pInstance_in);
		static void Query(PluginInfo& Info_out);

		bool StartLog();
		void StopLog();

		bool OnChatMessage(USHORT MessageType_in, const StringNode* pSender_in_out,
						   StringNode* pMessage_in_out, const char *pOriginalMsg_in,
						   DWORD dwOriginalMsgSize_in, char **pBuffer_in_out,
						   bool &Unsubscribe_out);
	protected:
		bool WriteLine(const string_t &Line_in);

		//! string buffer used to write in the log file
		string_t	 m_Buffer;
		//! flag specifying if the log file is open
		bool		 m_bOpened;
		//! handle of the log file
		FILE		*m_pFile;
	};
}

#endif//__CHAT_LOG_PLUGIN_H__