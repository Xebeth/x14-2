/**************************************************************************
	created		:	2011-06-03
	filename	: 	SetCursorPosHook.cpp
	author		:	Xebeth`
	copyright	:	North Edge (2011)
	purpose		:	SetCursorPos hook definitions
**************************************************************************/
#include "stdafx.h"

#include "WindowerEngine.h"

#include "SetCursorPosHook.h"
#include "CreateWindowExHook.h"
#include "RegisterClassExHook.h"

#include "WindowerCore.h"
#include "SystemCore.h"

extern Windower::WindowerEngine *g_pEngine;

BOOL WINAPI SetCursorPosAHook(INT X_in, INT Y_in)
{
	return g_pEngine->System().SetCursorPosAHook(X_in, Y_in);
}