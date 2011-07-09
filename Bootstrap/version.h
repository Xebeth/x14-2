/**************************************************************************
	created		:	2011-03-07
	filename	: 	Version.h
	author		:	Xebeth`
	copyright	:	North Edge (2011)
	purpose		:	DLL version information
**************************************************************************/
#ifndef __MODULE_VERSION_INFO_H__
#define __MODULE_VERSION_INFO_H__

#define MODULE_FILE_DESC			"Windower for Final Fantasy XIV\0"
#define MODULE_COMPANY_NAME			"North Edge\0"
#define MODULE_COPYRIGHT			"Copyright (C) 2011 North Edge.  All rights reserved.\0"
#define MODULE_PRODUCT_NAME			"x14 Bootstrap\0"

#ifdef _M_X64
	#define MODULE_FILENAME			"bootstrap64.dll\0"
#else
	#define MODULE_FILENAME			"bootstrap.dll\0"
#endif

#define MODULE_PRODUCT_VER_STR		"1, 0, 3, 0\0"
#define MODULE_MAJOR_VERSION		1
#define MODULE_MINOR_VERSION		0
#define MODULE_RELEASE_VERSION		3
#define MODULE_TEST_VERSION			0

#define MODULE_FILE_VER_STR			"1, 0, 3, 0\0"
#define MODULE_PRODUCT_VERSION		"1.0.3.0\0"

#endif//__MODULE_VERSION_INFO_H__
