/**************************************************************************
	created		:	2011-11-27
	filename	: 	stdafx.h
	author		:	Xebeth`
	copyright	:	North Edge (2011)
	purpose		:	Precompiled headers support
**************************************************************************/
#ifndef __STDAFX_H__
#define __STDAFX_H__

#define __USE_DETOURS
// #define __USE_VLD

#include <stdhdr.h>

#include <SettingsManager.h>
#include <PluginFramework.h>
#include <UIAL.h>

#define PROFILE_PREFIX_LENGTH 8
#define PROFILE_PREFIX _T("Profile:")

#endif//__STDAFX_H__