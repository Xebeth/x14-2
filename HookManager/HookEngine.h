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
#include <string>

#include <NonCopyable.h>
#include "HookManager.h"

#ifdef __USE_MINHOOK
	#include <MinHook.h>
	#include "MinHookManager.h"

	typedef HookEngineLib::MinHookManager HookEngine;
#elif defined __USE_DETOURS
	#include <detours.h>
	#include "DetoursManager.h"

	typedef HookEngineLib::DetoursManager HookEngine;
#elif defined __USE_EASYHOOK
	#include <EasyHook.h>
	#include "EasyHookManager.h"

	typedef HookEngineLib::EasyHookManager HookEngine;
#elif defined __USE_IATHOOK
	#include <IATPatcher.h>
	#include "IATHookManager.h"

	typedef HookEngineLib::IATHookManager HookEngine;
#else
	#error Unsupported hook engine
#endif

#endif//__HOOK_ENGINE_H__