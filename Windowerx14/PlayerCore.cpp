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
#include "WindowerEngine.h"

namespace Windower
{
	/*! \brief PlayerCore constructor
		\param[in,out] Engine_in_out : Windower engine
		\param[in,out] HookManager_in_out : Hook manager
	*/
	PlayerCore::PlayerCore()
		: WindowerCore(_T(PLAYER_DATA_MODULE)), m_pPlayerAddr(NULL),
		  m_pCharMgrInitTrampoline(NULL), m_pPlayerDataService(NULL), 
		  m_pPlayerTarget(NULL), m_pGetTargetTrampoline(NULL),
		  m_pDestroySingletonsTrampoline(NULL) {}

	/*! \brief Register the hooks for this module
		\param[in] HookManager_in : the hook manager
	*/
	void PlayerCore::RegisterHooks(HookEngineLib::IHookManager &HookManager_in)
	{
		// register the character manager initialization hook
		m_pHookManager->RegisterHook(INIT_CHARACTER_MGR_HOOK, SIGSCAN_GAME_PROCESSA, INIT_CHARACTER_MGR_OPCODES_SIGNATURE,
									 INIT_CHARACTER_MGR_OPCODES_SIGNATURE_OFFSET, ::CharacterMgrInitHook,
									 INIT_CHARACTER_MGR_OPCODES_HOOK_SIZE);
		// register the selected target hook
		m_pHookManager->RegisterHook(GET_SELECTED_TARGET_HOOK, SIGSCAN_GAME_PROCESSA, GET_SELECTED_TARGET_OPCODES_SIGNATURE,
									 GET_SELECTED_TARGET_OPCODES_SIGNATURE_OFFSET, ::GetSelectedTargetHook,
									 GET_SELECTED_TARGET_OPCODES_HOOK_SIZE);
		// register the destroy singletons hook
		m_pHookManager->RegisterHook(DESTROY_SINGLETONS_HOOK, SIGSCAN_GAME_PROCESSA, DESTROY_SINGLETONS_OPCODES_SIGNATURE,
									 DESTROY_SINGLETONS_OPCODES_SIGNATURE_OFFSET, ::DestroySingletonsHook,
									 DESTROY_SINGLETONS_OPCODES_HOOK_SIZE);
	}

	/*! \brief Registers the services of the module
		\return true if the services were registered; false otherwise
	*/
	bool PlayerCore::RegisterServices()
	{
		// register the services
		return (RegisterService(_T(INIT_CHARACTER_MGR_HOOK), false) != NULL);
	}

	/*! \brief Callback invoked when the hooks of the module are installed
		\param[in] HookManager_in : the hook manager
	*/
	void PlayerCore::OnHookInstall(HookEngineLib::IHookManager &HookManager_in)
	{
		m_pDestroySingletonsTrampoline = (fnDestroySingletons)HookManager_in.GetTrampolineFunc(DESTROY_SINGLETONS_HOOK);
		m_pCharMgrInitTrampoline = (fnCharacterMgrInit)HookManager_in.GetTrampolineFunc(INIT_CHARACTER_MGR_HOOK);
		m_pGetTargetTrampoline = (fnGetSelectedTarget)HookManager_in.GetTrampolineFunc(GET_SELECTED_TARGET_HOOK);
	}

	int PlayerCore::DestroySingletonsHook(LPVOID pThis)
	{
		// clear any saved player data
		Clear();
		// shutdown the main thread
		m_pEngine->OnClose();

		if (m_pDestroySingletonsTrampoline != NULL)
			return m_pDestroySingletonsTrampoline(pThis);

		return 0;
	}

	int PlayerCore::CharacterMgrInitHook(LPVOID pThis_in_out)
	{
		int Result = 0;

		if (m_pCharMgrInitTrampoline != NULL)
		{
			// >>> Critical section
			m_pEngine->LockEngineThread();

			Result = m_pCharMgrInitTrampoline(pThis_in_out);
			m_pPlayerAddr = (DWORD*)((DWORD)pThis_in_out + PLAYER_DATA_OFFSET);

			if (m_pPlayerDataService != NULL && m_pPlayerAddr != NULL)
			{
				TargetData *pPlayerData = *(TargetData**)m_pPlayerAddr;
				m_pPlayerDataService->OnPlayerPtrChange(pPlayerData);
			}

			m_pEngine->UnlockEngineThread();
			// Critical section <<<
		}

		return Result;
	}

	TargetData* PlayerCore::GetSelectedTargetHook(LPVOID pThis)
	{
		TargetData *pPreviousTarget = m_pPlayerTarget;

		m_pPlayerTarget = m_pGetTargetTrampoline(pThis);

		if (m_pPlayerTarget != pPreviousTarget && m_pPlayerDataService != NULL)
			m_pPlayerDataService->OnTargetPtrChange(m_pPlayerTarget);

		return m_pPlayerTarget;
	}

	void PlayerCore::Clear()
	{
		if (m_pPlayerDataService != NULL)
		{
			m_pPlayerDataService->OnTargetPtrChange(NULL);
			m_pPlayerDataService->OnPlayerPtrChange(NULL);
		}

		m_pPlayerTarget = NULL;
		m_pPlayerAddr = NULL;		
	}

	bool PlayerCore::IsLoggedIn() const
	{
		return (m_pPlayerAddr != NULL && *m_pPlayerAddr != NULL
			&& ((TargetData*)*m_pPlayerAddr)->Name[0] != '\0');
	}

	/*! \brief Creates a service object given its name
		\param[in] ServiceName_in : the name of the service
		\param[in] InvokePermission_in : flag specifying if the service can be invoked
		\return a pointer to the service object if successful; NULL otherwise
	*/
	BaseModuleService* PlayerCore::CreateService(const string_t& ServiceName_in, bool InvokePermission_in)
	{
 		m_pPlayerDataService = new PlayerDataService(ServiceName_in, InvokePermission_in);

		return m_pPlayerDataService;
	}

	void PlayerCore::OnSubscribe(ModuleService *pService_in_out, PluginFramework::IPlugin* pPlugin_in)
	{
		if (pService_in_out != NULL && pService_in_out->GetName().compare(_T(INIT_CHARACTER_MGR_HOOK)) == 0)
		{
			IPlayerDataPlugin *pPlugin = static_cast<IPlayerDataPlugin*>(pPlugin_in);

			if (pPlugin != NULL)
			{
				TargetPos Data;

				// update the player data for the new plugin
				if (m_pPlayerAddr != NULL)
				{
					TargetData *pPlayerData = *(TargetData**)m_pPlayerAddr;

					Data.pPosX = &pPlayerData->PosX;
					Data.pPosY = &pPlayerData->PosY;
					Data.pPosZ = &pPlayerData->PosZ;
					Data.pTargetName = pPlayerData->Name;
#ifdef _DEBUG
					Data.dwTargetAddr = *m_pPlayerAddr;
#endif // _DEBUG

					pPlugin->OnPlayerPtrChange(Data);
				}
				// update the target data for the new plugin
				if (m_pPlayerTarget != NULL)
				{
					Data.pPosX = &m_pPlayerTarget->PosX;
					Data.pPosY = &m_pPlayerTarget->PosY;
					Data.pPosZ = &m_pPlayerTarget->PosZ;
					Data.pTargetName = m_pPlayerTarget->Name;
#ifdef _DEBUG
					Data.dwTargetAddr = (DWORD)m_pPlayerTarget;
#endif // _DEBUG

					pPlugin->OnTargetPtrChange(Data);
				}
			}
		}
	}
}