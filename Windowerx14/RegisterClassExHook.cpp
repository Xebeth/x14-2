/**************************************************************************
	created		:	2011-06-03
	filename	: 	RegisterClassExHook.cpp
	author		:	Xebeth`
	copyright	:	North Edge (2011)
	purpose		:	RegisterClassEx hook definitions
**************************************************************************/
#include "stdafx.h"
#include <PluginFramework.h>
#include <HookEngine.h>

#include <CommandHandler.h>
#include "WindowerSettings.h"
#include "WindowerEngine.h"

#include "RegisterClassExHook.h"
#include "CreateWindowExHook.h"
#include "ReadConfigHook.h"

#include "ICoreModule.h"
#include "WindowerCore.h"
#include "SystemCore.h"

extern Windower::WindowerEngine *g_pEngine;

/*! \brief Registers a window class
	\param[in] pWndClass_in : pointer to a WNDCLASSEX structure
	\return class atom that uniquely identifies the class being registered if successfuly; NULL otherwise
*/
ATOM WINAPI RegisterClassExWHook(const WNDCLASSEXW *pWndClass_in)
{
	return g_pEngine->System().RegisterClassExWHook(const_cast<WNDCLASSEXW*>(pWndClass_in));
}