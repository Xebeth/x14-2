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
	typedef UIAL::CUiWindow<> UiWindow;
	typedef std::map<unsigned long, UiWindow*> WindowMap;
	typedef std::vector<IDirect3DDevice9Wrapper*> DeviceWrappers;

	class WindowerEngine;

	class GraphicsCore : public WindowerCore
	{
		enum eStaticLabels
		{
			GFX_TEXT_FPS = 0,	// text label for the FPS counter
			GFX_TEXT_COUNT		// number of static text labels
		};
	public:
		GraphicsCore(WindowerEngine &Engine_in_out, HookEngine &HookManager_in_out, bool VSync_in);

		void SetRendering(bool bEnable_in);
		void ToggleRendering();
		void ToggleFPS();

		// ICoreModule interface implementation
		void RegisterHooks(HookEngineLib::IHookManager &HookManager_in);
		void OnHookInstall(HookEngineLib::IHookManager &HookManager_in);

		// Direct3D
		IDirect3D9* Direct3DCreate9Hook(UINT SDKVersion_in);

	protected:
		//! function pointer to the original Direct3DCreate9 function
		fnDirect3DCreate9 m_pDirect3DCreate9Trampoline;
		//! Direct3D9 device wrapper
		IDirect3DDevice9Wrapper *m_pDeviceWrapper;
		//! Direct3D9 wrapper
		IDirect3D9Wrapper* m_pDirect3DWrapper;
		//! number of devices to skip
		unsigned int m_SkipDeviceCount;
		//! flag specifying if vertical synchronization is in use
		bool m_VSync;
		//! text label for the FPS display
	};
}

#endif//__GRAPHICS_CORE_H__