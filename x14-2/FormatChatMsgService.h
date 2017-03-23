/**************************************************************************
	created		:	2013-03-12
	filename	: 	FormatChatMsgService.h
	author		:	Xebeth`
	copyright	:	North Edge (2013)
	purpose		:	
**************************************************************************/
#ifndef __MESSAGE_FORMAT_SERVICE_H__
#define __MESSAGE_FORMAT_SERVICE_H__
														 
#ifdef _M_X64
	//														   45 32 D2 33 DB											2015-07-14
	//														   45 32 D2 45 33 F6										2016-05-29
	//														   45 32 D2 33 ?? 4D										2016-09-24
	#define FORMAT_CHAT_MESSAGE_OPCODES_SIGNATURE			 "##4532D233??4D"
	#define FORMAT_CHAT_MESSAGE_OPCODES_SIGNATURE_OFFSET	 -39
	#define FORMAT_CHAT_MESSAGE_OPCODES_SIZE				  0 // 15
#else
	//														   33 C5 89 ?? ?? 8B ?? ?? 53 56 0F ?? ?? ?? 57				2015-07-14
	//														   33 C5 89 ?? ?? 8B ?? ?? 53 56 8B ?? ?? 57 0F 			2016-05-29
	#define FORMAT_CHAT_MESSAGE_OPCODES_SIGNATURE			"##33C589????8B????53568B????570F"
	#define FORMAT_CHAT_MESSAGE_OPCODES_SIGNATURE_OFFSET	-14
	#define FORMAT_CHAT_MESSAGE_OPCODES_SIZE				 9
#endif // _M_X64

#define GAME_CHAT_MODULE			"GameChat"
// available services for the module
#define FORMAT_CHAT_MESSAGE_HOOK	"FormatChatMessage"

namespace Windower
{
	typedef struct _StringNode StringNode;
	
	// int __thiscall sub_B52730(void *this, __int16 a2, int a3, int a4, int a5, char a6)
	// __int64 __fastcall sub_14090F1A0(__int64 a1, __int16 a2, __int64 a3, __int64 a4)
	typedef bool (WINAPI *fnFormatChatMessage)(LPVOID pThis_in_out, CHAT_MESSAGE_TYPE MessageType_in,
											   StringNode* pSender_in_out, StringNode* pMessage_in_out,
											   const __time64_t *pTimestamp_in, bool Unknown1);

	class FormatChatMsgService : public ModuleService
	{
	public:
		FormatChatMsgService(const string_t& Name_in, bool InvokePermission_in = false);

		static bool WINAPI FormatChatMessageHook(LPVOID pThis_in_out, CHAT_MESSAGE_TYPE MessageType_in,
												 StringNode* pSender_in_out, StringNode* pMessage_in_out,
												 const __time64_t *pTimestamp_in, bool Unknown1);
		static bool InjectMessage(const std::string &Msg_in, const std::string &Sender_in = "",
								  CHAT_MESSAGE_TYPE MessageType_in = CHAT_MESSAGE_TYPE_ECHO_MESSAGE);

	private:

		bool FormatChatMsgService::FormatMessage(LPVOID pThis_in_out, CHAT_MESSAGE_TYPE MessageType_in, StringNode* pSender_in_out,
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
