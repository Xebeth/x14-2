/**************************************************************************
	created		:	2011-06-02
	filename	: 	dllmain.cpp
	author		:	Xebeth`
	copyright	:	North Edge (2011)
	purpose		:	DLL entry point
**************************************************************************/
#include "stdafx.h"

#include "WindowerEngine.h"

// You can use this value as a pseudo hinstDLL value (defined and set via ReflectiveLoader.c)
HINSTANCE g_hDllInstace = nullptr;
HINSTANCE g_hAppInstance = nullptr;

Windower::WindowerEngine *g_pEngine = nullptr;

BOOL DestroyEngine()
{
	if (g_pEngine != nullptr)
	{
		// stop the engine thread
		g_pEngine->Detach();
		// cleanup
		delete g_pEngine;
		g_pEngine = nullptr;
	}

	return TRUE;
}

#ifndef _DEBUG
extern "C"
{
#endif
DLLEXPORT BOOL CreateEngine(LPVOID lpUserdata, DWORD nUserdataLen)
{
	if (g_pEngine == nullptr && lpUserdata != nullptr)
	{
		string_t WorkingDir;

#ifdef _DEBUG
		//Sleep(5000);
#endif // _DEBUG

		file_path((LPCTSTR)lpUserdata, WorkingDir);
		g_pEngine = new Windower::WindowerEngine(WorkingDir.c_str(), _T("config.ini"));
		g_pEngine->Attach();
	}

	return TRUE;
}
#ifndef _DEBUG
}
#endif

/*! \brief DLL entry point
	\param[in] hModule_in : a handle to the DLL module
	\param[in] dwReason_in : the reason code that indicates why the DLL entry-point function is being called
	\param[in] lpReserved_in : reserved
	\return TRUE if it succeeds or FALSE if initialization fails
*/
BOOL APIENTRY DllMain(HMODULE hModule_in, DWORD dwReason_in, LPVOID lpReserved_in)
{
#ifndef _M_X64
	if (::DetourIsHelperProcess())
		return TRUE;
#endif

	switch (dwReason_in)
	{
#ifndef _DEBUG
		case DLL_QUERY_HMODULE:
			if (lpReserved_in != nullptr)
				*(HMODULE *)lpReserved_in = g_hAppInstance;
		break;
#endif // _DEBUG
		case DLL_PROCESS_ATTACH:
		{
#ifndef _M_X64
			// Restore the IAT table
			::DetourRestoreAfterWith();	
#endif // _M_X64

#ifdef _DEBUG
			TCHAR DirPath[_MAX_PATH] = { '\0' };
			
			g_hDllInstace = hModule_in;
			g_hAppInstance = GetModuleHandle(nullptr);
			// retrieve the name of the module
			if (GetModuleFileName(hModule_in, DirPath, _MAX_PATH) != 0UL)
				return CreateEngine((LPVOID)DirPath, 0UL /* unused */);
#endif
		}
		break;
		case  DLL_PROCESS_DETACH:
			return DestroyEngine();
		break;
	}

	return TRUE;
}