/**************************************************************************
	created		:	2011-10-31
	filename	: 	TestHook.cpp
	author		:	Xebeth`
	copyright	:	North Edge (2011)
	purpose		:	
**************************************************************************/
#include "stdafx.h"
#include <PluginFramework.h>
#include <HookEngine.h>

#include <CommandHandler.h>
#include "WindowerSettings.h"
#include "WindowerEngine.h"

#include "TestHook.h"

#include "ICoreModule.h"
#include "WindowerCore.h"
#include "TestCore.h"

extern Windower::WindowerEngine *g_pEngine;
// no initialized variable are allowed in the naked functions
extern DWORD g_5AEB30JumpAddr;

char WINAPI sub98B710Hook(LPVOID pThis, int a2, int a3, int a4, int a5)
{
	return g_pEngine->Test().sub98B710Hook(pThis, a2, a3, a4, a5);
}

// void __usercall sub_5AEB30(int a1<ecx>, int a2<ebp>)
__declspec(naked) void WINAPI sub5AEB30NakedHook()
{
	__asm
	{
		pushad;
		pushfd;
	}

	int a1, a2;

	__asm
	{
		// these register will be altered by calling sub5AEB30Hook
		push eax;
		push ecx;

		mov a1, ecx;
		mov a2, ebp;
	}

	g_pEngine->Test().sub5AEB30Hook(a1, a2);

	__asm
	{
		// restore the registers
		pop ecx;
		pop eax;
		popfd;
		popad;

		jmp g_5AEB30JumpAddr;

		ret;
	}
}