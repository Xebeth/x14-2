/**************************************************************************
	created		:	2013-02-28
	filename	: 	UIAL.h
	author		:	Xebeth`
	copyright	:	North Edge (2013)
	purpose		:	Library main header
**************************************************************************/
#ifndef __UIAL_H__
#define __UIAL_H__

#include <StringUtils.h>

#define _UIAL_SDK_INCLUDE_

#include <CoordinateSystemPolicies.h>
#include <Point.h>
#include <Rect.h>

#include <UiFont.h>
#include <UiColor.h>
#include <UiTextElement.h>

#include <UiElement.h>
#include <UiWindow.h>
#include <IWindowRenderer.h>

#ifdef _DEBUG
	#pragma comment(lib, "UIAL.x86d.lib")
#else
	#pragma comment(lib, "UIAL.x86.lib")
#endif // _DEBUG

#undef _UIAL_SDK_INCLUDE_

#endif//__UIAL_H__