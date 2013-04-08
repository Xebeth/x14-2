/**************************************************************************
	created		:	2013-04-06
	filename	: 	PlayerDataTypes.h
	author		:	Xebeth`
	copyright	:	North Edge (2013)
	purpose		:	
**************************************************************************/
#ifndef __PLAYER_DATA_TYPES_H__
#define __PLAYER_DATA_TYPES_H__

#ifndef _WINDOWER_PLUGIN_SDK_INCLUDE_
	#error Please include the global header file 'PluginSDK.h'
#endif

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

	#define GET_SELECTED_TARGET_HOOK	"GetSelectedTarget"
	#define INIT_CHARACTER_MGR_HOOK		"CharacterMgrInit"
	#define PLAYER_DATA_SERVICE			"PlayerData"
	#define PLAYER_DATA_OFFSET			0x1B8
}

#endif//__PLAYER_DATA_TYPES_H__
