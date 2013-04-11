/**************************************************************************
	created		:	2011-06-04
	filename	: 	PluginFramework.h
	author		:	Xebeth`
	copyright	:	North Edge (2011)
	purpose		:	Plugin framework header
**************************************************************************/
#ifndef __PLUGIN_FRAMEWORK_H__
#define __PLUGIN_FRAMEWORK_H__

#include <StringUtils.h>
#include <hash_map>
#include <set>
#include <map>

#define _PLUGIN_FRAMEWORK_SDK_INCLUDE_

#include <VersionInfo.h>
#include <IPlugin.h>

#include <IPluginServices.h>
#include <FrameworkVersion.h>

#include <PluginManager.h>

#ifdef _DEBUG
	#pragma comment(lib, "PluginFramework.x86d.lib")
#else
	#pragma comment(lib, "PluginFramework.x86.lib")
#endif // _DEBUG

#undef _PLUGIN_FRAMEWORK_SDK_INCLUDE_

#endif//__PLUGIN_FRAMEWORK_H__