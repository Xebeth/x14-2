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
		GraphicsCore(WindowerEngine &Engine_in, LONG ResX, LONG ResY, BOOL VSync);
		~GraphicsCore();

		void ToggleRendering();

		// events
		void OnCreateDevice(IDirect3DDevice9Wrapper *pDeviceWrapper);

		// ICoreModule interface implementation
		void RegisterHooks(IHookManager *pHookManager);
		void OnHookInstall(IHookManager *pHookManager);

		// Direct3D
		IDirect3D9Wrapper* Direct3DCreate9Hook(UINT SDKVersion);

	protected:
		fnDirect3DCreate9		 m_pDirect3DCreate9Trampoline;

		IDirect3DDevice9Wrapper	*m_pDeviceWrapper;
		IDirect3D9Wrapper		*m_pDirect3DWrapper;

		// Device creation parameters
		const LONG m_ResX;
		const LONG m_ResY;
		const BOOL m_VSync;
	};
}

#endif//__GRAPHICS_CORE_H__