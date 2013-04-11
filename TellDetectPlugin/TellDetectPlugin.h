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
	class TellDetectPlugin : public Windower::IGameChatPlugin
	{
	public:
		explicit TellDetectPlugin(PluginFramework::IPluginServices *pServices_in);
		~TellDetectPlugin();

		static bool Configure(PluginFramework::IPlugin *pInstance_in, const LPVOID pUserData_in);
		static PluginFramework::IPlugin* Create(PluginFramework::IPluginServices *pServices_in);
		static void Query(PluginFramework::PluginInfo& PluginInfo_out);
		static void Destroy(PluginFramework::IPlugin *pInstance_in);

		DWORD OnChatMessage(USHORT MessageType_in, const char* pSender_in,
							const char *pOriginalMsg_in, char **pModifiedMsg_in_out);

	protected:
		//! Timestamp plugin settings
		TellDetectSettings *m_pSettings;
		//! Path to the sound file
		string_t m_SoundFile;
	};
}

#endif//__TELL_DETECT_PLUGIN_H__