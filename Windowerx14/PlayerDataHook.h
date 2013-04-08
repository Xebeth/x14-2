/**************************************************************************
	created		:	2013-04-06
	filename	: 	PlayerDataHook.h
	author		:	Xebeth`
	copyright	:	North Edge (2013)
	purpose		:	
**************************************************************************/
#ifndef __PLAYER_DATA_HOOK_H__
#define __PLAYER_DATA_HOOK_H__

// int __thiscall sub_86B2D0(int this) ; CharacterMgrInit
typedef int (WINAPI *fnCharacterMgrInit)(LPVOID pThis);
int WINAPI CharacterMgrInitHook(LPVOID pThis);

// int __thiscall sub_69C130(int this) ; GetSelectedTarget
typedef Windower::TargetData* (WINAPI *fnGetSelectedTarget)(LPVOID pThis);
Windower::TargetData* WINAPI GetSelectedTargetHook(LPVOID pThis);

#endif//__PLAYER_DATA_HOOK_H__
