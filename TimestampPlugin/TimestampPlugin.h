/**************************************************************************
	created		:	2011-06-04
	filename	: 	TimestampPlugin.h
	author		:	Xebeth`
	copyright	:	North Edge (2011)
	purpose		:	Timestamp plugin
**************************************************************************/
#ifndef __TIMESTAMP_PLUGIN_H__
#define __TIMESTAMP_PLUGIN_H__

#define PLUGIN_REGKEY	0xAF8B3EE1
#define TIMESTAMP_DEFAULT_FORMAT	"[HH:mm:ss] "
#define TIMESTAMP_DEFAULT_LENGTH	11

namespace Windower
{
	class TimestampPlugin : public Windower::IGameChatPlugin
	{
	public:
		TimestampPlugin();

		static PluginFramework::IPlugin* Create();
		static void Destroy(PluginFramework::IPlugin *pInstance_in);
		static void Query(PluginInfo& Info_out);

		static int SetFormat(const WindowerCommand *pCommand_in);
		bool SetFormat(const std::string& Format_in);

		bool OnChatMessage(USHORT MessageType, const StringNode* pSender_in_out,
						   StringNode* pMessage_in_out, const char *pOriginalMsg_in,
						   DWORD dwOriginalMsgSize, char **pBuffer_in_out,
						   bool &Unsubscribe_out);
	private:
		//! the format of the timestamp plugin
		std::string		m_TimestampFormat;
		//! the length of the formatted timestamp
		unsigned int	m_TimestampLength;
	};
}

#endif//__TIMESTAMP_PLUGIN_H__