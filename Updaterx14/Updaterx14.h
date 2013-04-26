/**************************************************************************
	created		:	2013-04-18
	filename	: 	Updaterx14.h
	author		:	Xebeth`
	copyright	:	North Edge (2013)
	purpose		:	
**************************************************************************/
#ifndef __UPDATER_X14_H__
#define __UPDATER_X14_H__

#include <Updater.h>
#include <PluginSDK.h>

#define UPDATER_API __declspec(dllimport)
#define _UPDATER_X14_SDK_INCLUDE_

#include <UpdateDlg.h>

#ifdef _DEBUG
	#pragma comment(lib, "Updaterx14.x86d.lib")
#else
	#pragma comment(lib, "Updaterx14.x86.lib")
#endif // _DEBUG

#undef _UPDATER_X14_SDK_INCLUDE_

#endif//__UPDATER_X14_H__
