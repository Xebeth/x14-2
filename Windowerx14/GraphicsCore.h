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
	typedef std::vector<IDirect3DDevice9Wrapper*> DeviceWrappers;
	typedef std::vector<IDirect3D9Wrapper*> Direct3DWrappers;
	typedef HookEngineLib::IHookManager IHookManager;	

	class GraphicsCore : public WindowerCore
	{
	public:
		GraphicsCore(WindowerEngine &Engine_in_out, BOOL VSync_in);
		~GraphicsCore();

		void SetRendering(bool bEnable_in);
		void ToggleRendering();
		void ToggleWireframe();
		void ToggleFPS();

		// ICoreModule interface implementation
		void RegisterHooks(IHookManager &HookManager_in);
		void OnHookInstall(IHookManager &HookManager_in);

		// Direct3D
		IDirect3D9Wrapper* Direct3DCreate9Hook(UINT SDKVersion_in);
	protected:
		//! function pointer to the original Direct3DCreate9 function
		fnDirect3DCreate9 m_pDirect3DCreate9Trampoline;
		//! Direct3D9 device wrapper
		DeviceWrappers m_DeviceWrappers;
		//! Direct3D9 wrapper
		Direct3DWrappers m_Direct3DWrappers;
		//! flag specifying if vertical synchronization is in use
		BOOL m_VSync;
	};
}

#endif//__GRAPHICS_CORE_H__