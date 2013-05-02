/**************************************************************************
	created		:	2013-04-06
	filename	: 	PlayerCore.h
	author		:	Xebeth`
	copyright	:	North Edge (2013)
	purpose		:	Player data related module
**************************************************************************/
#ifndef __PLAYER_CORE_H__
#define __PLAYER_CORE_H__
														 //53565733DB536A21				2013-04-05
#define INIT_CHARACTER_MGR_OPCODES_SIGNATURE			"##53565733DB536A21"
#define INIT_CHARACTER_MGR_OPCODES_SIGNATURE_OFFSET		 0
#define INIT_CHARACTER_MGR_OPCODES_HOOK_SIZE			 8

														 //8B41288B492C3D000000E0		2013-04-05
#define GET_SELECTED_TARGET_OPCODES_SIGNATURE			"##8B41??8B49??3D000000E0"
#define GET_SELECTED_TARGET_OPCODES_SIGNATURE_OFFSET	 0
#define GET_SELECTED_TARGET_OPCODES_HOOK_SIZE			 11

														 //568BF1B803000000				2013-04-16
#define DESTROY_SINGLETONS_OPCODES_SIGNATURE			"##568BF1B803000000"
#define DESTROY_SINGLETONS_OPCODES_SIGNATURE_OFFSET		 0
#define DESTROY_SINGLETONS_OPCODES_HOOK_SIZE			 8

#define PLAYER_DATA_OFFSET								0x1B8

#define DESTROY_SINGLETONS_HOOK "DestroySingletons"

namespace Windower
{
	typedef struct _TargetData TargetData;
	class PlayerDataService;
	class WindowerEngine;

	// int __thiscall sub_86B2D0(int this) ; CharacterMgrInit
	typedef int (WINAPI *fnCharacterMgrInit)(LPVOID pThis);
	// int __thiscall sub_69C130(int this) ; GetSelectedTarget
	typedef TargetData* (WINAPI *fnGetSelectedTarget)(LPVOID pThis);
	// int __thiscall sub_7B4B60(int this) ; DestroySingletons
	typedef int (WINAPI *fnDestroySingletons)(LPVOID pThis);
	
	class PlayerCore : public WindowerCore
	{
	public:
		PlayerCore();

		BaseModuleService* CreateService(const string_t& ServiceName_in, bool InvokePermission_in = false);
		bool RegisterServices();
		void Detach();

		// ICoreModule interface implementation
		void RegisterHooks(HookEngineLib::IHookManager &HookManager_in);
		void OnHookInstall(HookEngineLib::IHookManager &HookManager_in);

		static TargetData* WINAPI GetSelectedTargetHook(LPVOID pThis);
		static int WINAPI CharacterMgrInitHook(LPVOID pThis_in_out);
		static int WINAPI DestroySingletonsHook(LPVOID pThis);

		bool IsLoggedIn() const;

	private:
		void OnSubscribe(ModuleService *pService_in_out, PluginFramework::IPlugin* pPlugin_in);
		
		fnDestroySingletons m_pDestroySingletonsTrampoline;
		fnCharacterMgrInit m_pCharMgrInitTrampoline;
		fnGetSelectedTarget m_pGetTargetTrampoline;
		//! player data service
		PlayerDataService *m_pPlayerDataService;
		//! player target data structure
		TargetData *m_pPlayerTarget;
		//! address of the player data structure
		DWORD *m_pPlayerAddr;
		//! calling context for the module hooks
		static CallingContext<PlayerCore> m_Context;
	};
}

#endif//__PLAYER_CORE_H__
