/**************************************************************************
	created		:	2011-05-09
	filename	: 	CreateProcessHook.h
	author		:	Xebeth`
	copyright	:	North Edge (2011)
	purpose		:	CreateProcess hook definitions
**************************************************************************/
#ifndef __CREATE_PROCESS_HOOK_H__
#define __CREATE_PROCESS_HOOK_H__

#define TARGET_PROCESS_GAME _T("ffxivgame.exe")

// CreateProcessW hook
typedef BOOL (WINAPI *fnCreateProcess)(LPCTSTR lpApplicationName, LPTSTR lpCommandLine,
									   LPSECURITY_ATTRIBUTES lpProcessAttributes,
									   LPSECURITY_ATTRIBUTES lpThreadAttributes,
									   BOOL bInheritHandles, DWORD dwCreationFlags,
									   LPVOID lpEnvironment, LPCTSTR lpCurrentDirectory,
									   LPSTARTUPINFO lpStartupInfo, 
									   LPPROCESS_INFORMATION lpProcessInformation);

BOOL WINAPI CreateProcessHook(LPCTSTR lpApplicationName, LPTSTR lpCommandLine,
							  LPSECURITY_ATTRIBUTES lpProcessAttributes,
							  LPSECURITY_ATTRIBUTES lpThreadAttributes,
							  BOOL bInheritHandles, DWORD dwCreationFlags,
							  LPVOID lpEnvironment, LPCTSTR lpCurrentDirectory,
							  LPSTARTUPINFO lpStartupInfo,
							  LPPROCESS_INFORMATION lpProcessInformation);

#endif//__CREATE_PROCESS_HOOK_H__