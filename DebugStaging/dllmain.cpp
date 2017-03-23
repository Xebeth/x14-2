/**************************************************************************
	created		:	2011-05-09
	filename	: 	dllmain.cpp
	author		:	Xebeth`
	copyright	:	North Edge (2011)
	purpose		:	DLL entry point
**************************************************************************/
#include "stdafx.h"

// You can use this value as a pseudo hinstDLL value (defined and set via ReflectiveLoader.c)
HINSTANCE g_hDllInstace;

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
			if (lpReserved != nullptr)
				*static_cast<HMODULE *>(lpReserved) = g_hDllInstace;
		break;
		case DLL_PROCESS_ATTACH:
			g_hDllInstace = hinstDLL;
#ifdef _DEBUG
			Sleep(5000);
#endif // _DEBUG
		break;
	}

	return TRUE;
}

extern "C"
{
	DLLEXPORT BOOL DllInject(LPVOID lpUserdata, DWORD nUserdataLen)
	{
		if (lpUserdata != nullptr)
		{
			auto pDLLPath = static_cast<LPCTSTR>(lpUserdata);

			if (LoadLibraryW(pDLLPath) == nullptr)
			{
				DWORD errCode = GetLastError();
				wchar_t buf[256];
				wchar_t err[8];

				_ltot_s(errCode, err, 10);

				FormatMessageW(FORMAT_MESSAGE_FROM_SYSTEM, nullptr, errCode,
							   MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), 
							   buf, 256, nullptr);

				MessageBox(nullptr, buf, pDLLPath, MB_OK);
			}
			else
				return TRUE;
		}

		return FALSE;
	}
}