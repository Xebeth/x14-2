/**************************************************************************
	created		:	2013-04-06
	filename	: 	PlayerCore.h
	author		:	Xebeth`
	copyright	:	North Edge (2013)
	purpose		:	Player data related module
**************************************************************************/
#ifndef __PLAYER_CORE_H__
#define __PLAYER_CORE_H__
														 //E8 ?? ?? ?? ?? 48 8D 0D ?? ?? ?? ?? 48 8B E8									2015-07-22
#define INIT_PLAYER_DATA_OPCODES_SIGNATURE				"##E8????????488D0D????????488BE8"
#define INIT_PLAYER_DATA_OPCODES_SIGNATURE_OFFSET		-36
#define INIT_PLAYER_DATA_OPCODES_HOOK_SIZE				 0

														 // outdated
#define SET_PLAYER_TARGET_OPCODES_SIGNATURE				"##57E8????????8B7D??57????E8"
#define SET_PLAYER_TARGET_OPCODES_SIGNATURE_OFFSET		-13
#define SET_PLAYER_TARGET_OPCODES_HOOK_SIZE				 13

														 //0F ?? ?? 66 C7 03 00 00														2015-07-22
#define FREE_PLAYER_DATA_OPCODES_SIGNATURE				"##0F????66C7030000"
#define FREE_PLAYER_DATA_OPCODES_SIGNATURE_OFFSET		-33
#define FREE_PLAYER_DATA_OPCODES_HOOK_SIZE				 0

														 //83 3D ?? ?? ?? ?? 00 48 8B ?? 48 8B ?? 0F ?? ?? ?? ?? ?? 49 89				2015-07-22
#define ON_CRAFTING_PROGRESS_OPCODES_SIGNATURE			"##833D????????00488B??488B??0F??????????4989"
#define ON_CRAFTING_PROGRESS_OPCODES_SIGNATURE_OFFSET	-33
#define ON_CRAFTING_PROGRESS_OPCODES_HOOK_SIZE			 0

#define PLAYER_DATA_OFFSET								0x4
#define TARGET_DATA_OFFSET								0x40

#define ON_CRAFTING_PROGRESS_HOOK	"OnCraftingProgress"
#define FREE_PLAYER_DATA_HOOK		"FreePlayerData"

namespace Windower
{
	typedef struct _TargetData TargetData;
	class PlayerDataService;
	class WindowerEngine;

	namespace Crafting
	{
		enum eCondition
		{
			Invalid = 0UL,
			Normal,
			Good,
			Excellent,
			Poor,
			Count
		};
	}

	typedef struct _CraftingData
	{
		BYTE State;
		BYTE Unknown[23];
		BYTE Step;
		BYTE Unknown2[28];
		Crafting::eCondition Condition;
		Crafting::eCondition PreviousCondition;
	} CraftingData;

	// int __fastcall sub_1406879B0(__int64 a1, __int64 a2)
	typedef int (WINAPI *fnInitPlayerData)(LPVOID pThis_in_out, LPVOID pUnknown_in);
	// void __thiscall sub_5A1B80(void *this, int a2, int a3)
	typedef void (WINAPI *fnSetPlayerTarget)(LPVOID pThis_in_out, const TargetData *pNewTarget_in, int Unknown_in);
	// signed __int64 __fastcall sub_1406491D0(__int64 pPlayerData)
	typedef int (WINAPI *fnFreePlayerData)(LPVOID pThis_in_out);
	// void __fastcall sub_14042A110(__int64 a1, __int64 a2)
	typedef int (WINAPI *fnOnCraftingProgress)(LPVOID pThis_in_out, CraftingData *pData_in);
	
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
		static void WINAPI OnCraftingProgressHook(LPVOID pThis_in_out, CraftingData *pData_in);
		static int WINAPI InitPlayerDataHook(LPVOID pThis_in_out, LPVOID pUnknown_in);
		static int WINAPI FreePlayerDataHook(LPVOID pThis);

		bool IsLoggedIn() const;

		Crafting::eCondition GetCraftingCondition() const { return m_CraftingCondition; }

	private:
		void OnSubscribe(ModuleService *pService_in_out, PluginFramework::IPlugin* pPlugin_in);
		
		fnOnCraftingProgress m_pOnCraftingProgressTrampoline;
		fnFreePlayerData m_pFreePlayerDataTrampoline;
		fnInitPlayerData m_pCharMgrInitTrampoline;
		fnSetPlayerTarget m_pGetTargetTrampoline;
		
		const TargetData **m_pPlayerTarget;
		const TargetData **m_pPlayerData;

		Crafting::eCondition m_CraftingCondition;
		//! player data service
		PlayerDataService *m_pPlayerDataService;
		//! calling context for the module hooks
		static CallingContext<PlayerCore> m_Context;
	};
}

#endif//__PLAYER_CORE_H__
