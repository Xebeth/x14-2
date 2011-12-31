/**************************************************************************
	created		:	2011-06-03
	filename	: 	Direct3D9Hook.cpp
	author		:	Xebeth`
	copyright	:	North Edge (2011)
	purpose		:	Direct3D 9 hook definitions
**************************************************************************/
#include "stdafx.h"
#include <PluginFramework.h>
#include <HookEngine.h>
#include <d3dx9.h>
#include <d3d9.h>

#include "WindowerSettings.h"
#include "WindowerEngine.h"

#include "Direct3D9Hook.h"

#include "ICoreModule.h"
#include "WindowerCore.h"
#include "GraphicsCore.h"

#include "Font.h"
#include "IDirect3D9Wrapper.h"
#include "IDirect3DDevice9Wrapper.h"

extern Windower::WindowerEngine *g_pEngine;

/*! \brief Creates a Direct3D device given a DirectX SDK version
	\param[in] SDKVersion : the DirectX SDK version
	\return a pointer to the new device
*/
IDirect3D9* WINAPI Direct3DCreate9Hook(UINT SDKVersion_in)
{
	return g_pEngine->Graphics().Direct3DCreate9Hook(SDKVersion_in);
}