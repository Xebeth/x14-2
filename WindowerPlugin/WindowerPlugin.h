/**************************************************************************
	created		:	2012-07-07
	filename	: 	WindowerPlugin.h
	author		:	Xebeth`
	copyright	:	North-Edge (2012)
	purpose		:	
**************************************************************************/
#ifndef __WINDOWER_PLUGIN_H__
#define __WINDOWER_PLUGIN_H__

#include <stdhdr.h>
#include <string_t.h>

#include <IGameChatPlugin.h>

#ifndef PLUGIN_EXPORTS
	#error "Please define PLUGIN_EXPORTS in the preprocessor configuration"
#endif

#ifdef _DEBUG
	#pragma comment(lib, "WindowerPlugin.x86d.lib")
#else
	#pragma comment(lib, "WindowerPlugin.x86.lib")
#endif // _DEBUG

#endif//__WINDOWER_PLUGIN_H__
