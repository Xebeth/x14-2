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
	TCHAR DLL32Path[_MAX_PATH];
	TCHAR ExePath[_MAX_PATH];
	TCHAR DirPath[_MAX_PATH];
	BOOL Result;

	GetCurrentDirectory(_MAX_PATH, DirPath);
	_stprintf_s(DLL32Path, _MAX_PATH, _T("%s\\bootstrap.dll"), DirPath);
	_stprintf_s(ExePath, _MAX_PATH, _T("%s\\ffxivboot.exe"), DirPath);

	Result = InjectModule::CreateProcessEx(ExePath, ProcessInfo, NULL, CREATE_DEFAULT_ERROR_MODE |
										   CREATE_NEW_CONSOLE | CREATE_UNICODE_ENVIRONMENT, DLL32Path, NULL);

	return (Result != FALSE);
}