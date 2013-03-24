/**************************************************************************
	created		:	2013-03-12
	filename	: 	FormatChatMsgTypes.h
	author		:	Xebeth`
	copyright	:	North Edge (2013)
	purpose		:	
**************************************************************************/
#ifndef __FORMAT_CHAT_MSG_TYPES_H__
#define __FORMAT_CHAT_MSG_TYPES_H__

namespace Windower
{
	#pragma pack(push, 1)

	typedef struct _StringNode
	{
		char		*pResBuf;		// +4
		DWORD		 dwCapacity;	// +8
		DWORD		 dwSize;		// +12
		DWORD		 dwUnknown2;	// +16
		bool		 bUnknown;		// +17
		bool		 bUnknown2;		// +18
		char		 cBuf0;
		char		 cBuf1;
		char		 cBuf2;
		char		 cBuf3;
	} StringNode;

	#pragma pack(pop)

	StringNode UpdateNode(char *pText_in, size_t TextLen_in, StringNode &Node_in_out);

	enum CHAT_MESSAGE_TYPE
	{
		CHAT_MESSAGE_TYPE_SYSTEM_MESSAGE			= 0x0003,	//!< system
		CHAT_MESSAGE_TYPE_SAY_MESSAGE				= 0x000A,	//!< say
		CHAT_MESSAGE_TYPE_SHOUT_MESSAGE				= 0x000B,	//!< shout
		CHAT_MESSAGE_TYPE_OUTGOING_TELL_MESSAGE		= 0x000C,	//!< outgoing tell
		CHAT_MESSAGE_TYPE_INCOMING_TELL_MESSAGE		= 0x000D,	//!< incoming tell
		CHAT_MESSAGE_TYPE_PARTY_MESSAGE				= 0x000E,	//!< party
		CHAT_MESSAGE_TYPE_LINKSHELL_MESSAGE			= 0x0011,	//!< linkshell
		CHAT_MESSAGE_TYPE_INVALID_MESSAGE			= 0x0021,	//!< invalid
		CHAT_MESSAGE_TYPE_ECHO_MESSAGE				= 0x0038,	//!< echo
		CHAT_MESSAGE_TYPE_SANCTUARY					= 0x0039,	//!< sanctuary?
		CHAT_MESSAGE_TYPE_ERROR_MESSAGE				= 0x003C,	//!< error
		CHAT_MESSAGE_TYPE_BATTLE_MESSAGE			= 0x0067,	//!< battle

		CHAT_MESSAGE_TYPE_PARTY_INVITE				= 0x1039,	//!< outgoing? invite
		CHAT_MESSAGE_TYPE_PARTY_JOIN				= 0x0839,	//!< party join

		BATTLE_MESSAGE_TYPE_ABILITY					= 0x042B,	//!< ability
		BATTLE_MESSAGE_TYPE_ABILITY_RESULT			= 0x042C,	//!< ability result?
		BATTLE_MESSAGE_TYPE_STATUS_EFFECT			= 0x04AE,	//!< status effect
	};

	#define FORMAT_CHAT_MESSAGE_HOOK "FormatChatMessage"

	#define TEXT_ARROW "\xe2\x87\x92 "
	#define TEXT_ARROW_LEN 4
}

#endif//__FORMAT_CHAT_MSG_TYPES_H__
