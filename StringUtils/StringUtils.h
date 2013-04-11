/**************************************************************************
	created		:	2013-04-08
	filename	: 	StringUtils.h
	author		:	Xebeth`
	copyright	:	North Edge (2013)
	purpose		:	String & misc. utilities library
**************************************************************************/
#ifndef __STRING_UTILS_H__
#define __STRING_UTILS_H__

#include <NonCopyable.h>
#include <tchar.h>
#include <string>
#include <list>

#define _STRING_UTILS_SDK_INCLUDE_

#include <string_t.h>
#include <utf8_convert.h>

#include <CryptUtils.h>
#include <UUID.h>

#ifdef _DEBUG
	#pragma comment(lib, "StringUtils.x86d.lib")
#else
	#pragma comment(lib, "StringUtils.x86.lib")
#endif // _DEBUG

#undef _STRING_UTILS_SDK_INCLUDE_

#endif//__STRING_UTILS_H__
