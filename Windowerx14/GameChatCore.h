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

	//! \brief Game chat module
	class GameChatCore : public WindowerCore
	{
	public:
		GameChatCore(WindowerEngine &Engine_in_out, CommandParser &Parser_in, CommandDispatcher &Dispatcher_in);
		~GameChatCore();

		// ICoreModule interface implementation
		void RegisterHooks(IHookManager &HookManager_in);
		void OnHookInstall(IHookManager &HookManager_in);

		//! \brief OnChatMessage hook
		bool FormatChatMessageHook(LPVOID pThis_in_out, USHORT MessageType_in, const StringNode* pSender_in, StringNode* pMessage_in_out);

		StringNode* CreateTextNodeHook(StringNode *pTextObject_out, const char *pText_in, int TextLength_in = -1);

	protected:
		bool DisplayWindowerVersion();

		virtual void OnSubscribe(const string_t &ServiceName_in,
								 const PluginSet &Subscribers_in);
		virtual void OnUnsubscribe(const string_t &ServiceName_in,
								   const PluginSet &Subscribers_in);
		bool FilterCommands(LPVOID pThis_in_out, USHORT MessageType_in,
							const StringNode* pSender_in,
							StringNode* pMessage_in);
		//! function pointer to the original FormatChatMessage function
		fnFormatChatMessage	m_pFormatChatMessageTrampoline;
		//! function pointer to the original CreateTextNode function
		fnCreateTextNode m_pCreateTextNodeTrampoline;

	private:
		PluginSet					 m_ChatFormatSubscribers;
		PluginSet					 m_CreateTextNodeSubscribers;
		bool						 m_bCreateTextNodeSubEmpty;
		//! the module service for the chat message formatting method
		ModuleService *m_pFormatChatMessage;
		//! the module service for the text node creation method
		ModuleService *m_pCreateTextNode;
		//! the command dispatcher
		CommandDispatcher &m_CommandDispatcher;
		//! the command parser
		CommandParser &m_CommandParser;
		//! the windower version injected in the start screen
		std::string	m_InjectVersion;
		//! critical section for thread synchronization
		CRITICAL_SECTION m_Lock;
	};
}

#endif//__GAME_CHAT_CORE_H__