// stdafx.cpp : source file that includes just the standard includes
// config.pch will be the pre-compiled header
// stdafx.obj will contain the pre-compiled type information

#include "stdafx.h"
//#include <vld.h>

#pragma comment(lib, "d3d9.lib")

#ifdef _DEBUG
	#ifdef _M_X64
		#pragma comment(lib, "PluginFramework.x64d.lib")
	#else
		#pragma comment(lib, "PluginFramework.x86d.lib")
	#endif
#else
	#ifdef _M_X64
		#pragma comment(lib, "PluginFramework.x64.lib")
	#else
		#pragma comment(lib, "PluginFramework.x86.lib")
	#endif
#endif