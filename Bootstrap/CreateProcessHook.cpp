/**************************************************************************
	created		:	2011-05-09
	filename	: 	CreateProcessHook.cpp
	author		:	Xebeth`
	copyright	:	North Edge (2011)
	purpose		:	CreateProcess hook definitions
**************************************************************************/
#include "stdafx.h"
#include <PluginFramework.h>
#include <NonCopyable.h>
#include <HookEngine.h>

#include "BaseEngine.h"
#include "PluginEngine.h"
#include "BootstrapEngine.h"

#include "CreateProcessHook.h"
#include "ShellExecuteExHook.h"
#include "CreateWindowExHook.h"

#include "ICoreModule.h"
#include "WindowerCore.h"
#include "SystemCore.h"

extern Bootstrap::BootstrapEngine *g_pEngine;

// CreateProcessW hook to inject the bootstrap/windower DLLs
BOOL WINAPI CreateProcessHook(LPCTSTR lpApplicationName, LPTSTR lpCommandLine, LPSECURITY_ATTRIBUTES lpProcessAttributes,
							  LPSECURITY_ATTRIBUTES lpThreadAttributes, BOOL bInheritHandles, DWORD dwCreationFlags,
							  LPVOID lpEnvironment, LPCTSTR lpCurrentDirectory, LPSTARTUPINFO lpStartupInfo,
							  LPPROCESS_INFORMATION lpProcessInformation)
{
	return g_pEngine->System().CreateProcessHook(lpApplicationName, lpCommandLine, lpProcessAttributes, lpThreadAttributes,
												 bInheritHandles, dwCreationFlags, lpEnvironment, lpCurrentDirectory,
												 lpStartupInfo, lpProcessInformation);
}