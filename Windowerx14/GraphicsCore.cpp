/**************************************************************************
	created		:	2010-10-10
	filename	: 	GraphicsCore.cpp
	author		:	xebeth`
	copyright	:	North Edge (2010)
	purpose		:	Interface with Direct3D 9
**************************************************************************/
#include "stdafx.h"
#include <d3dx9.h>
#include <d3d9.h>

#include "WindowerSettings.h"
#include "WindowerEngine.h"

#include "Direct3D9Hook.h"

#include "WindowerCore.h"
#include "GraphicsCore.h"

#include "Font.h"
#include "IDirect3D9Wrapper.h"
#include "IDirect3DDevice9Wrapper.h"
#include "Direct3D9Hook.h"

using namespace UIAL;

namespace Windower
{
	/*! \brief GraphicsCore constructor
		\param[in,out] Engine_in_out : the windower engine
		\param[in] VSync_in : flag specifying if vertical synchronization is in use
	*/
	GraphicsCore::GraphicsCore(WindowerEngine &Engine_in_out, HookEngine &HookManager_in_out, bool VSync_in)
		: WindowerCore(_T("Graphics"), Engine_in_out, HookManager_in_out), m_VSync(VSync_in),
		  m_pDirect3DWrapper(NULL), m_pDirect3DCreate9Trampoline(NULL), m_SkipDeviceCount(1U) {}

	/*! \brief Creates a Direct3D device given a DirectX SDK version
		\param[in] SDKVersion_in : the DirectX SDK version
		\return a pointer to the new device
	*/
	IDirect3D9* GraphicsCore::Direct3DCreate9Hook(UINT SDKVersion_in)
	{
		IDirect3D9* pDirect3D = NULL;

		if (m_pDirect3DCreate9Trampoline != NULL)
		{
			pDirect3D = m_pDirect3DCreate9Trampoline(SDKVersion_in);

			if (pDirect3D != NULL && m_SkipDeviceCount == 0)
			{
				IDirect3D9Wrapper *pDirect3DWrapper = new IDirect3D9Wrapper(pDirect3D, m_VSync);

				m_pDirect3DWrapper = pDirect3DWrapper;
				// subscribe for a pointer to the Direct3DDevice wrapper
				pDirect3DWrapper->Subscribe(&m_pDeviceWrapper);

				return pDirect3DWrapper;
			}
			else
				--m_SkipDeviceCount;
		}

		return pDirect3D;
	}

	//! \brief Switches on/off the rendering added by the windower
	void GraphicsCore::ToggleRendering()
	{
		if (m_pDeviceWrapper != NULL)
			m_pDeviceWrapper->ToggleRendering();
	}

	void GraphicsCore::SetRendering(bool bEnable_in)
	{
		if (m_pDeviceWrapper != NULL)
			m_pDeviceWrapper->SetRendering(bEnable_in);
	}

	void GraphicsCore::ToggleFPS()
	{
		
	}

	/*! \brief Register the hooks for this module
		\param[in] HookManager_in : the hook manager
	*/
	void GraphicsCore::RegisterHooks(HookEngineLib::IHookManager &HookManager_in)
	{
		// register the Direct3DCreate9 hook
		HookManager_in.RegisterHook("Direct3DCreate9", "d3d9.dll", NULL, ::Direct3DCreate9Hook);
	}

	/*! \brief Callback invoked when the hooks of the module are installed
		\param[in] HookManager_in : the hook manager
	*/
	void GraphicsCore::OnHookInstall(HookEngineLib::IHookManager &HookManager_in)
	{
		m_pDirect3DCreate9Trampoline = (fnDirect3DCreate9)HookManager_in.GetTrampolineFunc("Direct3DCreate9");		
	}
}