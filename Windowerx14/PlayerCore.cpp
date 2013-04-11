/**************************************************************************
	created		:	2013-04-06
	filename	: 	PlayerCore.cpp
	author		:	Xebeth`
	copyright	:	North Edge (2013)
	purpose		:	Player data related module
**************************************************************************/
#include "stdafx.h"

#include "PlayerDataHook.h"

#include "WindowerCore.h"
#include "PlayerCore.h"

#include "ModuleService.h"
#include "PlayerDataService.h"

#include "WindowerSettings.h"
#include "WindowerEngine.h"

namespace Windower
{
	/*! \brief PlayerCore constructor
		\param[in,out] Engine_in_out : Windower engine
		\param[in,out] HookManager_in_out : Hook manager
	*/
	PlayerCore::PlayerCore(WindowerEngine &Engine_in_out, HookEngine &HookManager_in_out)
		: WindowerCore(_T(PLAYER_DATA_MODULE), Engine_in_out, HookManager_in_out),
		  m_pPlayerData(NULL), m_pPlayerDataService(NULL),
		  m_pCharMgrInitTrampoline(NULL), m_pGetTargetTrampoline(NULL) {}

	/*! \brief Register the hooks for this module
		\param[in] HookManager_in : the hook manager
	*/
	void PlayerCore::RegisterHooks(HookEngineLib::IHookManager &HookManager_in)
	{
		// register the character manager initialization hook
		m_HookManager.RegisterHook(INIT_CHARACTER_MGR_HOOK, SIGSCAN_GAME_PROCESSA, INIT_CHARACTER_MGR_OPCODES_SIGNATURE,
								   INIT_CHARACTER_MGR_OPCODES_SIGNATURE_OFFSET, ::CharacterMgrInitHook,
								   INIT_CHARACTER_MGR_OPCODES_HOOK_SIZE);
		// register the selected target hook
		m_HookManager.RegisterHook(GET_SELECTED_TARGET_HOOK, SIGSCAN_GAME_PROCESSA, GET_SELECTED_TARGET_OPCODES_SIGNATURE,
								   GET_SELECTED_TARGET_OPCODES_SIGNATURE_OFFSET, ::GetSelectedTargetHook,
								   GET_SELECTED_TARGET_OPCODES_HOOK_SIZE);
	}

	/*! \brief Callback invoked when the hooks of the module are installed
		\param[in] HookManager_in : the hook manager
	*/
	void PlayerCore::OnHookInstall(HookEngineLib::IHookManager &HookManager_in)
	{
		m_pCharMgrInitTrampoline = (fnCharacterMgrInit)HookManager_in.GetTrampolineFunc(INIT_CHARACTER_MGR_HOOK);
		m_pGetTargetTrampoline = (fnGetSelectedTarget)HookManager_in.GetTrampolineFunc(GET_SELECTED_TARGET_HOOK);
	}

	int PlayerCore::CharacterMgrInitHook(LPVOID pThis_in_out)
	{
		int Result = 0;

		if (m_pCharMgrInitTrampoline != NULL)
		{
			Result = m_pCharMgrInitTrampoline(pThis_in_out);
			m_pPlayerData = *((TargetData**)((DWORD)pThis_in_out + PLAYER_DATA_OFFSET));

			if (m_pPlayerDataService != NULL)
				m_pPlayerDataService->OnPlayerPtrChange(m_pPlayerData);
		}

		return Result;
	}

	/*! \brief Registers the services of the module
		\return true if the services were registered; false otherwise
	*/
	bool PlayerCore::RegisterServices()
	{
		// register the services
		return (RegisterService(_T(INIT_CHARACTER_MGR_HOOK), false) != NULL);
	}

	/*! \brief Creates a service object given its name
		\param[in] ServiceName_in : the name of the service
		\param[in] Hooks_in : the hooks associated with the service
		\param[in] InvokePermission_in : flag specifying if the service can be invoked
		\return a pointer to the service object if successful; NULL otherwise
	*/
	ModuleService* PlayerCore::CreateService(const string_t& ServiceName_in, const HookPointers &Hooks_in, bool InvokePermission_in)
	{
 		m_pPlayerDataService = new PlayerDataService(ServiceName_in, Hooks_in, InvokePermission_in);

		return m_pPlayerDataService;
	}

	TargetData* PlayerCore::GetSelectedTargetHook(LPVOID pThis)
	{
		TargetData *pPreviousTarget = m_pPlayerTarget;

		m_pPlayerTarget = m_pGetTargetTrampoline(pThis);

		if (m_pPlayerTarget != pPreviousTarget && m_pPlayerDataService != NULL)
			m_pPlayerDataService->OnTargetPtrChange(m_pPlayerTarget);

		return m_pPlayerTarget;
	}

	bool PlayerCore::IsLoggedIn() const
	{
		return (m_pPlayerData && m_pPlayerData->Name[0] != '\0');
	}
}