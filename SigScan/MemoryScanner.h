/**************************************************************************
	created		:	2013-04-08
	filename	: 	MemoryScanner.h
	author		:	Xebeth`
	copyright	:	North Edge (2013)
	purpose		:	
**************************************************************************/
#ifndef __MEMORY_SCANNER_H__
#define __MEMORY_SCANNER_H__

#include <NonCopyable.h>
#include <StringUtils.h>
#include <vector>
#include <map>

#define _MEMORY_SCANNER_SDK_INCLUDE_

#include <SigScan.h>
#include <ScanResult.h>
#include <ProcessImage.h>

#ifdef _DEBUG
	#pragma comment(lib, "SigScan.x86d.lib")
#else
	#pragma comment(lib, "SigScan.x86.lib")
#endif // _DEBUG

#undef _MEMORY_SCANNER_SDK_INCLUDE_

#endif//__MEMORY_SCANNER_H__
