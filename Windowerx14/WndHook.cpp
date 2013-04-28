/**************************************************************************
	created		:	2011-06-03
	filename	: 	WndHook.cpp
	author		:	Xebeth`
	copyright	:	North Edge (2011)
	purpose		:	CreateWindowEx hook definitions
**************************************************************************/
#include "stdafx.h"

#include "WindowerEngine.h"

#include "WndHook.h"

#include "WindowerCore.h"
#include "SystemCore.h"

extern Windower::WindowerEngine *g_pEngine;

/*! \brief Installs or updates a window subclass callback
	\param[in] hWnd_in : handle of the window being sub-classed
	\param[in] pfnSubclass_in : pointer to a window procedure
	\param[in] uIdSubclass_in : subclass ID
	\param[in] dwRefData_in : reference data (application specific)
	\return TRUE if the subclass callback was successfully installed; FALSE otherwise
*/
BOOL WINAPI SetWindowSubclassHook(HWND hWnd_in, SUBCLASSPROC pfnSubclass_in, UINT_PTR uIdSubclass_in, DWORD_PTR dwRefData_in)
{
	return g_pEngine->System().SetWindowSubclassHook(hWnd_in, pfnSubclass_in, uIdSubclass_in, dwRefData_in);
}