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

#define _WINDOWER_PLUGIN_SDK_INCLUDE_

#include <ICoreModule.h>

#include <CommandParameter.h>
#include <ICommandHandler.h>
#include <CommandHandler.h>
#include <WindowerCommand.h>

#include <WindowerPlugin.h>
#include <IGameChatPlugin.h>
#include <IPlayerDataPlugin.h>

#include <BaseEngine.h>
#include <PluginEngine.h>

#include <WindowerSettings.h>
#include <WindowerSettingsManager.h>

#if !defined PLUGIN_EXPORTS && !defined BOOTSTRAP_EXPORTS && !defined X14_EXPORTS
	#error Please define PLUGIN_EXPORTS in the preprocessor configuration
#endif

#ifdef _DEBUG
	#pragma comment(lib, "PluginSDK.x86d.lib")
#else
	#pragma comment(lib, "PluginSDK.x86.lib")
#endif // _DEBUG

#undef _WINDOWER_PLUGIN_SDK_INCLUDE_

#endif//__PLUGIN_SDK_H__
