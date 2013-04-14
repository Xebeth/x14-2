/**************************************************************************
	created		:	2011-06-04
	filename	: 	WndProcHook.cpp
	author		:	Xebeth`
	copyright	:	North Edge (2011)
	purpose		:	WndProc hook definitions
**************************************************************************/
#include "stdafx.h"

#include "WindowerEngine.h"

#include "RegisterClassExHook.h"
#include "CreateWindowExHook.h"
#include "SetCursorPosHook.h"
#include "WndProcHook.h"

#include "WindowerCore.h"
#include "SystemCore.h"

extern Windower::WindowerEngine *g_pEngine;

/*! \brief Processes messages sent to a window
	\param[in] hWnd_in : handle to the window
	\param[in] uMsg_in : message
	\param[in] wParam_in : additional message information
	\param[in] lParam_in : additional message information
	\return result of the message processing that depends on the message sent
*/
LRESULT WINAPI WndProcHook(HWND hWnd_in, UINT uMsg_in, WPARAM wParam_in, LPARAM lParam_in)
{
	return g_pEngine->System().WndProcHook(hWnd_in, uMsg_in, wParam_in, lParam_in);
}