/**************************************************************************
	created		:	2013-04-24
	filename	: 	ZipArchiveLib.h
	author		:	Xebeth`
	copyright	:	North Edge (2013)
	purpose		:	Zip archive library
**************************************************************************/
#ifndef __ZIP_ARCHIVE_LIB_H__
#define __ZIP_ARCHIVE_LIB_H__

#include <list>
#include <map>

#define _ZIP_ARCHIVE_SDK_INCLUDE_

#include <ZipTypes.h>
#include <ZipArchive.h>

#ifdef _DEBUG
	#pragma comment(lib, "ZipArchive.x86d.lib")
	#pragma comment(lib, "zlib.x86d.lib")
#else
	#pragma comment(lib, "ZipArchive.x86.lib")
	#pragma comment(lib, "zlib.x86.lib")
#endif // _DEBUG

#undef _ZIP_ARCHIVE_SDK_INCLUDE_

#endif//__ZIP_ARCHIVE_LIB_H__
