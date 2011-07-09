/**************************************************************************
	created		:	2011-05-09
	filename	: 	launcher.cpp
	author		:	Xebeth`
	copyright	:	North Edge (2011)
	purpose		:	Windower x14 DLL injector
**************************************************************************/
#include "stdafx.h"
#include <tchar.h>
#include <InjectModule.h>

#ifdef _DEBUG
	#ifdef _M_X64
		#pragma comment(lib, "InjectModule.x64d.lib")
	#else
		#pragma comment(lib, "InjectModule.x86d.lib")
	#endif
#else
	#ifdef _M_X64
		#pragma comment(lib, "InjectModule.x64.lib")
	#else
		#pragma comment(lib, "InjectModule.x86.lib")
	#endif
#endif

// application entry point
int APIENTRY _tWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
					   LPTSTR lpCmdLine, int nCmdShow)
{
	PROCESS_INFORMATION ProcessInfo;
	TCHAR* pDLL32Path, *pExePath;
	BOOL Result;

#ifdef _DEBUG
	pExePath = _tcsdup(_T("E:\\Final Fantasy XIV\\ffxivboot.exe"));
	pDLL32Path = _tcsdup(_T("F:\\~dev\\cpp\\Windowerx14\\Debug\\bootstrap.dll"));
#else
	TCHAR DirPath [MAX_PATH];

	pDLL32Path = new TCHAR[MAX_PATH];
	pExePath = new TCHAR[MAX_PATH];

	GetCurrentDirectory(MAX_PATH, DirPath);
	_stprintf_s(pDLL32Path, MAX_PATH, _T("%s\\bootstrap.dll"), DirPath);
	_stprintf_s(pExePath, MAX_PATH, _T("%s\\ffxivboot.exe"), DirPath);
#endif

	Result = InjectModule::CreateProcessEx(pExePath, ProcessInfo, NULL, CREATE_DEFAULT_ERROR_MODE |
										   CREATE_NEW_CONSOLE | CREATE_UNICODE_ENVIRONMENT, pDLL32Path, NULL);
#ifdef _DEBUG
	delete pDLL32Path;
	delete pExePath;
#else
	delete[] pDLL32Path;
	delete[] pExePath;
#endif // _DEBUG

	return (Result != FALSE);
}