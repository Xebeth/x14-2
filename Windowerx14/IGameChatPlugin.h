/**************************************************************************
	created		:	2011-06-04
	filename	: 	IGameChatPlugin.h
	author		:	Xebeth`
	copyright	:	North Edge (2011)
	purpose		:	Game chat plugin interface
**************************************************************************/
#ifndef __IGAME_CHAT_PLUGIN_H__
#define __IGAME_CHAT_PLUGIN_H__

#include <queue>

namespace Windower
{
	class IGameChatPlugin : public PluginFramework::IPlugin
	{
	public:
		IGameChatPlugin() : PluginFramework::IPlugin() {}

		virtual int FilterCommands(GameChatTextObject* pMessage_in_out, const char *pOriginalMsg_in,
								   DWORD dwOriginalMsgSize, char **pBuffer_in_out) { return -1; }
		virtual bool DispatchCommand(const std::queue<std::string> &Arguments_in,
									 const PluginFramework::IPluginServices *pServices_in) { return true; }
		virtual bool FormatChatMessage(USHORT MessageType, const GameChatTextObject* pSender_in_out,
									   GameChatTextObject* pMessage_in_out, const char *pOriginalMsg_in,
									   DWORD dwOriginalMsgSize, char **pBuffer_in_out) =0;
	};
}

#endif//__IGAME_CHAT_PLUGIN_H__