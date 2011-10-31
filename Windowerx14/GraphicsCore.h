/**************************************************************************
	created		:	2010-10-10
	filename	: 	GraphicsCore.h
	author		:	xebeth`
	copyright	:	North Edge (2010)
	purpose		:	Interface with Direct3D 9
**************************************************************************/
#ifndef __GRAPHICS_CORE_H__
#define __GRAPHICS_CORE_H__

class IDirect3DDevice9Wrapper;
class IDirect3D9Wrapper;

namespace Windower
{
	typedef HookEngineLib::IHookManager IHookManager;

	class GraphicsCore : public WindowerCore
	{
	public:
		GraphicsCore(WindowerEngine &Engine_in_out, LONG ResX_in, LONG ResY_in, BOOL VSync_in, BOOL Direct3DEx_in);
		~GraphicsCore();

		void ToggleRendering();

		// events
		void OnCreateDevice(IDirect3DDevice9Wrapper *pDeviceWrapper_in);

		// ICoreModule interface implementation
		void RegisterHooks(IHookManager &HookManager_in);
		void OnHookInstall(IHookManager &HookManager_in);

		// Direct3D
		IDirect3D9Wrapper* Direct3DCreate9Hook(UINT SDKVersion_in);

	protected:
		//! function pointer to the original Direct3DCreate9 function
		fnDirect3DCreate9 m_pDirect3DCreate9Trampoline;
		//! Direct3D9 device wrapper
		IDirect3DDevice9Wrapper *m_pDeviceWrapper;
		//! Direct3D9 wrapper
		IDirect3D9Wrapper *m_pDirect3DWrapper;
		//! the width of the rendering surface
		LONG m_ResX;
		//! the height of the rendering surface
		LONG m_ResY;
		//! flag specifying if vertical synchronization is in use
		BOOL m_VSync;
		//! flag specifying if Direct3D9Ex should be used
		BOOL m_Direct3D9Ex;
	};
}

#endif//__GRAPHICS_CORE_H__