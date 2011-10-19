/**************************************************************************
	created		:	2011-06-03
	filename	: 	FormatChatMessageHook.h
	author		:	Xebeth`
	copyright	:	North Edge (2011)
	purpose		:	Game chat hook definitions
**************************************************************************/
#ifndef __CHAT_MESSAGE_FORMAT_HOOK_H__
#define __CHAT_MESSAGE_FORMAT_HOOK_H__

class StringObject
{
public:
	const char	*pResBuf;		// +4
	DWORD		 dwUnknown;		// +8
	DWORD		 dwSize;		// +12
	bool		 bUnknown;		// +16
	bool		 bUnknown2;		// +17
	const char  *pUnknown;		// +18
};

// int __thiscall sub_504EA0(int this, unsigned __int16 a2, int a3, int a4) => search for %04X in disassembly
typedef bool (WINAPI *fnFormatChatMessage)(void* _this, USHORT MessageType, const StringObject* pSender, StringObject* pMessage);
bool WINAPI FormatChatMessageHook(void* _this, USHORT MessageType, const StringObject* pSender, StringObject* pMessage);

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

#define ALLOCSTRING_OPCODES_SIGNATURE				"@@538B5C2408568BF1BA01000000"
#define ALLOCSTRING_OPCODES_HOOK_SIZE				8
#define ALLOCSTRING_OPCODES_SIGNATURE_OFFSET		-13

// int __thiscall sub_4470E0(int this, const char *a2, unsigned int a3)
typedef StringObject* (WINAPI *fnAllocString)(StringObject *pTextObject_out, const char *pText_in, UINT TextLength_in);
StringObject* WINAPI AllocStringHook(StringObject *pTextObject_out, const char *pText_in, UINT TextLength_in = -1);

#endif//__CHAT_MESSAGE_FORMAT_HOOK_H__