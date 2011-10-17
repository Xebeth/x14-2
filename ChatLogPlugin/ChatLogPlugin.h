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

		bool FormatChatMessage(USHORT MessageType, const StringObject* pSender_in_out,
							   StringObject* pMessage_in_out, const char *pOriginalMsg_in,
							   DWORD dwOriginalMsgSize, char **pBuffer_in_out);
	protected:
		bool WriteLine(string_t& Line_in);

		string_t	 m_Buffer;
		bool		 m_bOpened;
		FILE		*m_pFile;
	};
}

#endif//__CHAT_LOG_PLUGIN_H__