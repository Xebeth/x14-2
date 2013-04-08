/**************************************************************************
	created		:	2011-05-09
	filename	: 	CreateProcessHook.cpp
	author		:	Xebeth`
	copyright	:	North Edge (2011)
	purpose		:	CreateProcess hook definitions
					Enables the injection of the windower DLLs
**************************************************************************/
#include "stdafx.h"

#include "BootstrapEngine.h"

#include "CreateProcessHook.h"
#include "CreateWindowExHook.h"

#include "SystemCore.h"

extern Bootstrap::BootstrapEngine *g_pEngine;

/*! \brief Creates a new process and its primary thread
	\param[in] lpApplicationName_in : the name of the module to be executed
	\param[in,out] lpCommandLine_in_out : the command line to be executed
	\param[in] lpProcessAttributes_in : pointer to a SECURITY_ATTRIBUTES structure
	\param[in] lpThreadAttributes_in : pointer to a SECURITY_ATTRIBUTES structure
	\param[in] bInheritHandles_in : flag specifying if each inheritable handle in the calling process is inherited by the new process
	\param[in] dwCreationFlags_in : the flags that control the priority class and the creation of the process
	\param[in] lpEnvironment_in : pointer to the environment block for the new process
	\param[in] lpCurrentDirectory_in : the full path to the current directory for the process
	\param[in] lpStartupInfo_in : pointer to a STARTUPINFO or STARTUPINFOEX structure
	\param[out] lpProcessInformation_out : pointer to a PROCESS_INFORMATION structure that receives identification information about the new process
	\return Non-zero if the process was created successfully; FALSE otherwise
*/
BOOL WINAPI CreateProcessHook(LPCTSTR lpApplicationName_in, LPTSTR lpCommandLine_in_out, LPSECURITY_ATTRIBUTES lpProcessAttributes_in,
							  LPSECURITY_ATTRIBUTES lpThreadAttributes_in, BOOL bInheritHandles_in, DWORD dwCreationFlags_in,
							  LPVOID lpEnvironment_in, LPCTSTR lpCurrentDirectory_in, LPSTARTUPINFO lpStartupInfo_in, 
							  LPPROCESS_INFORMATION lpProcessInformation_out)
{
	return g_pEngine->System().CreateProcessHook(lpApplicationName_in, lpCommandLine_in_out, lpProcessAttributes_in, lpThreadAttributes_in,
												 bInheritHandles_in, dwCreationFlags_in, lpEnvironment_in, lpCurrentDirectory_in,
												 lpStartupInfo_in, lpProcessInformation_out);
}