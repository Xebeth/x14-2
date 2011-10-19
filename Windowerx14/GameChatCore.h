/**************************************************************************
	created		:	2010-10-23
	filename	: 	GameChatCore.h
	author		:	xebeth`
	copyright	:	North Edge (2010)
	purpose		:	Interaction with the game chat memory
**************************************************************************/
#ifndef __GAME_CHAT_CORE_H__
#define __GAME_CHAT_CORE_H__

namespace Windower
{
	class IGameChatPlugin;

	typedef HookEngineLib::IHookManager IHookManager;

	class GameChatCore : public WindowerCore
	{
	public:
		GameChatCore(WindowerEngine &Engine_in,
					 CommandParser &Parser_in,
					 CommandDispatcher &Dispatcher_in);
		~GameChatCore();

		// ICoreModule interface implementation
		void RegisterHooks(IHookManager *pHookManager);
		void OnHookInstall(IHookManager *pHookManager);

		//! \brief FormatChatMessage hook
		bool FormatChatMessageHook(LPVOID _this, USHORT MessageType, const StringObject* pSender, StringObject* pMessage);

		StringObject* AllocStringHook(StringObject *pTextObject_out, const char *pText_in, UINT TextLength_in = -1);

	protected:
		bool DisplayWindowerVersion();

		void OnSubscribe(const string_t &ServiceName_in,
						 const PluginSet &Subscribers_in);
		void OnUnsubscribe(const string_t &ServiceName_in,
						   const PluginSet &Subscribers_in);
		bool FilterCommands(LPVOID _this, USHORT MessageType_in,
							const StringObject* pSender_in,
							StringObject* pMessage_in);

		fnFormatChatMessage	m_pFormatChatMessageTrampoline;
		fnAllocString		m_pAllocStringTrampoline;

		//! pointer to the start of the chat memory
		char			 **m_pChatHead;
		//! pointer to the previous start of the chat memory
		char			  *m_pPrevChatHead;
		//! pointer to the write position in the chat memory
		char			 **m_pChatTail;
		//! pointer to the previous write position in the chat memory
		char			  *m_pPrevChatTail;
		//! vector keeping track of the chat head positions
		std::vector<char*> m_ChatHeadVector;
	private:
		void UpdateChatData(const void *pChatObj_in);

		bool						 m_bAllocStringSubEmpty;
		PluginSet					 m_ChatFormatSubscribers;
		PluginSet					 m_AllocStringSubscribers;
		CommandDispatcher			&m_CommandDispatcher;
		CommandParser				&m_CommandParser;
		std::string					 m_InjectVersion;
	};
}

#endif//__GAME_CHAT_CORE_H__