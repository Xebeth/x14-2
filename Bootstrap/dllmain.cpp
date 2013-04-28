/**************************************************************************
	created		:	2011-05-09
	filename	: 	dllmain.cpp
	author		:	Xebeth`
	copyright	:	North Edge (2011)
	purpose		:	DLL entry point
**************************************************************************/
#include "stdafx.h"

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
	if (::DetourIsHelperProcess())
		return TRUE;

	if (dwReason_in == DLL_PROCESS_ATTACH) 
	{
		// Restore the IAT table
		::DetourRestoreAfterWith();

		if (g_pEngine == NULL)
		{
#ifdef _DEBUG
			Sleep(5000);
#endif // _DEBUG
			g_pEngine = new Bootstrap::BootstrapEngine(hModule_in, _T("config.ini"));
		}
	}
	else if (dwReason_in == DLL_PROCESS_DETACH)
	{
		if (g_pEngine != NULL)
		{
			// cleanup
			delete g_pEngine;
			g_pEngine = NULL;
		}
	}

	return TRUE;
}