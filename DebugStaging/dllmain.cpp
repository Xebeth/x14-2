/**************************************************************************
	created		:	2011-05-09
	filename	: 	dllmain.cpp
	author		:	Xebeth`
	copyright	:	North Edge (2011)
	purpose		:	DLL entry point
**************************************************************************/
#include "stdafx.h"
#include "ReflectiveLoader.h"

// You can use this value as a pseudo hinstDLL value (defined and set via ReflectiveLoader.c)
HINSTANCE g_hAppInstance;

/*! \brief DLL entry point
	\param[in] hModule_in : a handle to the DLL module
	\param[in] dwReason_in : the reason code that indicates why the DLL entry-point function is being called
	\param[in] lpReserved_in : reserved
	\return TRUE if it succeeds or FALSE if initialization fails
*/
BOOL WINAPI DllMain(HINSTANCE hinstDLL, DWORD dwReason, LPVOID lpReserved)
{
	switch (dwReason)
	{
		case DLL_QUERY_HMODULE:
			if (lpReserved != NULL)
				*(HMODULE *)lpReserved = g_hAppInstance;
		break;
		case DLL_PROCESS_ATTACH:
			g_hAppInstance = hinstDLL;
		break;
	}

	return TRUE;
}

extern "C"
{
	DLLEXPORT BOOL DllInject(LPVOID lpUserdata, DWORD nUserdataLen)
	{
		return (lpUserdata != NULL && LoadLibraryW((LPCTSTR)lpUserdata) != NULL);
	}
}