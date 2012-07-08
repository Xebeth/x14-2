/**************************************************************************
	created		:	2012-07-08
	filename	: 	ReadConfigHook.cpp
	author		:	Xebeth`
	copyright	:	North Edge (2012)
	purpose		:	Load configuration hook (config.sys)
**************************************************************************/
#include "stdafx.h"
#include <PluginFramework.h>
#include <HookEngine.h>

#include <CommandHandler.h>
#include "WindowerSettings.h"
#include "WindowerEngine.h"

#include "RegisterClassExHook.h"
#include "CreateWindowExHook.h"
#include "ReadConfigHook.h"

#include "ICoreModule.h"
#include "WindowerCore.h"
#include "SystemCore.h"

extern Windower::WindowerEngine *g_pEngine;

/*! \brief Reads the config.sys file
	\param[in] pConfigData_out : the buffer receiving the configuration data
	\return unknown value
*/
int __cdecl ReadConfigHook(BYTE *pConfigData_out)
{
	return g_pEngine->System().ReadConfigHook(pConfigData_out);
}