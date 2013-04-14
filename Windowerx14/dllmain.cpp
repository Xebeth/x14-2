/**************************************************************************
	created		:	2011-06-02
	filename	: 	dllmain.cpp
	author		:	Xebeth`
	copyright	:	North Edge (2011)
	purpose		:	DLL entry point
**************************************************************************/
#include "stdafx.h"

#include "WindowerEngine.h"

Windower::WindowerEngine *g_pEngine = NULL;

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
			g_pEngine = new Windower::WindowerEngine(hModule_in, _T("config.ini"));
			g_pEngine->Attach();
		}
	}
	else if (dwReason_in == DLL_PROCESS_DETACH) 
	{
		if (g_pEngine != NULL)
		{
			g_pEngine->Detach();
			// cleanup
			delete g_pEngine;
			g_pEngine = NULL;
		}
	}

	return TRUE;
}