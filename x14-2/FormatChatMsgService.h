/**************************************************************************
	created		:	2013-03-12
	filename	: 	FormatChatMsgService.h
	author		:	Xebeth`
	copyright	:	North Edge (2013)
	purpose		:	
**************************************************************************/
#ifndef __MESSAGE_FORMAT_SERVICE_H__
#define __MESSAGE_FORMAT_SERVICE_H__
														 //4532D233DB						14/07/2015
#define FORMAT_CHAT_MESSAGE_OPCODES_SIGNATURE			"##4532D233DB"
#define FORMAT_CHAT_MESSAGE_OPCODES_SIGNATURE_OFFSET	-41

#define GAME_CHAT_MODULE			"GameChat"
// available services for the module
#define FORMAT_CHAT_MESSAGE_HOOK	"FormatChatMessage"

namespace Windower
{
	typedef struct _StringNode StringNode;
	
	// __int64 __fastcall sub_14090F1A0(__int64 a1, __int16 a2, __int64 a3, __int64 a4)
	typedef bool (WINAPI *fnFormatChatMessage)(LPVOID pThis_in_out, DWORD_PTR MessageType_in,
											   StringNode* pSender_in_out, StringNode* pMessage_in_out,
											   const __time64_t *pTimestamp_in, bool Unknown1);

	class FormatChatMsgService : public ModuleService
	{
	public:
		FormatChatMsgService(const string_t& Name_in, bool InvokePermission_in = false);

		static bool WINAPI FormatChatMessageHook(LPVOID pThis_in_out, DWORD_PTR MessageType_in,
												 StringNode* pSender_in_out, StringNode* pMessage_in_out,
												 const __time64_t *pTimestamp_in, bool Unknown1);
		static bool InjectMessage(const std::string &Msg_in, const std::string &Sender_in = "",
								  UINT MessageType_in = CHAT_MESSAGE_TYPE_ECHO_MESSAGE);

	private:

		bool FormatChatMsgService::FormatMessage(LPVOID pThis_in_out, DWORD_PTR MessageType_in, StringNode* pSender_in_out,
												 StringNode* pMessage_in_out, char *pModifiedMsg_in, DWORD_PTR NewSize_in,
											     const __time64_t *pTimestamp_in, bool Unknown1);
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
