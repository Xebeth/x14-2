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
	class WindowerEngine;

	typedef HookEngineLib::IHookManager IHookManager;

	//! \brief Game chat module
	class GameChatCore : public WindowerCore
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

		GameChatCore(WindowerEngine &Engine_in_out, HookEngine &HookManager_in_out);

		ModuleService* CreateService(const string_t& ServiceName_in, const HookPointers &Hooks_in,
									 bool InvokePermission_in = false);
		bool RegisterHooks(const string_t& ServiceName_in, HookPointers &Hooks_out);
		bool RegisterServices();

	private:
		//! flag specifying if the chat log is on hold
		bool m_Active;
	};
}

#endif//__GAME_CHAT_CORE_H__