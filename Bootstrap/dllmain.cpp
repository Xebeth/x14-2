/**************************************************************************
	created		:	2011-05-09
	filename	: 	dllmain.cpp
	author		:	Xebeth`
	copyright	:	North Edge (2011)
	purpose		:	DLL entry point
**************************************************************************/
#include "stdafx.h"

#include "BootstrapEngine.h"

Bootstrap::BootstrapEngine *g_pEngine = nullptr;

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
#ifndef _M_X64
		// Restore the IAT table
		::DetourRestoreAfterWith();
#endif // _M_X64
		if (g_pEngine == nullptr)
		{
			TCHAR DirPath[_MAX_PATH] = { '\0' };
			string_t WorkingDir;

			// retrieve the name of the module
			if (GetModuleFileName(hModule_in, DirPath, _MAX_PATH) != 0UL)
				filepath(DirPath, WorkingDir);

			g_pEngine = new Bootstrap::BootstrapEngine(WorkingDir.c_str(), _T("config.ini"));
		}
	}
	else if (dwReason_in == DLL_PROCESS_DETACH)
	{
		if (g_pEngine != nullptr)
		{
			// cleanup
			delete g_pEngine;
			g_pEngine = nullptr;
		}
	}

	return TRUE;
}