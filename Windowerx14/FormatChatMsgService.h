/**************************************************************************
	created		:	2013-03-12
	filename	: 	FormatChatMsgService.h
	author		:	Xebeth`
	copyright	:	North Edge (2013)
	purpose		:	
**************************************************************************/
#ifndef __MESSAGE_FORMAT_SERVICE_H__
#define __MESSAGE_FORMAT_SERVICE_H__

#include <FormatChatMsgTypes.h>
														 //81ECAC0200008B8424B80200005355578BF9		2013-03-01
														 //81ECAC0200008B8424B80200005355578BF9		2013-03-08
														 //5356578BF98DB71402000050					2013-03-16
														 //5356578BF98DB71402000050					2013-03-22
#define FORMAT_CHAT_MESSAGE_OPCODES_SIGNATURE			"@@5356578BF98D??????????50"
#define FORMAT_CHAT_MESSAGE_OPCODES_SIGNATURE_OFFSET	-27
#define FORMAT_CHAT_MESSAGE_OPCODES_HOOK_SIZE			 12

namespace Windower
{
	// int __thiscall sub_570EC0(void *this, unsigned __int16 a2, int a3, int a4, __time64_t *ArgList) => search for %08X%04X in disassembly
	typedef bool (WINAPI *fnFormatChatMessage)(LPVOID pThis_in_out, USHORT MessageType_in, StringNode* pSender_in_out,
											   StringNode* pMessage_in_out, const __time64_t *pTimestamp_in);

	class FormatChatMsgService : public ModuleService
	{
		class CallingContext : public BaseContext
		{
		public:
			CallingContext(fnFormatChatMessage pTrampoline_in, PluginSet &Subscribers_in)
				: BaseContext(pTrampoline_in, Subscribers_in) {}
		};
	public:
		FormatChatMsgService(const string_t& Name_in, const HookPointers &Hooks_in,
							 bool InvokePermission_in = false);

		static bool WINAPI FormatChatMessageHook(LPVOID pThis_in_out, USHORT MessageType_in, StringNode* pSender_in_out,
												 StringNode* pMessage_in_out, const __time64_t *pTimestamp_in);
		void DestroyContext();
		void CreateContext();

	private:
		static bool FormatChatMsgService::FormatMessage(LPVOID pThis_in_out, USHORT MessageType_in, StringNode* pSender_in_out,
														StringNode* pMessage_in_out, char *pModifiedMsg_in, DWORD NewSize_in,
														bool AlwaysShow_in = false);
		//! calling context for the service hooks
		static CallingContext *m_pContext;
	};
}

#endif//__MESSAGE_FORMAT_SERVICE_H__
