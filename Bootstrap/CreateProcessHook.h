/**************************************************************************
	created		:	2011-05-09
	filename	: 	CreateProcessHook.h
	author		:	Xebeth`
	copyright	:	North Edge (2011)
	purpose		:	CreateProcess hook definitions
**************************************************************************/
#ifndef __CREATE_PROCESS_HOOK_H__
#define __CREATE_PROCESS_HOOK_H__

#define TARGET_PROCESS_GAME_DX11	_T("ffxiv_dx11.exe")
#define TARGET_PROCESS_GAME			_T("ffxiv.exe")

#ifdef _DEBUG
	#define FUNCTION_HASH			0x9ccf0759	// DllInject
#else
	#define FUNCTION_HASH			0x34943165	// CreateEngine
#endif // _DEBUG

typedef BOOL (WINAPI *fnCreateProcess)(LPCTSTR lpApplicationName_in, LPTSTR lpCommandLine_in_out,
									   LPSECURITY_ATTRIBUTES lpProcessAttributes_in,
									   LPSECURITY_ATTRIBUTES lpThreadAttributes_in,
									   BOOL bInheritHandles_in, DWORD dwCreationFlags_in,
									   LPVOID lpEnvironment_in, LPCTSTR lpCurrentDirectory_in,
									   LPSTARTUPINFO lpStartupInfo_in, 
									   LPPROCESS_INFORMATION lpProcessInformation_out);
BOOL WINAPI CreateProcessHook(LPCTSTR lpApplicationName_in, LPTSTR lpCommandLine_in_out,
							  LPSECURITY_ATTRIBUTES lpProcessAttributes_in,
							  LPSECURITY_ATTRIBUTES lpThreadAttributes_in,
							  BOOL bInheritHandles_in, DWORD dwCreationFlags_in,
							  LPVOID lpEnvironment_in, LPCTSTR lpCurrentDirectory_in,
							  LPSTARTUPINFO lpStartupInfo_in,
							  LPPROCESS_INFORMATION lpProcessInformation_out);

#endif//__CREATE_PROCESS_HOOK_H__