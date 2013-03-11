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
	class GameChatCore : public WindowerCore, public CommandHandler
	{
	public:
		//! IDs of the registered commands
		enum CommandMap
		{
			CMD_START = 0,	//!< start chat
			CMD_STOP,		//!< stop chat
			CMD_TOGGLE,		//!< toggle chat
			CMD_COUNT		//!< number of registered commands
		};

		GameChatCore(WindowerEngine &Engine_in_out, IHookManager &HookManager_in,
					 CommandParser &Parser_in, CommandDispatcher &Dispatcher_in);
		~GameChatCore();

		// ICoreModule interface implementation
		void RegisterHooks(IHookManager &HookManager_in);
		void OnHookInstall(IHookManager &HookManager_in);

		bool FormatChatMessageHook(LPVOID pThis_in_out, USHORT MessageType_in, StringNode* pSender_in_out,
								   StringNode* pMessage_in_out, const __time64_t *pTimestamp_in);

		virtual bool ExecuteCommand(INT_PTR CmdID_in, const WindowerCommand &Command_in, std::string &Feedback_out);

	protected:
		bool DisplayWindowerVersion();
		bool UnregisterCommands();
		bool RegisterCommands();

		virtual void OnSubscribe(const string_t &ServiceName_in,
								 const PluginSet &Subscribers_in);
		virtual void OnUnsubscribe(const string_t &ServiceName_in,
								   const PluginSet &Subscribers_in);
		bool FilterCommands(LPVOID pThis_in_out, USHORT MessageType_in,
							StringNode* pSender_in_out, StringNode* pMessage_in);
		//! function pointer to the original FormatChatMessage function
		fnFormatChatMessage	m_pFormatChatMessageTrampoline;

	private:
		bool FormatMessage(LPVOID pThis_in_out, USHORT MessageType_in, StringNode* pSender_in_out,
						   StringNode* pMessage_in_out, char *pModifiedMsg_in, DWORD NewSize_in,
						   bool AlwaysShow_in = false);

		PluginSet					 m_ChatFormatSubscribers;

		//! Hook manager
		IHookManager &m_HookMgr;
		//! vector keeping track of the chat head positions
		std::vector<char*> m_ChatHeadVector;
		//! the command dispatcher
		CommandDispatcher &m_CommandDispatcher;
		//! the command parser
		CommandParser &m_CommandParser;
		//! the windower version injected in the start screen
		std::string	m_InjectVersion;
		//! flag specifying if the chat log is on hold
		bool m_Active;
	};
}

#endif//__GAME_CHAT_CORE_H__