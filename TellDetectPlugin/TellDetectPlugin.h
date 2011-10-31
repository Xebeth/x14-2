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
		static PluginFramework::IPlugin* Create();
		static void Destroy(PluginFramework::IPlugin *pInstance_in);
		static void Query(PluginInfo& Info_out);

		bool OnChatMessage(USHORT MessageType_in, const StringNode* pSender_in_out,
						   StringNode* pMessage_in_out, const char *pOriginalMsg_in,
						   DWORD dwOriginalMsgSize_in, char **pBuffer_in_out,
						   bool &Unsubscribe_out);
	};
}

#endif//__TELL_DETECT_PLUGIN_H__