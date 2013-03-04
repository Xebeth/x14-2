/**************************************************************************
	created		:	2011-03-06
	filename	: 	stdafx.h
	author		:	Xebeth`
	copyright	:	North Edge (2011)
	purpose		:	Precompiled header
**************************************************************************/
#ifndef __INJECT_MODULE_AFX_H__
#define __INJECT_MODULE_AFX_H__

#define __USE_VLD
#include <stdhdr.h>

// Library DECL specification for exports
#define INJECT_MODULE_API __declspec(dllexport)

#endif//__INJECT_MODULE_AFX_H__