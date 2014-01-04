/**************************************************************************
	created		:	2013-04-06
	filename	: 	PlayerCore.cpp
	author		:	Xebeth`
	copyright	:	North Edge (2013)
	purpose		:	Player data related module
**************************************************************************/
#include "stdafx.h"

#include "WindowerCore.h"
#include "PlayerCore.h"

#include "ModuleService.h"
#include "PlayerDataService.h"
#include "WindowerEngine.h"

namespace Windower
{
	WindowerCore::CallingContext<PlayerCore> PlayerCore::m_Context;

	/*! \brief PlayerCore constructor
		\param[in,out] Engine_in_out : Windower engine
		\param[in,out] HookManager_in_out : Hook manager
	*/
	PlayerCore::PlayerCore()
		: WindowerCore(_T(PLAYER_DATA_MODULE)), m_pPlayerData(NULL),
		  m_pPlayerDataService(NULL), m_pPlayerTarget(NULL),
		  m_pDestroySingletonsTrampoline(NULL),
		  m_pCharMgrInitTrampoline(NULL),
		  m_pGetTargetTrampoline(NULL)
	{
		// set the calling context for the hooks
		m_Context.Set(this);
	}

	/*! \brief Register the hooks for this module
		\param[in] HookManager_in : the hook manager
	*/
	void PlayerCore::RegisterHooks(HookEngineLib::IHookManager &HookManager_in)
	{
		// register the character manager initialization hook
		m_pHookManager->RegisterHook(INIT_CHARACTER_MGR_HOOK, SIGSCAN_GAME_PROCESSA, INIT_CHARACTER_MGR_OPCODES_SIGNATURE,
									 INIT_CHARACTER_MGR_OPCODES_SIGNATURE_OFFSET, &PlayerCore::CharacterMgrInitHook,
									 INIT_CHARACTER_MGR_OPCODES_HOOK_SIZE);
		m_pHookManager->RegisterHook(SET_PLAYER_TARGET_HOOK, SIGSCAN_GAME_PROCESSA, SET_PLAYER_TARGET_OPCODES_SIGNATURE,
									 SET_PLAYER_TARGET_OPCODES_SIGNATURE_OFFSET, &PlayerCore::SetPlayerTargetHook,
									 SET_PLAYER_TARGET_OPCODES_HOOK_SIZE);
		// register the destroy singletons hook
		m_pHookManager->RegisterHook(DESTROY_SINGLETONS_HOOK, SIGSCAN_GAME_PROCESSA, DESTROY_SINGLETONS_OPCODES_SIGNATURE,
									 DESTROY_SINGLETONS_OPCODES_SIGNATURE_OFFSET, &PlayerCore::DestroySingletonsHook,
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
		m_pGetTargetTrampoline = (fnSetPlayerTarget)HookManager_in.GetTrampolineFunc(SET_PLAYER_TARGET_HOOK);
 	}

	int PlayerCore::DestroySingletonsHook(LPVOID pThis)
	{
		int Result = 0;

		// shutdown the main thread
		m_pEngine->Detach();

		if (m_Context->m_pDestroySingletonsTrampoline != NULL)
			Result = m_Context->m_pDestroySingletonsTrampoline(pThis);

		m_pEngine->ShutdownEngine();

		return Result;
	}

	int PlayerCore::CharacterMgrInitHook(LPVOID pThis_in_out)
	{
		if (m_Context->m_pCharMgrInitTrampoline != NULL)
		{
			m_Context->m_pPlayerData = (const TargetData**)((DWORD)pThis_in_out + PLAYER_DATA_OFFSET);

			if (m_Context->m_pPlayerDataService != NULL && m_Context->m_pPlayerData != NULL)
				m_Context->m_pPlayerDataService->OnPlayerPtrChange(m_Context->m_pPlayerData);

			return m_Context->m_pCharMgrInitTrampoline(pThis_in_out);
		}

		return NULL;
	}

	void PlayerCore::SetPlayerTargetHook(LPVOID pThis_in_out, const TargetData *pNewTarget_in, int Unknown_in)
	{
		m_Context->m_pPlayerTarget = (const TargetData**)((DWORD)pThis_in_out + TARGET_DATA_OFFSET);
		m_Context->m_pGetTargetTrampoline(pThis_in_out, pNewTarget_in, Unknown_in);

		if (m_Context->m_pPlayerDataService != NULL)
			m_Context->m_pPlayerDataService->OnTargetPtrChange(m_Context->m_pPlayerTarget);
	}

	void PlayerCore::Detach()
	{
		if (m_Context->m_pPlayerDataService != NULL)
		{
			m_Context->m_pPlayerDataService->OnTargetPtrChange(NULL);
			m_Context->m_pPlayerDataService->OnPlayerPtrChange(NULL);
		}

		m_Context->m_pPlayerTarget = NULL;
		m_Context->m_pPlayerData = NULL;
	}

	bool PlayerCore::IsLoggedIn() const
	{
		return (m_pPlayerData != NULL && *m_pPlayerData != NULL && (*m_pPlayerData)->Name[0] != '\0');
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
				pPlugin->OnTargetPtrChange(m_pPlayerData, m_pPlayerTarget);
		}
	}
}