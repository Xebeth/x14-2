/**************************************************************************
	created		:	2011-05-09
	filename	: 	stdafx.cpp
	author		:	Xebeth`
	copyright	:	North Edge (2011)
	purpose		:	Precompiled headers support
**************************************************************************/
#include "stdafx.h"

#ifdef _DEBUG
	#ifdef _M_X64
		#pragma comment(lib, "PluginFramework.x64d.lib")
		#pragma comment(lib, "SettingsManager.x64d.lib")
		#pragma comment(lib, "WindowerPlugin.x64d.lib")
	#else
		#pragma comment(lib, "PluginFramework.x86d.lib")
		#pragma comment(lib, "SettingsManager.x86d.lib")
		#pragma comment(lib, "WindowerPlugin.x86d.lib")
	#endif
#else
	#ifdef _M_X64
		#pragma comment(lib, "PluginFramework.x64.lib")
		#pragma comment(lib, "SettingsManager.x64.lib")
		#pragma comment(lib, "WindowerPlugin.x64.lib")
	#else
		#pragma comment(lib, "PluginFramework.x86.lib")
		#pragma comment(lib, "SettingsManager.x86.lib")
		#pragma comment(lib, "WindowerPlugin.x86.lib")
	#endif
#endif