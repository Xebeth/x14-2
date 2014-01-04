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
														 //56576A006A006800				2013-12-21
#define INIT_CHARACTER_MGR_OPCODES_SIGNATURE			"##56576A006A006800"
#define INIT_CHARACTER_MGR_OPCODES_SIGNATURE_OFFSET		 0
#define INIT_CHARACTER_MGR_OPCODES_HOOK_SIZE			 11

														 //57E8AD52E8FF8B7D08578BD8E8		2013-12-21
#define SET_PLAYER_TARGET_OPCODES_SIGNATURE				"##57E8????????8B7D??57????E8"
#define SET_PLAYER_TARGET_OPCODES_SIGNATURE_OFFSET		-13
#define SET_PLAYER_TARGET_OPCODES_HOOK_SIZE				 13

														 //568BF1B803000000				2013-04-16
#define DESTROY_SINGLETONS_OPCODES_SIGNATURE			"##568BF1B803000000"
#define DESTROY_SINGLETONS_OPCODES_SIGNATURE_OFFSET		 0
#define DESTROY_SINGLETONS_OPCODES_HOOK_SIZE			 8

#define PLAYER_DATA_OFFSET								0x4
#define TARGET_DATA_OFFSET								0x40

#define DESTROY_SINGLETONS_HOOK "DestroySingletons"

namespace Windower
{
	typedef struct _TargetData TargetData;
	class PlayerDataService;
	class WindowerEngine;

	// int __thiscall sub_86B2D0(int this) ; CharacterMgrInit
	typedef int (WINAPI *fnCharacterMgrInit)(LPVOID pThis);
	// void __thiscall sub_5A1B80(void *this, int a2, int a3)
	typedef void (WINAPI *fnSetPlayerTarget)(LPVOID pThis_in_out, const TargetData *pNewTarget_in, int Unknown_in);
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

		static void WINAPI SetPlayerTargetHook(LPVOID pThis_in_out, const TargetData *pNewTarget_in, int Unknown_in);
		static int WINAPI CharacterMgrInitHook(LPVOID pThis_in_out);
		static int WINAPI DestroySingletonsHook(LPVOID pThis);

		bool IsLoggedIn() const;

	private:
		void OnSubscribe(ModuleService *pService_in_out, PluginFramework::IPlugin* pPlugin_in);
		
		fnDestroySingletons m_pDestroySingletonsTrampoline;
		fnCharacterMgrInit m_pCharMgrInitTrampoline;
		fnSetPlayerTarget m_pGetTargetTrampoline;

		const TargetData **m_pPlayerTarget;
		const TargetData **m_pPlayerData;

		//! player data service
		PlayerDataService *m_pPlayerDataService;
		//! calling context for the module hooks
		static CallingContext<PlayerCore> m_Context;
	};
}

#endif//__PLAYER_CORE_H__
