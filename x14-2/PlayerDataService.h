/**************************************************************************
	created		:	2013-04-06
	filename	: 	PlayerDataService.h
	author		:	Xebeth`
	copyright	:	North Edge (2013)
	purpose		:	Player data service
**************************************************************************/
#ifndef __PLAYER_DATA_SERVICE_H__
#define __PLAYER_DATA_SERVICE_H__

#define PLAYER_DATA_MODULE			"PlayerData"
// available services for the module
#define GET_SELECTED_TARGET_HOOK	"GetSelectedTarget"
#define INIT_CHARACTER_MGR_HOOK		"CharacterMgrInit"

namespace Windower
{
	#pragma pack(push, 1)

	typedef struct _TargetData
	{						// Alignment	Offset
		DWORD dwUnknown00;	// 0030			+0
		DWORD dwUnknown01;	// 0034			+4
		DWORD dwUnknown02;	// 0038			+8
		DWORD dwUnknown03;	// 003C			+12
		DWORD dwUnknown04;	// 0040			+16
		DWORD dwUnknown05;	// 0044			+20
		DWORD dwUnknown06;	// 0048			+24
		DWORD dwUnknown07;	// 004C			+28
		DWORD dwUnknown08;	// 0050			+32
		DWORD dwUnknown09;	// 0054			+36
		DWORD dwUnknown10;	// 0058			+40
		DWORD dwUnknown11;	// 005C			+44
		char Name[96];		// 0060			+48
		float PosX;			// 00C0			+144
		float PosY;			// 00C4			+148
		float PosZ;			// 00C8			+152
	} TargetData;

	#pragma pack(pop)

	class PlayerDataService : public ModuleService
	{
	public:
		PlayerDataService(const string_t& Name_in, bool InvokePermission_in = false);

		void OnTargetPtrChange(TargetData *pTargetData_in);
		void OnPlayerPtrChange(TargetData *pPlayerData_in);
		void DestroyContext() {} 
		void CreateContext();

	private:		
		TargetData *m_pPlayerTarget;
		TargetData *m_pPlayerData;
	};
}

#endif//__PLAYER_DATA_SERVICE_H__
