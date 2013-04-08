/**************************************************************************
	created		:	2011-05-09
	filename	: 	stdafx.h
	author		:	Xebeth`
	copyright	:	North Edge (2011)
	purpose		:	Precompiled headers support
**************************************************************************/
#ifndef __TELL_DETECT_PLUGIN_AFX_H__
#define __TELL_DETECT_PLUGIN_AFX_H__

// #define __USE_VLD
#include <stdhdr.h>
#include <PluginSDK.h>

// tell Timestamp it's not exporting the InitPlugin C function
#ifndef NO_EXPORT
	#error Timestamp shouldn't export InitPlugin
#endif

#endif//__TELL_DETECT_PLUGIN_AFX_H__