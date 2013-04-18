/**************************************************************************
	created		:	2013-04-08
	filename	: 	StringUtils.h
	author		:	Xebeth`
	copyright	:	North Edge (2013)
	purpose		:	String & misc. utilities library
**************************************************************************/
#ifndef __UPDATER_H__
#define __UPDATER_H__

#include <StringUtils.h>
#include <UrlMon.h>
#include <vector>

#define _UPDATER_SDK_INCLUDE_

#include <IThreadProc.h>
#include <FileDownload.h>
#include <UpdaterThread.h>
#include <StreamProgress.h>
#include <IThreadProgress.h>

#ifdef _DEBUG
	#pragma comment(lib, "Updater.x86d.lib")
#else
	#pragma comment(lib, "Updater.x86.lib")
#endif // _DEBUG

#pragma comment(lib, "urlmon.lib")

#undef _UPDATER_SDK_INCLUDE_

#endif//__UPDATER_H__
