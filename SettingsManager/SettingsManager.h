/**************************************************************************
	created		:	2011-07-03
	filename	: 	SettingsManager.h
	author		:	Xebeth`
	copyright	:	North Edge (2011)
	purpose		:	Settings manager header
**************************************************************************/
#ifndef __SETTINGS_MANAGER_H__
#define __SETTINGS_MANAGER_H__

#include <StringUtils.h>
#include <hash_map>

#define _SETTINGS_MANAGER_SDK_INCLUDE_

#include <SimpleIni.h>
#include <SettingsInterface.h>
#include <SettingsIniFile.h>

#ifdef _DEBUG
	#pragma comment(lib, "SettingsManager.x86d.lib")
#else
	#pragma comment(lib, "SettingsManager.x86.lib")
#endif // _DEBUG

#undef _SETTINGS_MANAGER_SDK_INCLUDE_

#endif//__SETTINGS_MANAGER_H__