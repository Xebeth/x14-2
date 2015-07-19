/**************************************************************************
	created		:	2011-06-04
	filename	: 	TellDetectPlugin.h
	author		:	Xebeth`
	copyright	:	North Edge (2011)
	purpose		:	Tell detect plugin
**************************************************************************/
#ifndef __TELL_DETECT_PLUGIN_H__
#define __TELL_DETECT_PLUGIN_H__

namespace Windower
{
	class PluginPropertyPage;

	class TellDetectPlugin : public Windower::IGameChatPlugin
	{
	public:
		explicit TellDetectPlugin(PluginFramework::IPluginServices *pServices_in);

		static PluginFramework::IPlugin* Create(PluginFramework::IPluginServices *pServices_in);
		static void Query(PluginFramework::PluginInfo& PluginInfo_out);
		static void Destroy(PluginFramework::IPlugin *pInstance_in);

		DWORD_PTR OnChatMessage(CHAT_MESSAGE_TYPE MessageType_in, const char* pSender_in, DWORD_PTR MsgSize_in, const char *pOriginalMsg_in,
								char **pModifiedMsg_in_out, DWORD_PTR ModifiedSize_in, DWORD &MessageFlags_out);
	protected:
		PluginPropertyPage* GetPropertyPage();
		void OnSettingsChanged();

		//! Path to the sound file
		string_t m_SoundFile;
	};
}

#endif//__TELL_DETECT_PLUGIN_H__