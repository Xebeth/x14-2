/**************************************************************************
	created		:	2011-06-03
	filename	: 	HookEngine.h
	author		:	Xebeth`
	copyright	:	North Edge (2011)
	purpose		:	Hook engine header
**************************************************************************/
#ifndef __HOOK_ENGINE_H__
#define __HOOK_ENGINE_H__

#include <hash_map>

#include <MemoryScanner.h>
#include <NonCopyable.h>

#define _HOOK_MANAGER_SDK_INCLUDE_

#include <HookManager.h>

#ifdef __USE_MINHOOK
	#include <MinHook.h>
	#include <MinHookManager.h>

	typedef HookEngineLib::MinHookManager HookEngine;
#elif defined __USE_DETOURS
	#include <detours.h>
	#include <DetoursManager.h>

	typedef HookEngineLib::DetoursManager HookEngine;
#elif defined __USE_EASYHOOK
	#include <EasyHook.h>
	#include <EasyHookManager.h>

	typedef HookEngineLib::EasyHookManager HookEngine;
#elif defined __USE_IATHOOK
	#include <ImportTablePatcher.h>
	#include <IATHookManager.h>

	typedef HookEngineLib::IATHookManager HookEngine;
#else
	#error No hook engine selected : define __USE_DETOURS, __USE_EASYHOOK, __USE_IATHOOK or __USE_MINHOOK
#endif

#ifdef _DEBUG
	#pragma comment(lib, "HookManager.x86d.lib")
#else
	#pragma comment(lib, "HookManager.x86.lib")
#endif // _DEBUG

#undef _HOOK_MANAGER_SDK_INCLUDE_

#endif//__HOOK_ENGINE_H__