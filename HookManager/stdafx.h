/**************************************************************************
	created		:	2011-05-09
	filename	: 	stdafx.h
	author		:	Xebeth`
	copyright	:	North Edge (2011)
	purpose		:	Precompiled headers support
**************************************************************************/
#ifndef __HOOK_MANAGER_AFX_H__
#define __HOOK_MANAGER_AFX_H__

// #define __USE_VLD
#include <stdhdr.h>

#include <hash_map>
#include <StringUtils.h>
#include <MemoryScanner.h>

#ifdef _DEBUG
	#define __USE_DETOURS
	#define __USE_MINHOOK
	#define __USE_EASYHOOK
	#define __USE_IATHOOK
#else
	#define __USE_DETOURS
//	#define __USE_MINHOOK
//	#define __USE_EASYHOOK
//	#define __USE_IATHOOK
#endif // _DEBUG

#endif//__HOOK_MANAGER_AFX_H__