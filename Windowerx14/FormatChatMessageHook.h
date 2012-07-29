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

// int __thiscall sub_4CCB50(void *this, unsigned __int16 a2, int a3, int a4) => search for %04X in disassembly
typedef bool (WINAPI *fnFormatChatMessage)(LPVOID pThis_in_out, USHORT MessageType_in, StringNode* pSender_in_out, StringNode* pMessage_in_out);
bool WINAPI FormatChatMessageHook(LPVOID pThis_in_out, USHORT MessageType_in, StringNode* pSender_in_out, StringNode* pMessage_in_out);

#define FORMAT_CHAT_HEAD_POINTER_OFFSET					64
#define FORMAT_CHAT_TAIL_POINTER_OFFSET					68

#define FORMAT_CHAT_MESSAGE_OPCODES_SIGNATURE			"@@78010000??A3000000000F"
#define FORMAT_CHAT_MESSAGE_OPCODES_SIGNATURE_OFFSET	-60
#define FORMAT_CHAT_MESSAGE_OPCODES_HOOK_SIZE			 13

#define CREATETEXTNODE_OPCODES_SIGNATURE				"@@538B5C2408568BF1BA01000000"
#define CREATETEXTNODE_OPCODES_SIGNATURE_OFFSET		-13
#define CREATETEXTNODE_OPCODES_HOOK_SIZE				 8

// int _pThis_in_outcall sub_4470E0(int this, const char *a2, unsigned int a3)
typedef StringNode* (WINAPI *fnCreateTextNode)(StringNode *pTextObject_out, const char *pText_in, int TextLength_in);
StringNode* WINAPI CreateTextNodeHook(StringNode *pTextObject_out, const char *pText_in, int TextLength_in);

#endif//__CHAT_MESSAGE_FORMAT_HOOK_H__