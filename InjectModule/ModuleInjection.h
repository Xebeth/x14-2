/**************************************************************************
	created		:	2013-04-08
	filename	: 	ModuleInjection.h
	author		:	Xebeth`
	copyright	:	North Edge (2013)
	purpose		:	Module injection library
**************************************************************************/
#ifndef __MODULE_INJECTION_H__
#define __MODULE_INJECTION_H__

#define _MODULE_INJECTION_SDK_INCLUDE_

#include <StringUtils.h>
#include <InjectModule.h>

#ifdef _DEBUG
	#pragma comment(lib, "InjectModule.x86d.lib")
#else
	#pragma comment(lib, "InjectModule.x86.lib")
#endif // _DEBUG

#undef _MODULE_INJECTION_SDK_INCLUDE_

#endif//__MODULE_INJECTION_H__
