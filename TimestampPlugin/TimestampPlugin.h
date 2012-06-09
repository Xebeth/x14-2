/**************************************************************************
	created		:	2011-06-04
	filename	: 	TimestampPlugin.h
	author		:	Xebeth`
	copyright	:	North Edge (2011)
	purpose		:	Timestamp plugin
**************************************************************************/
#ifndef __TIMESTAMP_PLUGIN_H__
#define __TIMESTAMP_PLUGIN_H__

#define TIMESTAMP_DEFAULT_FORMAT	"[HH:mm:ss] "
#define TIMESTAMP_DEFAULT_LENGTH	11

namespace Windower
{
	class TimestampPlugin : public Windower::IGameChatPlugin, public CommandHandler
	{
		//! IDs of the commands registered with the plugin
		enum CommandMap
		{
			CMD_FORMAT = 0,	//!< sets the format of the timestamp
			CMD_COUNT		//!< number of registered commands
		};

	public:
		explicit TimestampPlugin(PluginFramework::IPluginServices *pServices_in);

		static bool Configure(PluginFramework::IPlugin *pInstance_in, const LPVOID pUserData_in);
		static PluginFramework::IPlugin* Create(PluginFramework::IPluginServices *pServices_in);
		static void Query(PluginFramework::PluginInfo& PluginInfo_out);
		static void Destroy(PluginFramework::IPlugin *pInstance_in);		

		virtual bool ExecuteCommand(INT_PTR CmdID_in, const WindowerCommand &Command_in, std::string &Feedback_out);
		bool SetFormat(const std::string& Format_in);

		bool OnChatMessage(USHORT MessageType_in, const StringNode* pSender_in_out,
						   StringNode* pMessage_in_out, const char *pOriginalMsg_in,
						   DWORD dwOriginalMsgSize_in, char **pBuffer_in_out,
						   bool &Unsubscribe_out);

	protected:
		bool UnregisterCommands();
		bool RegisterCommands();

	private:
		//! the format of the timestamp plugin
		std::string		m_TimestampFormat;
		//! the length of the formatted timestamp
		unsigned int	m_TimestampLength;
	};
}

#endif//__TIMESTAMP_PLUGIN_H__