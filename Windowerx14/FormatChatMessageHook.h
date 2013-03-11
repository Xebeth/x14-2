/**************************************************************************
	created		:	2011-06-03
	filename	: 	FormatChatMessageHook.h
	author		:	Xebeth`
	copyright	:	North Edge (2011)
	purpose		:	Game chat hook definitions
**************************************************************************/
#ifndef __CHAT_MESSAGE_FORMAT_HOOK_H__
#define __CHAT_MESSAGE_FORMAT_HOOK_H__

#include <ICreateTextNodePlugin.h>

// int __thiscall sub_570EC0(void *this, unsigned __int16 a2, int a3, int a4, __time64_t *ArgList) => search for %08X%04X in disassembly
typedef bool (WINAPI *fnFormatChatMessage)(LPVOID pThis_in_out, USHORT MessageType_in, StringNode* pSender_in_out, StringNode* pMessage_in_out, const __time64_t *pTimestamp_in);
bool WINAPI FormatChatMessageHook(LPVOID pThis_in_out, USHORT MessageType_in, StringNode* pSender_in_out, StringNode* pMessage_in_out, const __time64_t *pTimestamp_in);

														 //81ECAC0200008B8424B80200005355578BF9		2013-03-01
														 //81ECAC0200008B8424B80200005355578BF9		2013-03-08
#define FORMAT_CHAT_MESSAGE_OPCODES_SIGNATURE			"@@81EC????00008B8424????00005355578BF9"
#define FORMAT_CHAT_MESSAGE_OPCODES_SIGNATURE_OFFSET	-18
#define FORMAT_CHAT_MESSAGE_OPCODES_HOOK_SIZE			 13

#endif//__CHAT_MESSAGE_FORMAT_HOOK_H__