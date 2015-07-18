/**************************************************************************
	created		:	2011-06-04
	filename	: 	IGameChatPlugin.h
	author		:	Xebeth`
	copyright	:	North Edge (2011)
	purpose		:	Base class for plugins subscribing to the FormatChatMessage service
**************************************************************************/
#ifndef __IGAME_CHAT_PLUGIN_H__
#define __IGAME_CHAT_PLUGIN_H__

#ifndef _WINDOWER_PLUGIN_SDK_INCLUDE_
	#error Please include the global header file 'PluginSDK.h'
#endif

namespace Windower
{
	enum eMsgFlags
	{
		MSG_FLAG_NONE		= 0x0000000,
		MSG_FLAG_DISCARD	= 0x0000001,
		MSG_FLAG_FORCE_ECHO	= 0x0000002,
		MSG_FLAG_EXEC		= 0x0000004,
	};

	enum CHAT_MESSAGE_TYPE
	{
		CHAT_MESSAGE_TYPE_SYSTEM_MESSAGE			= 0x0003,	//!< system
		CHAT_MESSAGE_TYPE_SAY_MESSAGE				= 0x000A,	//!< say
		CHAT_MESSAGE_TYPE_SHOUT_MESSAGE				= 0x000B,	//!< shout
		CHAT_MESSAGE_TYPE_OUTGOING_TELL_MESSAGE		= 0x000C,	//!< outgoing tell
		CHAT_MESSAGE_TYPE_INCOMING_TELL_MESSAGE		= 0x000D,	//!< incoming tell
		CHAT_MESSAGE_TYPE_PARTY_MESSAGE				= 0x000E,	//!< party
		CHAT_MESSAGE_TYPE_LINKSHELL_MESSAGE			= 0x0011,	//!< linkshell
		CHAT_MESSAGE_TYPE_YELL_MESSAGE				= 0x001E,	//!< yell
		CHAT_MESSAGE_TYPE_INVALID_MESSAGE			= 0x0021,	//!< invalid
		CHAT_MESSAGE_TYPE_ECHO_MESSAGE				= 0x0038,	//!< echo
		CHAT_MESSAGE_TYPE_NOTICE					= 0x0039,	//!< help message (/?) and sanctuary, etc.
		CHAT_MESSAGE_TYPE_ERROR_MESSAGE				= 0x003C,	//!< error
		CHAT_MESSAGE_TYPE_BATTLE_MESSAGE			= 0x0067,	//!< battle

		CHAT_MESSAGE_TYPE_PARTY_INVITE				= 0x1039,	//!< outgoing? invite
		CHAT_MESSAGE_TYPE_PARTY_JOIN				= 0x0839,	//!< party join

		BATTLE_MESSAGE_TYPE_ABILITY					= 0x042B,	//!< ability
		BATTLE_MESSAGE_TYPE_ABILITY_RESULT			= 0x042C,	//!< ability result?
		BATTLE_MESSAGE_TYPE_EXPERIENCE_GAIN			= 0x0440,	//!< experience?
		BATTLE_MESSAGE_TYPE_STATUS_EFFECT			= 0x04AE,	//!< status effect
	};

	//! \brief Base class for plugins subscribing to the FormatChatMessage service
	class IGameChatPlugin : public WindowerPlugin
	{
	public:
		/*! \brief Callback invoked when the game chat receives a new line
			\param[in] MessageType_in : the type of the message
			\param[in] pSender_in : the sender of the message
			\param[in] MsgSize_in : the size of the unmodified message
			\param[in] pOriginalMsg_in : a pointer to the unmodified message
			\param[in] pModifiedMsg_in_out : the resulting text modified by the plugin
			\param[in] dwNewSize_out : the new size of the message
			\param[in] DWORD ModifiedSize_in : the modified message size
			\return the new size of the message
		*/
		virtual DWORD_PTR OnChatMessage(DWORD_PTR MessageType_in, const char* pSender_in, DWORD_PTR MsgSize_in, const char *pOriginalMsg_in,
										char **pModifiedMsg_in_out, DWORD_PTR ModifiedSize_in, DWORD &MessageFlags_out) =0;

	protected:
		/*! \brief IGameChatPlugin constructor
			\param[in] pServices_in : a pointer to the plugin services
		*/
		explicit IGameChatPlugin(PluginFramework::IPluginServices *pServices_in)
			: WindowerPlugin(pServices_in) {}

		bool ResizeBuffer(const char *pSrc_in, DWORD_PTR SrcSize_in, DWORD_PTR dwNewSize_in,
						  char **pBuffer_in_out, DWORD_PTR BufferSize_in, DWORD_PTR Offset_in = 0UL) const;

		virtual bool Unsubscribe();
		virtual bool Subscribe();
	};
}

#endif//__IGAME_CHAT_PLUGIN_H__