/**************************************************************************
	created		:	2011-05-09
	filename	: 	dllmain.cpp
	author		:	Xebeth`
	copyright	:	North Edge (2011)
	purpose		:	DLL entry point
**************************************************************************/
#include "stdafx.h"
#include <PluginFramework.h>
#include <HookEngine.h>

#include "BaseEngine.h"
#include "PluginEngine.h"
#include "BootstrapEngine.h"

Bootstrap::BootstrapEngine *g_pEngine = NULL;

/*! \brief DLL entry point
	\param[in] hModule_in : a handle to the DLL module
	\param[in] dwReason_in : the reason code that indicates why the DLL entry-point function is being called
	\param[in] lpReserved_in : reserved
	\return TRUE if it succeeds or FALSE if initialization fails
*/
BOOL APIENTRY DllMain(HMODULE hModule_in, DWORD dwReason_in, LPVOID lpReserved_in)
{
	BOOL bResult = TRUE;

	if (dwReason_in == DLL_PROCESS_ATTACH) 
	{
		if (g_pEngine == NULL)
		{
			g_pEngine = new Bootstrap::BootstrapEngine(_T("config.ini"));
			bResult = g_pEngine->Attach();
		}
	}
	else if (dwReason_in == DLL_PROCESS_DETACH)
	{
		if (g_pEngine != NULL)
		{
			g_pEngine->Detach();

			delete g_pEngine;
			g_pEngine = NULL;
		}
	}

	return bResult;
}