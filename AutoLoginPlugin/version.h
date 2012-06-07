/**************************************************************************
	created		:	2011-03-07
	filename	: 	Version.h
	author		:	Xebeth`
	copyright	:	North Edge (2011)
	purpose		:	DLL version information
**************************************************************************/
#ifndef __MODULE_VERSION_INFO_H__
#define __MODULE_VERSION_INFO_H__

#define W(x)    W_(x)
#define W_(x)   L ## x
#define N(x)    x
#define STR(x)	#x

#define MAKE_VER_(t, major, minor, release, test, sep)  t(STR(major)) t(sep) t(STR(minor)) t(sep) t(STR(release)) t(sep) t(STR(test)) t("\0")
#define MAKE_VER(major, minor, release, test, sep)      MAKE_VER_(N, major, minor, release, test, sep)
#define MAKE_VER_W(major, minor, release, test, sep)    MAKE_VER_(W, major, minor, release, test, sep)

#define MODULE_FILE_DESC			"x14 AutoLogin plugin\0"
#define MODULE_COMPANY_NAME			"North Edge\0"
#define MODULE_COPYRIGHT			"2011-2012 (c) North Edge.  All rights reserved.\0"
#define MODULE_PRODUCT_NAME			"Windower for Final Fantasy XIV\0"

#define MODULE_FILENAME				"AutoLogin.dll\0"

#define MODULE_MAJOR_VERSION		1
#define MODULE_MINOR_VERSION		0
#define MODULE_RELEASE_VERSION		0
#define MODULE_TEST_VERSION			0

#define PLUGIN_VERSION MAKE_VER_W(MODULE_MAJOR_VERSION, MODULE_MINOR_VERSION, MODULE_RELEASE_VERSION, MODULE_TEST_VERSION, ".")
#define MODULE_PRODUCT_VERSION MAKE_VER(MODULE_MAJOR_VERSION, MODULE_MINOR_VERSION, MODULE_RELEASE_VERSION, MODULE_TEST_VERSION, ".")
#define MODULE_PRODUCT_VER_STR MAKE_VER(MODULE_MAJOR_VERSION, MODULE_MINOR_VERSION, MODULE_RELEASE_VERSION, MODULE_TEST_VERSION, ", ")
#define MODULE_FILE_VER_STR MODULE_PRODUCT_VER_STR

#endif//__MODULE_VERSION_INFO_H__
