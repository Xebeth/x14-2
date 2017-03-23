/**************************************************************************
	created		:	2010-10-10
	filename	: 	GraphicsCore.h
	author		:	xebeth`
	copyright	:	North Edge (2010)
	purpose		:	Interface with Direct3D 9
**************************************************************************/
#ifndef __GRAPHICS_CORE_H__
#define __GRAPHICS_CORE_H__

class Direct3DDevice9WrapperImpl;
class Direct3D9WrapperImpl;
class IDirect3D9Wrapper;

#define GRAPHICS_MODULE			"Graphics"
#define TEXT_LABEL_SERVICE		"TextLabelService"
#define FPS_LABEL_NAME			_T("FPS##Label")
#define MACRO_LABEL_NAME		_T("Macro##Label")

typedef struct IDirect3D9 Direct3D9;
typedef struct IDirect3DDevice9 IDirect3DDevice9;

typedef IDirect3D9*	(WINAPI *fnDirect3DCreate9)(UINT SDKVersion_in);
typedef HRESULT (WINAPI *fnCreateDXGIFactory)(REFIID riid, void **ppFactory);
typedef HRESULT(WINAPI *fnCreateSwapChain)(IDXGIFactory *pFactory, IUnknown *pDevice, DXGI_SWAP_CHAIN_DESC *pDesc, IDXGISwapChain **ppSwapChain);

typedef struct ID3D11DeviceContext ID3D11DeviceContext;
typedef struct IDXGISwapChain IDXGISwapChain;
typedef struct ID3D11Device ID3D11Device;

class Timer;

namespace Windower
{
	class TextLabelRenderer;
	class WindowerEngine;
	class UiTextLabel;
	
	class GraphicsCore : public WindowerCore, public IEventInterface, public IDeviceCreateSubscriber, public ISwapChainCreateSubscriber
	{
		enum eStaticLabels
		{
			GFX_TEXT_FPS = 0,	// text label for the FPS counter
			GFX_TEXT_MACRO,		// text label for macros
			GFX_TEXT_COUNT		// number of static text labels
		};

	public:
		explicit GraphicsCore(bool VSync_in);
		~GraphicsCore();

		void Detach();

		BaseModuleService* CreateService(const string_t& ServiceName_in, bool InvokePermission_in = false);
		bool Invoke(const string_t& ServiceName_in, PluginFramework::ServiceParam &Params_in);

		void OnDXGIFactoryCreate(IDXGIFactory *pFactory_in);
		void OnDeviceCreate(IDirect3DDevice9 *pDevice_in, const D3DPRESENT_PARAMETERS *pPresentParams_in, HWND hWnd_in);
		void OnSwapChainCreate(IDXGISwapChain *pSwapChain, const DXGI_SWAP_CHAIN_DESC* pSwapChainDesc);

		
		LRESULT OnLButtonDown(WORD X_in, WORD Y_in, DWORD_PTR MouseFlags_in, UINT_PTR KeyFlags_in);
		LRESULT OnMouseMove(WORD X_in, WORD Y_in, DWORD_PTR MouseFlags_in, UINT_PTR KeyFlags_in);
		LRESULT OnLButtonUp(WORD X_in, WORD Y_in, DWORD_PTR MouseFlags_in, UINT_PTR KeyFlags_in);
		LRESULT OnKeyUp(UINT_PTR PressedChar_in, UINT RepetitionCount_in, UINT_PTR KeyFlags_in);
		LRESULT OnSize(int NewWidth_in, int NewHeight_in, UINT nFlags_in);
		LRESULT OnActivate(bool bActive_in, bool bMinimized_in);

		UiTextLabel* HitTest(WORD X_in, WORD Y_in);

		void ShowMacroProgress(unsigned long current, unsigned long total, bool visible);
		void SetRendering(bool bEnable_in);
		bool RegisterServices();
		void ToggleRendering();
		void ToggleFPS();

		// ICoreModule interface implementation
		void RegisterHooks(HookEngineLib::IHookManager &HookManager_in);
		void OnHookInstall(HookEngineLib::IHookManager &HookManager_in);
		
		// Direct3D

		static HRESULT WINAPI CreateSwapChainHook(IDXGIFactory *pFactory, IUnknown *pDevice, DXGI_SWAP_CHAIN_DESC *pDesc, IDXGISwapChain **ppSwapChain);
		static IDirect3D9* WINAPI Direct3DCreate9Hook(UINT SDKVersion_in);
		static HRESULT WINAPI CreateDXGIFactoryHook(REFIID riid, void **ppFactory);
		static HRESULT WINAPI D3D11CreateDeviceHook(IDXGIAdapter* pAdapter, D3D_DRIVER_TYPE DriverType, HMODULE Software,
													UINT Flags, CONST D3D_FEATURE_LEVEL* pFeatureLevels, UINT FeatureLevels,
													UINT SDKVersion, ID3D11Device** ppDevice, D3D_FEATURE_LEVEL* pFeatureLevel,
													ID3D11DeviceContext** ppImmediateContext);
	protected:
		TextLabelRenderer* GetLabelRenderer();
		void InitializeLabel(UiTextLabel * pLabel);
		void InitializeRenderer(IDirect3DDevice9 * pDevice_in);

		//! function pointer to the original Direct3DCreate9 function
		fnDirect3DCreate9 m_pDirect3DCreate9Trampoline;
		//! Direct3D device
		IDirect3DDevice9 *m_pDirect3DDevice;
		ID3D11Device *m_pD3D11Device;
		PFN_D3D11_CREATE_DEVICE m_pD3D11CreateDeviceTrampoline;
		fnCreateDXGIFactory m_pCreateDXGIFactoryTrampoline;
		fnCreateSwapChain m_pCreateSwapChainTrampoline;
		//! flag specifying if vertical synchronization is in use
		bool m_VSync;
		//! graphical elements
		RenderableMap m_UiElements;
		//! text label renderer
		TextLabelRenderer *m_pLabelRenderer;
		//! game window size
		WORD m_Width, m_Height;
		//! mouse position
		long m_MouseOffsetX, m_MouseOffsetY;
		DWORD m_LabelWidth, m_LabelHeight;
		//! pointer to the label being moved
		UiTextLabel *m_pMovingLabel;
		//! calling context for the service hooks
		static CallingContext<GraphicsCore> m_Context;
	};
}

#endif//__GRAPHICS_CORE_H__