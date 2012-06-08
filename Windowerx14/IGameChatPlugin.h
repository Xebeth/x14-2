/**************************************************************************
	created		:	2011-06-04
	filename	: 	IGameChatPlugin.h
	author		:	Xebeth`
	copyright	:	North Edge (2011)
	purpose		:	Base class for plugins subscribing to the FormatChatMessage service
**************************************************************************/
#ifndef __IGAME_CHAT_PLUGIN_H__
#define __IGAME_CHAT_PLUGIN_H__

#include <queue>

class StringNode;

namespace Windower
{
	//! \brief Base class for plugins subscribing to the FormatChatMessage service
	class IGameChatPlugin : public PluginFramework::IPlugin
	{
	public:
		/*! \brief IGameChatPlugin constructor
			\param[in] pServices_in : a pointer to the plugin services
		*/
		explicit IGameChatPlugin(PluginFramework::IPluginServices *pServices_in)
			: PluginFramework::IPlugin(pServices_in) {}

		/*! \brief Callback invoked when the game chat receives a new line
			\param[in] MessageType_in : the type of the message
			\param[in] pSender_in : the sender of the message
			\param[in,out] pMessage_in_out : the message (might have been modified by other plugins)
			\param[in] pOriginalMsg_in : a pointer to the unmodified message
			\param[in] dwOriginalMsgSize_in : the size of the original message
			\param[in] pBuffer_in_out : the resulting text modified by the plugin
			\param[in] Unsubscribe_out : flag specifying if the plugin wants to revoke its subscription to the hook
			\return true if the message was logged; false otherwise
		*/
		virtual bool OnChatMessage(USHORT MessageType_in, const StringNode* pSender_in_out,
								   StringNode* pMessage_in_out, const char *pOriginalMsg_in,
								   DWORD dwOriginalMsgSize_in, char **pBuffer_in_out, bool &Unsubscribe_out) =0;
	};
}

#endif//__IGAME_CHAT_PLUGIN_H__