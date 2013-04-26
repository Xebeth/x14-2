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

#define PLAYER_DATA_OFFSET								0x1B8

namespace Windower
{
	typedef struct _TargetData TargetData;
	class PlayerDataService;
	class WindowerEngine;
	
	class PlayerCore : public WindowerCore
	{
	public:
		PlayerCore();

		BaseModuleService* CreateService(const string_t& ServiceName_in, bool InvokePermission_in = false);
		bool RegisterServices();

		// ICoreModule interface implementation
		void RegisterHooks(HookEngineLib::IHookManager &HookManager_in);
		void OnHookInstall(HookEngineLib::IHookManager &HookManager_in);

		TargetData* GetSelectedTargetHook(LPVOID pThis);
		int CharacterMgrInitHook(LPVOID pThis_in_out);

		bool IsLoggedIn() const;

	protected:
		void OnSubscribe(ModuleService *pService_in_out, PluginFramework::IPlugin* pPlugin_in);

	private:
		PlayerDataService *m_pPlayerDataService;

		fnCharacterMgrInit m_pCharMgrInitTrampoline;
		fnGetSelectedTarget m_pGetTargetTrampoline;

		//! player target data structure
		TargetData *m_pPlayerTarget;
		//! address of the player data structure
		DWORD *m_pPlayerAddr;
	};
}

#endif//__PLAYER_CORE_H__
