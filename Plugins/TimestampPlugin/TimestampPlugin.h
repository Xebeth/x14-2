/**************************************************************************
	created		:	2011-06-04
	filename	: 	TimestampPlugin.h
	author		:	Xebeth`
	copyright	:	North Edge (2011)
	purpose		:	Timestamp plugin
**************************************************************************/
#ifndef __TIMESTAMP_PLUGIN_H__
#define __TIMESTAMP_PLUGIN_H__

namespace Windower
{
	class PluginPropertyPage;
	class TimestampSettings;	

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

		static PluginFramework::IPlugin* Create(PluginFramework::IPluginServices *pServices_in);
		static void Query(PluginFramework::PluginInfo& PluginInfo_out);
		static void Destroy(PluginFramework::IPlugin *pInstance_in);		

		bool ExecuteCommand(INT_PTR CmdID_in, const WindowerCommand &Command_in, std::string &Feedback_out);
		bool SetFormat(const std::string& Format_in);
		
		DWORD_PTR OnChatMessage(CHAT_MESSAGE_TYPE MessageType_in, const char* pSender_in, DWORD_PTR MsgSize_in, const char *pOriginalMsg_in,
								char **pModifiedMsg_in_out, DWORD_PTR ModifiedSize_in, DWORD &MessageFlags_out);

	protected:
		PluginPropertyPage* GetPropertyPage();
		void OnSettingsChanged();

		bool UnregisterCommands();
		bool RegisterCommands();

		//! the format of the timestamp plugin
		std::string	m_TimestampFormat;
		//! the length of the formatted timestamp
		size_t m_TimestampLength;
	};
}

#endif//__TIMESTAMP_PLUGIN_H__