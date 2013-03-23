/**************************************************************************
	created		:	2010-10-23
	filename	: 	GameChatCore.cpp
	author		:	xebeth`
	copyright	:	North Edge (2010)
	purpose		:	Interaction with the game chat memory
**************************************************************************/
#include "stdafx.h"
#include <PluginFramework.h>
#include <HookEngine.h>
#include "version.h"

#include <CommandHandler.h>
#include "WindowerSettings.h"
#include "WindowerEngine.h"

#include "IGameChatPlugin.h"
#include "ICreateTextNodePlugin.h"

#include "ICoreModule.h"
#include "WindowerCore.h"
#include "GameChatCore.h"

#include "ModuleService.h"
#include "FormatChatMsgService.h"

#include "WindowerCommand.h"
#include "CommandDispatcher.h"
#include "CommandParser.h"

namespace Windower
{
	/*! \brief GameChatCore constructor */
	GameChatCore::GameChatCore(WindowerEngine &Engine_in_out, HookEngine &HookManager_in_out) 
		: WindowerCore(_T("GameChat"), Engine_in_out, HookManager_in_out) {}

	/*! \brief Registers the services of the module
		\return true if the services were registered; false otherwise
	*/
	bool GameChatCore::RegisterServices()
	{
		// register the services
		return (RegisterService(_T(FORMAT_CHAT_MESSAGE_HOOK), false) != NULL);
	}

	/*! \brief Register the hooks of the specified service with the hook manager
		\param[in] pService_in : the service for which to register hooks
		\param[out] Hooks_out : the list of hooks to register
		\return true if the hooks were registered; false otherwise
	*/
	bool GameChatCore::RegisterHooks(const string_t& ServiceName_in, HookPointers &Hooks_out)
	{
		if (ServiceName_in.compare(_T(FORMAT_CHAT_MESSAGE_HOOK)) == 0)
		{
			// register the format chat message hook
			m_HookManager.RegisterHook(FORMAT_CHAT_MESSAGE_HOOK, SIGSCAN_GAME_PROCESSA, FORMAT_CHAT_MESSAGE_OPCODES_SIGNATURE,
									   FORMAT_CHAT_MESSAGE_OPCODES_SIGNATURE_OFFSET, &FormatChatMsgService::FormatChatMessageHook, 
									   FORMAT_CHAT_MESSAGE_OPCODES_HOOK_SIZE);
			// add it to the hook set
			Hooks_out[FORMAT_CHAT_MESSAGE_HOOK] = NULL;

			return true;
		}

		return false;
	}

	/*! \brief Creates a service object given its name
		\param[in] ServiceName_in : the name of the service
		\param[in] Hooks_in : the hooks associated with the service
		\param[in] InvokePermission_in : flag specifying if the service can be invoked
		\return a pointer to the service object if successful; NULL otherwise
	*/
	ModuleService* GameChatCore::CreateService(const string_t& ServiceName_in, const HookPointers &Hooks_in, bool InvokePermission_in)
	{
		return new FormatChatMsgService(ServiceName_in, Hooks_in, InvokePermission_in);
	}
}