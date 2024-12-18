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

		GameChatCore();

		BaseModuleService* CreateService(const string_t& ServiceName_in, bool InvokePermission_in = false);
		bool RegisterHooks(ModuleService *pService_in_out);
		bool RegisterServices();

	private:
		//! flag specifying if the chat log is on hold
		bool m_Active;
	};
}

#endif//__GAME_CHAT_CORE_H__