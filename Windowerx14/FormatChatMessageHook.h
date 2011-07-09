/**************************************************************************
	created		:	2011-06-03
	filename	: 	FormatChatMessageHook.h
	author		:	Xebeth`
	copyright	:	North Edge (2011)
	purpose		:	Game chat hook definitions
**************************************************************************/
#ifndef __CHAT_MESSAGE_FORMAT_HOOK_H__
#define __CHAT_MESSAGE_FORMAT_HOOK_H__

typedef struct _GameChatTextObject
{
	char	*pResBuf;
	DWORD	dwUnknown;
	DWORD	dwSize;
} GameChatTextObject;

// int __thiscall sub_504EA0(int this, unsigned __int16 a2, int a3, int a4) => search for %04X in disassembly
typedef bool (WINAPI *fnFormatChatMessage)(void* _this, USHORT MessageType, const GameChatTextObject* pSender, GameChatTextObject* pMessage);
bool WINAPI FormatChatMessageHook(void* _this, USHORT MessageType, const GameChatTextObject* pSender, GameChatTextObject* pMessage);

#define FORMAT_CHAT_HEAD_POINTER_OFFSET					64
#define FORMAT_CHAT_TAIL_POINTER_OFFSET					68

#define FORMAT_CHAT_MESSAGE_OPCODES_SIGNATURE_OFFSET	-60
#define FORMAT_CHAT_MESSAGE_OPCODES_SIGNATURE			"@@78010000??A3000000000F"
#define FORMAT_CHAT_MESSAGE_OPCODES_HOOK_SIZE			13

enum CHAT_MESSAGE_TYPE
{
	CHAT_MESSAGE_TYPE_SAY_MESSAGE				= 0x0001,
	CHAT_MESSAGE_TYPE_SHOUT_MESSAGE				= 0x0002,
	CHAT_MESSAGE_TYPE_INCOMING_TELL_MESSAGE		= 0x0003,
	CHAT_MESSAGE_TYPE_PARTY_MESSAGE				= 0x0004,
	CHAT_MESSAGE_TYPE_LINKSHELL_MESSAGE			= 0x0005,
	CHAT_MESSAGE_TYPE_OUTGOING_TELL_MESSAGE		= 0x000D,
	CHAT_MESSAGE_TYPE_SYSTEM_MESSAGE			= 0x001D,
	CHAT_MESSAGE_TYPE_ECHO_MESSAGE				= 0x0020,
	CHAT_MESSAGE_TYPE_INVALID_MESSAGE			= 0x0021,
	CHAT_MESSAGE_TYPE_BATTLE_MESSAGE			= 0x0067,
};

#endif//__CHAT_MESSAGE_FORMAT_HOOK_H__