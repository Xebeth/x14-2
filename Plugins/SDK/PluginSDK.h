/**************************************************************************
	created		:	2013-04-08
	filename	: 	PluginSDK.h
	author		:	Xebeth`
	copyright	:	North Edge (2013)
	purpose		:	Windower Plugin SDK header
**************************************************************************/
#ifndef __PLUGIN_SDK_H__
#define __PLUGIN_SDK_H__

#include <PluginFramework.h>
#include <SettingsManager.h>

#ifdef __USE_UIAL
	#include <UIAL.h>
#endif

typedef hash_map<std::string, long> ScoredWords;

#define _WINDOWER_PLUGIN_SDK_INCLUDE_

#include <ICoreModule.h>

#include <CommandParameter.h>
#include <ICommandHandler.h>
#include <CommandHandler.h>
#include <WindowerCommand.h>

#include <ConfigurablePlugin.h>
#include <WindowerPlugin.h>
#include <IGameChatPlugin.h>
#include <IPlayerDataPlugin.h>

#include <BaseEngine.h>
#include <PluginEngine.h>
#include <PluginsServices.h>

#include <WindowerSettings.h>
#include <WindowerSettingsManager.h>

#if !defined PLUGIN_EXPORTS && !defined BOOTSTRAP_EXPORTS && !defined X14_EXPORTS
	#error Please define PLUGIN_EXPORTS in the preprocessor configuration
#endif

#ifdef _DEBUG
	#ifdef _M_X64
		#pragma comment(lib, "PluginSDK.x64d.lib")
	#else
		#pragma comment(lib, "PluginSDK.x86d.lib")
	#endif
#else
	#ifdef _M_X64
		#pragma comment(lib, "PluginSDK.x64.lib")
	#else
		#pragma comment(lib, "PluginSDK.x86.lib")
	#endif
#endif // _DEBUG

#undef _WINDOWER_PLUGIN_SDK_INCLUDE_

#endif//__PLUGIN_SDK_H__
