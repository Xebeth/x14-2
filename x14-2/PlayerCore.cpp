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
		: WindowerCore(_T(PLAYER_DATA_MODULE)), m_pPlayerData(nullptr),
		  m_pPlayerDataService(nullptr), m_pPlayerTarget(nullptr),
		  m_CraftingCondition(Crafting::Invalid),
		  m_pOnCraftingProgressTrampoline(nullptr),
		  m_pFreePlayerDataTrampoline(nullptr),		  
		  m_pCharMgrInitTrampoline(nullptr),
		  m_pGetTargetTrampoline(nullptr)
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
		HookManager_in.RegisterHook(INIT_PLAYER_DATA_HOOK, SIGSCAN_GAME_PROCESSA, INIT_PLAYER_DATA_OPCODES_SIGNATURE,
									INIT_PLAYER_DATA_OPCODES_SIGNATURE_OFFSET, &PlayerCore::InitPlayerDataHook,
									INIT_PLAYER_DATA_OPCODES_HOOK_SIZE);/*
		HookManager_in.RegisterHook(SET_PLAYER_TARGET_HOOK, SIGSCAN_GAME_PROCESSA, SET_PLAYER_TARGET_OPCODES_SIGNATURE,
									SET_PLAYER_TARGET_OPCODES_SIGNATURE_OFFSET, &PlayerCore::SetPlayerTargetHook,
									SET_PLAYER_TARGET_OPCODES_HOOK_SIZE);  */
		// register the destroy singletons hook
		HookManager_in.RegisterHook(FREE_PLAYER_DATA_HOOK, SIGSCAN_GAME_PROCESSA, FREE_PLAYER_DATA_OPCODES_SIGNATURE,
									FREE_PLAYER_DATA_OPCODES_SIGNATURE_OFFSET, &PlayerCore::FreePlayerDataHook,
									FREE_PLAYER_DATA_OPCODES_HOOK_SIZE);
		HookManager_in.RegisterHook(ON_CRAFTING_PROGRESS_HOOK, SIGSCAN_GAME_PROCESSA, ON_CRAFTING_PROGRESS_OPCODES_SIGNATURE,
									ON_CRAFTING_PROGRESS_OPCODES_SIGNATURE_OFFSET, &PlayerCore::OnCraftingProgressHook,
									ON_CRAFTING_PROGRESS_OPCODES_HOOK_SIZE);
	}

	/*! \brief Registers the services of the module
		\return true if the services were registered; false otherwise
	*/
	bool PlayerCore::RegisterServices()
	{
		// register the services
		return (RegisterService(_T(INIT_PLAYER_DATA_HOOK), false) != nullptr);
	}

	/*! \brief Callback invoked when the hooks of the module are installed
		\param[in] HookManager_in : the hook manager
	*/
	void PlayerCore::OnHookInstall(HookEngineLib::IHookManager &HookManager_in)
	{
		m_pOnCraftingProgressTrampoline = (fnOnCraftingProgress)HookManager_in.GetTrampolineFunc(ON_CRAFTING_PROGRESS_HOOK);
		m_pFreePlayerDataTrampoline = (fnFreePlayerData)HookManager_in.GetTrampolineFunc(FREE_PLAYER_DATA_HOOK);
		m_pCharMgrInitTrampoline = (fnInitPlayerData)HookManager_in.GetTrampolineFunc(INIT_PLAYER_DATA_HOOK);
		m_pGetTargetTrampoline = (fnSetPlayerTarget)HookManager_in.GetTrampolineFunc(SET_PLAYER_TARGET_HOOK);
 	}

	void PlayerCore::OnCraftingProgressHook(LPVOID pThis_in_out, CraftingData *pData_in)
	{
		if (pData_in != nullptr)
			m_Context->m_CraftingCondition = pData_in->Condition;

		if (m_Context->m_pOnCraftingProgressTrampoline != nullptr)
			m_Context->m_pOnCraftingProgressTrampoline(pThis_in_out, pData_in);
	}

	int PlayerCore::FreePlayerDataHook(LPVOID pThis)
	{
		int Result = 0;

		if (m_Context->m_pFreePlayerDataTrampoline != nullptr)
		{
			Result = m_Context->m_pFreePlayerDataTrampoline(pThis);
			m_Context->m_pPlayerDataService->OnPlayerPtrChange(nullptr);
			m_Context->m_pEngine->AbortMacro();
		}

		return Result;
	}

	int PlayerCore::InitPlayerDataHook(LPVOID pThis_in_out, LPVOID pUnknown_in)
	{
		if (m_Context->m_pCharMgrInitTrampoline != nullptr)
		{
			m_Context->m_pPlayerData = (const TargetData**)((DWORD_PTR)pThis_in_out + PLAYER_DATA_OFFSET);

			if (m_Context->m_pPlayerDataService != nullptr && m_Context->m_pPlayerData != nullptr)
				m_Context->m_pPlayerDataService->OnPlayerPtrChange(m_Context->m_pPlayerData);

			return m_Context->m_pCharMgrInitTrampoline(pThis_in_out, pUnknown_in);
		}

		return NULL;
	}

	void PlayerCore::SetPlayerTargetHook(LPVOID pThis_in_out, const TargetData *pNewTarget_in, int Unknown_in)
	{
		m_Context->m_pPlayerTarget = (const TargetData**)((DWORD_PTR)pThis_in_out + TARGET_DATA_OFFSET);
		m_Context->m_pGetTargetTrampoline(pThis_in_out, pNewTarget_in, Unknown_in);

		if (m_Context->m_pPlayerDataService != nullptr)
			m_Context->m_pPlayerDataService->OnTargetPtrChange(m_Context->m_pPlayerTarget);
	}

	void PlayerCore::Detach()
	{
		if (m_Context->m_pPlayerDataService != nullptr)
		{
			m_Context->m_pPlayerDataService->OnTargetPtrChange(nullptr);
			m_Context->m_pPlayerDataService->OnPlayerPtrChange(nullptr);
		}

		m_Context->m_pPlayerTarget = nullptr;
		m_Context->m_pPlayerData = nullptr;
	}

	bool PlayerCore::IsLoggedIn() const
	{
		return (m_pPlayerData != nullptr && *m_pPlayerData != nullptr && (*m_pPlayerData)->Name[0] != '\0');
	}

	/*! \brief Creates a service object given its name
		\param[in] ServiceName_in : the name of the service
		\param[in] InvokePermission_in : flag specifying if the service can be invoked
		\return a pointer to the service object if successful; nullptr otherwise
	*/
	BaseModuleService* PlayerCore::CreateService(const string_t& ServiceName_in, bool InvokePermission_in)
	{
 		m_pPlayerDataService = new PlayerDataService(ServiceName_in, InvokePermission_in);

		return m_pPlayerDataService;
	}

	void PlayerCore::OnSubscribe(ModuleService *pService_in_out, PluginFramework::IPlugin* pPlugin_in)
	{
		if (pService_in_out != nullptr && pService_in_out->GetName().compare(_T(INIT_PLAYER_DATA_HOOK)) == 0)
		{
			IPlayerDataPlugin *pPlugin = static_cast<IPlayerDataPlugin*>(pPlugin_in);

			if (pPlugin != nullptr)
				pPlugin->OnTargetPtrChange(m_pPlayerData, m_pPlayerTarget);
		}
	}
}