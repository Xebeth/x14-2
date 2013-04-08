/**************************************************************************
	created		:	2013-04-08
	filename	: 	ImportTablePatcher.h
	author		:	Xebeth`
	copyright	:	North Edge (2013)
	purpose		:	Import table patcher library
**************************************************************************/
#ifndef __IMPORT_TABLE_PATCHER_H__
#define __IMPORT_TABLE_PATCHER_H__

#define _IAT_PATCHER_SDK_INCLUDE_

#include <IATPatcher.h>

#ifdef _DEBUG
	#pragma comment(lib, "IATPatcher.x86d.lib")
#else
	#pragma comment(lib, "IATPatcher.x86.lib")
#endif // _DEBUG

#undef _IAT_PATCHER_SDK_INCLUDE_

#endif//__IMPORT_TABLE_PATCHER_H__
