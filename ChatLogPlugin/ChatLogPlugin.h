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
	class ChatLogPlugin : public TimestampPlugin
	{
	public:
		explicit ChatLogPlugin(PluginFramework::IPluginServices *pServices_in);
		virtual ~ChatLogPlugin();

		static PluginFramework::IPlugin* Create(PluginFramework::IPluginServices *pServices_in);
		static void Destroy(PluginFramework::IPlugin *pInstance_in);
		static void Query(PluginFramework::PluginInfo& PluginInfo_out);

		bool StartLog();
		void StopLog();

		bool OnChatMessage(USHORT MessageType_in, const StringNode* pSender_in,
						   const StringNode* pMessage_in, const char *pOriginalMsg_in,
						   DWORD dwOriginalMsgSize_in, char **pBuffer_in_out,
						   DWORD *pNewSize_out);
	protected:
		bool WriteLine(const string_t &Line_in);

		void UpdateTimestamp();

		//! path of the logs
		string_t	 m_LogPath;
		//! string buffer used to write in the log file
		string_t	 m_Buffer;
		//! timestamp format in unicode
		string_t	 m_TimestampFormatW;
		//! flag specifying if the log file is open
		bool		 m_bOpened;
		//! handle of the log file
		FILE		*m_pFile;
		//! buffer to hold the timestamp
		TCHAR		*m_pTimestamp;
	};
}

#endif//__CHAT_LOG_PLUGIN_H__