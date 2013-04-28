/**************************************************************************
	created		:	2013-04-06
	filename	: 	PlayerDataHook.h
	author		:	Xebeth`
	copyright	:	North Edge (2013)
	purpose		:	
**************************************************************************/
#ifndef __PLAYER_DATA_HOOK_H__
#define __PLAYER_DATA_HOOK_H__

namespace Windower
{
	typedef struct _TargetData TargetData;
}

// int __thiscall sub_86B2D0(int this) ; CharacterMgrInit
typedef int (WINAPI *fnCharacterMgrInit)(LPVOID pThis);
int WINAPI CharacterMgrInitHook(LPVOID pThis);

// int __thiscall sub_69C130(int this) ; GetSelectedTarget
typedef Windower::TargetData* (WINAPI *fnGetSelectedTarget)(LPVOID pThis);
Windower::TargetData* WINAPI GetSelectedTargetHook(LPVOID pThis);

// int __thiscall sub_7B4B60(int this) ; DestroySingletons
typedef int (WINAPI *fnDestroySingletons)(LPVOID pThis);
int WINAPI DestroySingletonsHook(LPVOID pThis);

#define DESTROY_SINGLETONS_HOOK "DestroySingletons"

#endif//__PLAYER_DATA_HOOK_H__
