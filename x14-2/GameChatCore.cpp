/**************************************************************************
	created		:	2010-10-23
	filename	: 	GameChatCore.cpp
	author		:	xebeth`
	copyright	:	North Edge (2010)
	purpose		:	Interaction with the game chat memory
**************************************************************************/
#include "stdafx.h"

#include "WindowerCore.h"
#include "GameChatCore.h"

#include "WindowerEngine.h"

#include "ModuleService.h"
#include "FormatChatMsgService.h"

namespace Windower
{
	/*! \brief GameChatCore constructor */
	GameChatCore::GameChatCore() 
		: WindowerCore(_T(GAME_CHAT_MODULE)) {}

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
	bool GameChatCore::RegisterHooks(ModuleService *pService_in_out)
	{
		if (pService_in_out != NULL && pService_in_out->GetName().compare(_T(FORMAT_CHAT_MESSAGE_HOOK)) == 0)
		{
			// register the format chat message hook
			m_pHookManager->RegisterHook(FORMAT_CHAT_MESSAGE_HOOK, SIGSCAN_GAME_PROCESSA, FORMAT_CHAT_MESSAGE_OPCODES_SIGNATURE,
										 FORMAT_CHAT_MESSAGE_OPCODES_SIGNATURE_OFFSET, &FormatChatMsgService::FormatChatMessageHook,
										 FORMAT_CHAT_MESSAGE_OPCODES_SIZE);
			// add it to the hook set
			pService_in_out->SetPointer(FORMAT_CHAT_MESSAGE_HOOK, NULL);

			return true;
		}

		return false;
	}

	/*! \brief Creates a service object given its name
		\param[in] ServiceName_in : the name of the service
		\param[in] InvokePermission_in : flag specifying if the service can be invoked
		\return a pointer to the service object if successful; NULL otherwise
	*/
	BaseModuleService* GameChatCore::CreateService(const string_t& ServiceName_in, bool InvokePermission_in)
	{
		return new FormatChatMsgService(ServiceName_in, InvokePermission_in);
	}
}