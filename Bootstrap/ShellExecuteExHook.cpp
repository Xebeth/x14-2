/**************************************************************************
	created		:	2011-05-09
	filename	: 	ShellExecuteExHook.cpp
	author		:	Xebeth`
	copyright	:	North Edge (2011)
	purpose		:	ShellExecuteExW hook definitions
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

BOOL WINAPI ShellExecuteExHook(LPSHELLEXECUTEINFO lpExecInfo)
{
	return g_pEngine->System().ShellExecuteExHook(lpExecInfo);
}