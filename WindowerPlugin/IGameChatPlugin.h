/**************************************************************************
	created		:	2011-06-04
	filename	: 	IGameChatPlugin.h
	author		:	Xebeth`
	copyright	:	North Edge (2011)
	purpose		:	Base class for plugins subscribing to the FormatChatMessage service
**************************************************************************/
#ifndef __IGAME_CHAT_PLUGIN_H__
#define __IGAME_CHAT_PLUGIN_H__

typedef struct _StringNode
{
	const char	*pResBuf;		// +4
	DWORD		 dwUnknown;		// +8
	DWORD		 dwSize;		// +12
	bool		 bUnknown;		// +16
	bool		 bUnknown2;		// +17
	const char  *pUnknown;		// +18
} StringNode;

enum CHAT_MESSAGE_TYPE
{
	CHAT_MESSAGE_TYPE_SAY_MESSAGE				= 0x0001,	//!< say
	CHAT_MESSAGE_TYPE_SHOUT_MESSAGE				= 0x0002,	//!< shout
	CHAT_MESSAGE_TYPE_INCOMING_TELL_MESSAGE		= 0x0003,	//!< incoming tell
	CHAT_MESSAGE_TYPE_PARTY_MESSAGE				= 0x0004,	//!< party
	CHAT_MESSAGE_TYPE_LINKSHELL_MESSAGE			= 0x0005,	//!< linkshell
	CHAT_MESSAGE_TYPE_OUTGOING_TELL_MESSAGE		= 0x000D,	//!< outgoing tell
	CHAT_MESSAGE_TYPE_SYSTEM_MESSAGE			= 0x001D,	//!< system
	CHAT_MESSAGE_TYPE_ECHO_MESSAGE				= 0x0020,	//!< echo
	CHAT_MESSAGE_TYPE_INVALID_MESSAGE			= 0x0021,	//!< invalid
	CHAT_MESSAGE_TYPE_BATTLE_MESSAGE			= 0x0067,	//!< battle
};

namespace Windower
{
	using namespace PluginFramework;

	//! \brief Base class for plugins subscribing to the FormatChatMessage service
	class IGameChatPlugin : public IPlugin
	{
	public:
		/*! \brief IGameChatPlugin constructor
			\param[in] pServices_in : a pointer to the plugin services
		*/
		explicit IGameChatPlugin(PluginFramework::IPluginServices *pServices_in)
			: IPlugin(pServices_in) {}

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

	protected:
		virtual bool Unsubscribe();
		virtual bool Subscribe();
	};
}

#endif//__IGAME_CHAT_PLUGIN_H__