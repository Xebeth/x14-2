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
		//! \brief TellDetectPlugin constructor
		explicit TellDetectPlugin(PluginFramework::IPluginServices *pServices_in)
			: IGameChatPlugin(pServices_in) {}

		static bool Configure(PluginFramework::IPlugin *pInstance_in, const LPVOID pUserData_in);
		static PluginFramework::IPlugin* Create(PluginFramework::IPluginServices *pServices_in);
		static void Query(PluginFramework::PluginInfo& PluginInfo_out);
		static void Destroy(PluginFramework::IPlugin *pInstance_in);

		bool OnChatMessage(USHORT MessageType_in, const StringNode* pSender_in_out,
						   StringNode* pMessage_in_out, const char *pOriginalMsg_in,
						   DWORD dwOriginalMsgSize_in, char **pBuffer_in_out,
						   bool &Unsubscribe_out);
	};
}

#endif//__TELL_DETECT_PLUGIN_H__