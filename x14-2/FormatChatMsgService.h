/**************************************************************************
	created		:	2013-03-12
	filename	: 	FormatChatMsgService.h
	author		:	Xebeth`
	copyright	:	North Edge (2013)
	purpose		:	
**************************************************************************/
#ifndef __MESSAGE_FORMAT_SERVICE_H__
#define __MESSAGE_FORMAT_SERVICE_H__
														 //81ECAC0200008B8424B80200005355578BF9		2013-03-01
														 //81ECAC0200008B8424B80200005355578BF9		2013-03-08
														 //5356578BF98DB71402000050					2013-03-16
														 //5356578BF98DB71402000050					2013-03-22
														 //538B5D0856578BF953897C242CE8F235FCFF		2013-04-05
														 //A1????????33C589????8B????56570FB7????	2013-09-04
														 //8BF18B4D10898D88FDFFFF578BCE
#define FORMAT_CHAT_MESSAGE_OPCODES_SIGNATURE			"##33C589????8B????56570FB7"
#define FORMAT_CHAT_MESSAGE_OPCODES_SIGNATURE_OFFSET	-14
#define FORMAT_CHAT_MESSAGE_OPCODES_HOOK_SIZE			 9

#define GAME_CHAT_MODULE			"GameChat"
// available services for the module
#define FORMAT_CHAT_MESSAGE_HOOK	"FormatChatMessage"

namespace Windower
{
	typedef struct _StringNode StringNode;

	// int __thiscall sub_570EC0(void *this, unsigned __int16 a2, int a3, int a4, __time64_t *ArgList) => search for %08X%04X in disassembly
	typedef bool (WINAPI *fnFormatChatMessage)(LPVOID pThis_in_out, USHORT MessageType_in, StringNode* pSender_in_out,
											   StringNode* pMessage_in_out, const __time64_t *pTimestamp_in);

	class FormatChatMsgService : public ModuleService
	{
	public:
		FormatChatMsgService(const string_t& Name_in, bool InvokePermission_in = false);

		static bool WINAPI FormatChatMessageHook(LPVOID pThis_in_out, USHORT MessageType_in, StringNode* pSender_in_out,
												 StringNode* pMessage_in_out, const __time64_t *pTimestamp_in);
		static bool InjectMessage(const std::string &Msg_in, const std::string &Sender_in = "",
								  UINT MessageType_in = CHAT_MESSAGE_TYPE_ECHO_MESSAGE);

	private:

		bool FormatChatMsgService::FormatMessage(LPVOID pThis_in_out, USHORT MessageType_in, StringNode* pSender_in_out,
												 StringNode* pMessage_in_out, char *pModifiedMsg_in, DWORD NewSize_in);
		void OnPointerChange(const std::string &HookName_in, LPVOID pPointer_in);

		//! game pointer to this
		LPVOID m_pChatMsg;
		//! trampoline for the format chat message function
		fnFormatChatMessage m_pFormatChatMsgTrampoline;
		//! calling context for the service hooks
		static CallingContext<FormatChatMsgService> m_Context;
	};
}

#endif//__MESSAGE_FORMAT_SERVICE_H__
