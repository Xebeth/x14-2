#include "stdafx.h"
#include <d3dx9.h>
#include <d3d9.h>

#include "IRenderable.h"

#include "IDirect3D9Wrapper.h"
#include "IDirect3DDevice9Wrapper.h"

IDirect3DDevice9Wrapper::IDirect3DDevice9Wrapper(LPDIRECT3DDEVICE9 *pDirect3dDevice,
												 D3DPRESENT_PARAMETERS &PresentParams_in)
	: m_pDirect3dDevice(*pDirect3dDevice), m_PresentParams(PresentParams_in), m_bRender(true), 
	  m_Fullscreen(false), m_DrawUi(false), m_bSceneStarted(false) {}

IDirect3DDevice9Wrapper::~IDirect3DDevice9Wrapper()
{
	m_pDirect3dDevice = NULL;
}

bool IDirect3DDevice9Wrapper::AddRenderable(unsigned long ID_in, IRenderable *pRenderable_in)
{
	if (pRenderable_in != NULL)
	{
		m_UiElements[ID_in] = pRenderable_in;

		return true;
	}

	return false;
}

bool IDirect3DDevice9Wrapper::RemoveRenderable(unsigned long ID_in)
{
	RenderableMap::const_iterator RenderableIt = m_UiElements.find(ID_in);

	if (RenderableIt != m_UiElements.end())
	{
		m_UiElements.erase(RenderableIt);

		return true;
	}

	return false;
}

ULONG __stdcall IDirect3DDevice9Wrapper::Release(void)
{
	return m_pDirect3dDevice->Release();
}

HRESULT __stdcall IDirect3DDevice9Wrapper::BeginScene() 
{
	HRESULT Result = S_OK;

	if (m_bRender && m_bSceneStarted == false)
	{
		Result = m_pDirect3dDevice->BeginScene();
		m_bSceneStarted = true;
		m_DrawUi = !m_DrawUi;

		if (m_DrawUi)
		{
			RenderableMap::const_iterator RenderableIt = m_UiElements.cbegin();
			RenderableMap::const_iterator EndIt = m_UiElements.cend();
			IRenderable *pRenderable = NULL;

			for(; RenderableIt != EndIt; ++RenderableIt)
			{
				pRenderable = RenderableIt->second;
				pRenderable->Update();
				pRenderable->Draw();
			}
		}
	}

	return Result;
}

HRESULT __stdcall IDirect3DDevice9Wrapper::EndScene() 
{
	HRESULT Result = S_OK;

	if (m_bRender && m_bSceneStarted)
	{
		Result = m_pDirect3dDevice->EndScene();
		m_bSceneStarted = false;
	}
	
	return Result;
}

HRESULT __stdcall IDirect3DDevice9Wrapper::SetRenderState(D3DRENDERSTATETYPE State, DWORD Value)
{
	m_DrawUi = m_DrawUi ? true : (State == D3DRS_SCISSORTESTENABLE && Value == 0);

	return m_pDirect3dDevice->SetRenderState(State, Value);
}

HRESULT __stdcall IDirect3DDevice9Wrapper::Reset(D3DPRESENT_PARAMETERS* pPresentationParameters) 
{
	// force vertical sync
	pPresentationParameters->PresentationInterval = m_PresentParams.PresentationInterval;
	pPresentationParameters->BackBufferCount = m_PresentParams.BackBufferCount;
	pPresentationParameters->SwapEffect = m_PresentParams.SwapEffect;

	RenderableMap::const_iterator RenderableIt = m_UiElements.cbegin();
	RenderableMap::const_iterator EndIt = m_UiElements.cend();

	for(; RenderableIt != EndIt; ++RenderableIt)
		RenderableIt->second->OnDeviceLost();

	HRESULT Result = m_pDirect3dDevice->Reset(pPresentationParameters);

	m_Fullscreen = (pPresentationParameters->Windowed == FALSE);
	m_bRender = (Result == D3D_OK);

	if (m_bRender)
	{
		RenderableIt = m_UiElements.cbegin();

		for(; RenderableIt != EndIt; ++RenderableIt)
			RenderableIt->second->OnDeviceReset();
	}

	return Result;
}

HRESULT __stdcall IDirect3DDevice9Wrapper::QueryInterface(REFIID iid, void ** ppvObject)
{
	return m_pDirect3dDevice->QueryInterface(iid, ppvObject);
}

ULONG	__stdcall IDirect3DDevice9Wrapper::AddRef(void)
{
	return m_pDirect3dDevice->AddRef();
}

HRESULT __stdcall IDirect3DDevice9Wrapper::GetDirect3D(IDirect3D9** ppD3D9)
{
	return m_pDirect3dDevice->GetDirect3D(ppD3D9);
}

HRESULT __stdcall IDirect3DDevice9Wrapper::TestCooperativeLevel()
{
	return m_pDirect3dDevice->TestCooperativeLevel();
}

UINT __stdcall IDirect3DDevice9Wrapper:: GetAvailableTextureMem() 
{
	return m_pDirect3dDevice->GetAvailableTextureMem();
}

HRESULT __stdcall IDirect3DDevice9Wrapper::EvictManagedResources()
{
	return m_pDirect3dDevice->EvictManagedResources();
}

HRESULT __stdcall IDirect3DDevice9Wrapper::GetDeviceCaps(D3DCAPS9* pCaps)
{
	return m_pDirect3dDevice->GetDeviceCaps(pCaps);
}

HRESULT __stdcall IDirect3DDevice9Wrapper::GetDisplayMode(UINT iSwapChain, D3DDISPLAYMODE* pMode) 
{
	return m_pDirect3dDevice->GetDisplayMode(iSwapChain, pMode); 
}

HRESULT __stdcall IDirect3DDevice9Wrapper::GetCreationParameters(D3DDEVICE_CREATION_PARAMETERS *pParameters) 
{
	return m_pDirect3dDevice->GetCreationParameters(pParameters); 
}

HRESULT __stdcall IDirect3DDevice9Wrapper::SetCursorProperties(UINT XHotSpot, UINT YHotSpot, IDirect3DSurface9* pCursorBitmap)
{
	return m_pDirect3dDevice->SetCursorProperties(XHotSpot, YHotSpot, pCursorBitmap);
}

void __stdcall IDirect3DDevice9Wrapper::SetCursorPosition(int X, int Y, DWORD Flags)
{
	m_pDirect3dDevice->SetCursorPosition(X, Y, Flags);
}

BOOL __stdcall IDirect3DDevice9Wrapper::ShowCursor(BOOL bShow)
{
	return m_pDirect3dDevice->ShowCursor(bShow);
}

HRESULT __stdcall IDirect3DDevice9Wrapper::CreateAdditionalSwapChain(D3DPRESENT_PARAMETERS* pPresentationParameters, IDirect3DSwapChain9** ppSwapChain) 
{
	// force vertical sync
	pPresentationParameters->PresentationInterval = m_PresentParams.PresentationInterval;
	pPresentationParameters->BackBufferCount = m_PresentParams.BackBufferCount;
	pPresentationParameters->SwapEffect = m_PresentParams.SwapEffect;

	return m_pDirect3dDevice->CreateAdditionalSwapChain(pPresentationParameters, ppSwapChain);
}

HRESULT __stdcall IDirect3DDevice9Wrapper::GetSwapChain(UINT iSwapChain, IDirect3DSwapChain9** ppSwapChain)
{
	return m_pDirect3dDevice->GetSwapChain(iSwapChain, ppSwapChain);
}

UINT __stdcall IDirect3DDevice9Wrapper::GetNumberOfSwapChains()
{
	return m_pDirect3dDevice->GetNumberOfSwapChains();
}

HRESULT __stdcall IDirect3DDevice9Wrapper::Present(CONST RECT* pSourceRect, CONST RECT* pDestRect, HWND hDestWindowOverride, CONST RGNDATA* pDirtyRegion) 
{
	return m_bRender ? m_pDirect3dDevice->Present(pSourceRect, pDestRect, hDestWindowOverride, pDirtyRegion) : S_OK;
}

HRESULT __stdcall IDirect3DDevice9Wrapper::GetBackBuffer(UINT iSwapChain, UINT iBackBuffer, D3DBACKBUFFER_TYPE Type, IDirect3DSurface9** ppBackBuffer) 
{
	return m_pDirect3dDevice->GetBackBuffer(iSwapChain, iBackBuffer, Type, ppBackBuffer);
}

HRESULT __stdcall IDirect3DDevice9Wrapper::GetRasterStatus(UINT iSwapChain, D3DRASTER_STATUS* pRasterStatus) 
{
	return m_pDirect3dDevice->GetRasterStatus(iSwapChain, pRasterStatus); 
}

HRESULT __stdcall IDirect3DDevice9Wrapper::SetDialogBoxMode(BOOL bEnableDialogs) 
{
	return m_pDirect3dDevice->SetDialogBoxMode(bEnableDialogs); 
}

void __stdcall IDirect3DDevice9Wrapper::SetGammaRamp(UINT iSwapChain, DWORD Flags, CONST D3DGAMMARAMP* pRamp) 
{
	m_pDirect3dDevice->SetGammaRamp(iSwapChain, Flags, pRamp); 
}

void __stdcall IDirect3DDevice9Wrapper::GetGammaRamp(UINT iSwapChain, D3DGAMMARAMP* pRamp) 
{
	m_pDirect3dDevice->GetGammaRamp(iSwapChain, pRamp); 
}

HRESULT __stdcall IDirect3DDevice9Wrapper::CreateTexture(UINT Width, UINT Height, UINT Levels, DWORD Usage, D3DFORMAT Format, D3DPOOL Pool, IDirect3DTexture9** ppTexture, HANDLE* pSharedHandle)
{
	return m_pDirect3dDevice->CreateTexture(Width, Height, Levels, Usage, Format, Pool, ppTexture , pSharedHandle);
}

HRESULT __stdcall IDirect3DDevice9Wrapper::CreateVolumeTexture(UINT Width, UINT Height, UINT Depth, UINT Levels, DWORD Usage, D3DFORMAT Format, D3DPOOL Pool, IDirect3DVolumeTexture9** ppVolumeTexture, HANDLE* pSharedHandle)
{
	return m_pDirect3dDevice->CreateVolumeTexture(Width, Height, Depth, Levels, Usage, Format, Pool, ppVolumeTexture , pSharedHandle);
}

HRESULT __stdcall IDirect3DDevice9Wrapper::CreateCubeTexture(UINT EdgeLength, UINT Levels, DWORD Usage, D3DFORMAT Format, D3DPOOL Pool, IDirect3DCubeTexture9** ppCubeTexture, HANDLE* pSharedHandle) 
{
	return m_pDirect3dDevice->CreateCubeTexture(EdgeLength, Levels, Usage, Format, Pool, ppCubeTexture , pSharedHandle);
}

HRESULT __stdcall IDirect3DDevice9Wrapper::CreateVertexBuffer(UINT Length, DWORD Usage, DWORD FVF, D3DPOOL Pool, IDirect3DVertexBuffer9** ppVertexBuffer, HANDLE* pSharedHandle) 
{
	return m_pDirect3dDevice->CreateVertexBuffer(Length, Usage, FVF, Pool, ppVertexBuffer , pSharedHandle);
}

HRESULT __stdcall IDirect3DDevice9Wrapper::CreateIndexBuffer(UINT Length, DWORD Usage, D3DFORMAT Format, D3DPOOL Pool, IDirect3DIndexBuffer9** ppIndexBuffer, HANDLE* pSharedHandle) 
{
	return m_pDirect3dDevice->CreateIndexBuffer(Length, Usage, Format, Pool, ppIndexBuffer , pSharedHandle);
}

HRESULT __stdcall IDirect3DDevice9Wrapper::CreateRenderTarget(UINT Width, UINT Height, D3DFORMAT Format, D3DMULTISAMPLE_TYPE MultiSample, DWORD MultisampleQuality, BOOL Lockable, IDirect3DSurface9** ppSurface, HANDLE* pSharedHandle) 
{
	return m_pDirect3dDevice->CreateRenderTarget(Width, Height, Format, MultiSample, MultisampleQuality, Lockable, ppSurface , pSharedHandle) ;
}

HRESULT __stdcall IDirect3DDevice9Wrapper::CreateDepthStencilSurface(UINT Width, UINT Height, D3DFORMAT Format, D3DMULTISAMPLE_TYPE MultiSample, DWORD MultisampleQuality, BOOL Discard, IDirect3DSurface9** ppSurface, HANDLE* pSharedHandle) 
{
	return m_pDirect3dDevice->CreateDepthStencilSurface(Width, Height, Format, MultiSample, MultisampleQuality, Discard, ppSurface , pSharedHandle);
}

HRESULT __stdcall IDirect3DDevice9Wrapper::UpdateSurface(IDirect3DSurface9* pSourceSurface, CONST RECT* pSourceRect, IDirect3DSurface9* pDestinationSurface, CONST POINT* pDestPoint) 
{
	return m_pDirect3dDevice->UpdateSurface(pSourceSurface , pSourceRect, pDestinationSurface, pDestPoint);
}																	

HRESULT __stdcall IDirect3DDevice9Wrapper::UpdateTexture(IDirect3DBaseTexture9* pSourceTexture, IDirect3DBaseTexture9* pDestinationTexture) 
{
	return m_pDirect3dDevice->UpdateTexture(pSourceTexture , pDestinationTexture); 
}

HRESULT __stdcall IDirect3DDevice9Wrapper::GetRenderTargetData(IDirect3DSurface9* pRenderTarget, IDirect3DSurface9* pDestSurface) 
{
	return m_pDirect3dDevice->GetRenderTargetData(pRenderTarget, pDestSurface) ;
}

HRESULT __stdcall IDirect3DDevice9Wrapper::GetFrontBufferData(UINT iSwapChain, IDirect3DSurface9* pDestSurface) 
{
	return 	m_pDirect3dDevice->GetFrontBufferData(iSwapChain, pDestSurface); 
}

HRESULT __stdcall IDirect3DDevice9Wrapper::StretchRect(IDirect3DSurface9* pSourceSurface, CONST RECT* pSourceRect, IDirect3DSurface9* pDestSurface, CONST RECT* pDestRect, D3DTEXTUREFILTERTYPE Filter)
{
	return m_pDirect3dDevice->StretchRect(pSourceSurface, pSourceRect, pDestSurface, pDestRect, Filter);
}

HRESULT __stdcall IDirect3DDevice9Wrapper::ColorFill(IDirect3DSurface9* pSurface, CONST RECT* pRect, D3DCOLOR color)
{
	return m_pDirect3dDevice->ColorFill(pSurface, pRect, color);
}

HRESULT __stdcall IDirect3DDevice9Wrapper::CreateOffscreenPlainSurface(UINT Width, UINT Height, D3DFORMAT Format, D3DPOOL Pool, IDirect3DSurface9** ppSurface, HANDLE* pSharedHandle) 
{
	return m_pDirect3dDevice->CreateOffscreenPlainSurface(Width, Height, Format, Pool, ppSurface, pSharedHandle);
}

HRESULT __stdcall IDirect3DDevice9Wrapper::SetRenderTarget(DWORD RenderTargetIndex, IDirect3DSurface9* pRenderTarget)
{
	return m_pDirect3dDevice->SetRenderTarget(RenderTargetIndex, pRenderTarget);
}

HRESULT __stdcall IDirect3DDevice9Wrapper::GetRenderTarget(DWORD RenderTargetIndex, IDirect3DSurface9** ppRenderTarget)
{
	return m_pDirect3dDevice->GetRenderTarget(RenderTargetIndex, ppRenderTarget);
}

HRESULT __stdcall IDirect3DDevice9Wrapper::SetDepthStencilSurface(IDirect3DSurface9* pNewZStencil) 
{	
	return m_pDirect3dDevice->SetDepthStencilSurface(pNewZStencil);
}

HRESULT __stdcall IDirect3DDevice9Wrapper::GetDepthStencilSurface(IDirect3DSurface9** ppZStencilSurface)
{
	return m_pDirect3dDevice->GetDepthStencilSurface(ppZStencilSurface);
}

HRESULT __stdcall IDirect3DDevice9Wrapper::Clear(DWORD Count, CONST D3DRECT* pRects, DWORD Flags, D3DCOLOR Color, float Z, DWORD Stencil)
{
	return m_pDirect3dDevice->Clear(Count, pRects, Flags, Color, Z, Stencil);
}

HRESULT __stdcall IDirect3DDevice9Wrapper::SetTransform(D3DTRANSFORMSTATETYPE State, CONST D3DMATRIX* pMatrix) 
{
	return m_pDirect3dDevice->SetTransform(State, pMatrix); 
}

HRESULT __stdcall IDirect3DDevice9Wrapper::GetTransform(D3DTRANSFORMSTATETYPE State, D3DMATRIX* pMatrix) 
{
	return m_pDirect3dDevice->GetTransform(State, pMatrix);
}

HRESULT __stdcall IDirect3DDevice9Wrapper::MultiplyTransform(D3DTRANSFORMSTATETYPE state, CONST D3DMATRIX* pMatrix) 
{
	return m_pDirect3dDevice->MultiplyTransform(state, pMatrix); 
}

HRESULT __stdcall IDirect3DDevice9Wrapper::SetViewport(CONST D3DVIEWPORT9* pViewport)
{
	return m_pDirect3dDevice->SetViewport(pViewport);
}

HRESULT __stdcall IDirect3DDevice9Wrapper::GetViewport(D3DVIEWPORT9* pViewport)
{
	return m_pDirect3dDevice->GetViewport(pViewport);
}

HRESULT __stdcall IDirect3DDevice9Wrapper::SetMaterial(CONST D3DMATERIAL9* pMaterial)
{
	return m_pDirect3dDevice->SetMaterial(pMaterial);
}

HRESULT __stdcall IDirect3DDevice9Wrapper::GetMaterial(D3DMATERIAL9* pMaterial) 
{
	return m_pDirect3dDevice->GetMaterial(pMaterial);
}

HRESULT __stdcall IDirect3DDevice9Wrapper::SetLight(DWORD Index, CONST D3DLIGHT9* pLight) 
{
	return m_pDirect3dDevice->SetLight(Index, pLight); 
}

HRESULT __stdcall IDirect3DDevice9Wrapper::GetLight(DWORD Index, D3DLIGHT9* pLight) 
{
	return m_pDirect3dDevice->GetLight(Index, pLight); 
}

HRESULT __stdcall IDirect3DDevice9Wrapper::LightEnable(DWORD Index, BOOL Enable) 
{
	return m_pDirect3dDevice->LightEnable(Index, Enable); 
}

HRESULT __stdcall IDirect3DDevice9Wrapper::GetLightEnable(DWORD Index, BOOL* pEnable) 
{
	return m_pDirect3dDevice->GetLightEnable(Index, pEnable); 
}

HRESULT __stdcall IDirect3DDevice9Wrapper::SetClipPlane(DWORD Index, CONST float* pPlane)
{
	return m_pDirect3dDevice->SetClipPlane(Index, pPlane);
}

HRESULT __stdcall IDirect3DDevice9Wrapper::GetClipPlane(DWORD Index, float* pPlane) 
{
	return m_pDirect3dDevice->GetClipPlane(Index, pPlane);
}

HRESULT __stdcall IDirect3DDevice9Wrapper::GetRenderState(D3DRENDERSTATETYPE State, DWORD* pValue) 
{
	return m_pDirect3dDevice->GetRenderState(State, pValue);
}

HRESULT __stdcall IDirect3DDevice9Wrapper::CreateStateBlock(D3DSTATEBLOCKTYPE Type, IDirect3DStateBlock9** ppSB)
{
	return m_pDirect3dDevice->CreateStateBlock(Type, ppSB);
}

HRESULT __stdcall IDirect3DDevice9Wrapper::BeginStateBlock()
{
	return m_pDirect3dDevice->BeginStateBlock();
}

HRESULT __stdcall IDirect3DDevice9Wrapper::EndStateBlock(IDirect3DStateBlock9** ppSB) 
{
	return m_pDirect3dDevice->EndStateBlock(ppSB);
}

HRESULT __stdcall IDirect3DDevice9Wrapper::SetClipStatus(CONST D3DCLIPSTATUS9* pClipStatus) 
{
	return m_pDirect3dDevice->SetClipStatus(pClipStatus); 
}

HRESULT __stdcall IDirect3DDevice9Wrapper::GetClipStatus(D3DCLIPSTATUS9* pClipStatus) 
{
	return m_pDirect3dDevice->GetClipStatus(pClipStatus); 
}

HRESULT __stdcall IDirect3DDevice9Wrapper::GetTexture(DWORD Stage, IDirect3DBaseTexture9** ppTexture) 
{
	return m_pDirect3dDevice->GetTexture(Stage, ppTexture);
}

HRESULT __stdcall IDirect3DDevice9Wrapper::SetTexture(DWORD Stage, IDirect3DBaseTexture9* pTexture)
{
	return m_pDirect3dDevice->SetTexture(Stage, pTexture);
}

HRESULT __stdcall IDirect3DDevice9Wrapper::GetTextureStageState(DWORD Stage, D3DTEXTURESTAGESTATETYPE Type, DWORD* pValue) 
{
	return m_pDirect3dDevice->GetTextureStageState(Stage, Type, pValue); 
}


HRESULT __stdcall IDirect3DDevice9Wrapper::SetTextureStageState(DWORD Stage, D3DTEXTURESTAGESTATETYPE Type, DWORD Value) 
{
	return m_pDirect3dDevice->SetTextureStageState(Stage, Type, Value);
}

HRESULT __stdcall IDirect3DDevice9Wrapper::GetSamplerState(DWORD Sampler, D3DSAMPLERSTATETYPE Type, DWORD* pValue) 
{
	return m_pDirect3dDevice->GetSamplerState(Sampler, Type, pValue); 
}

HRESULT __stdcall IDirect3DDevice9Wrapper::SetSamplerState(DWORD Sampler, D3DSAMPLERSTATETYPE Type, DWORD Value) 
{
	return m_pDirect3dDevice->SetSamplerState(Sampler, Type, Value); 
}

HRESULT __stdcall IDirect3DDevice9Wrapper::ValidateDevice(DWORD* pNumPasses) 
{
	return m_pDirect3dDevice->ValidateDevice(pNumPasses); 
}

HRESULT __stdcall IDirect3DDevice9Wrapper::SetPaletteEntries(UINT PaletteNumber, CONST PALETTEENTRY* pEntries) 
{
	return m_pDirect3dDevice->SetPaletteEntries(PaletteNumber, pEntries) ;
}

HRESULT __stdcall IDirect3DDevice9Wrapper::GetPaletteEntries(UINT PaletteNumber, PALETTEENTRY* pEntries) 
{
	return m_pDirect3dDevice->GetPaletteEntries(PaletteNumber, pEntries); 
}

HRESULT __stdcall IDirect3DDevice9Wrapper::SetCurrentTexturePalette(UINT PaletteNumber) 
{
	return m_pDirect3dDevice->SetCurrentTexturePalette(PaletteNumber); 
}

HRESULT __stdcall IDirect3DDevice9Wrapper::GetCurrentTexturePalette(UINT *PaletteNumber) 
{
	return m_pDirect3dDevice->GetCurrentTexturePalette(PaletteNumber); 
}

HRESULT __stdcall IDirect3DDevice9Wrapper::SetScissorRect(CONST RECT* pRect) 
{
	return m_pDirect3dDevice->SetScissorRect(pRect); 
}

HRESULT __stdcall IDirect3DDevice9Wrapper::GetScissorRect(RECT* pRect) 
{
	return m_pDirect3dDevice->GetScissorRect(pRect); 
}

HRESULT __stdcall IDirect3DDevice9Wrapper::SetSoftwareVertexProcessing(BOOL bSoftware) 
{
	return m_pDirect3dDevice->SetSoftwareVertexProcessing(bSoftware); 
}

BOOL __stdcall IDirect3DDevice9Wrapper::GetSoftwareVertexProcessing() 
{
	return m_pDirect3dDevice->GetSoftwareVertexProcessing();
}

HRESULT __stdcall IDirect3DDevice9Wrapper::SetNPatchMode(float nSegments) 
{
	return m_pDirect3dDevice->SetNPatchMode(nSegments); 
}

float __stdcall IDirect3DDevice9Wrapper::GetNPatchMode() 
{
	return m_pDirect3dDevice->GetNPatchMode() ;
}

HRESULT __stdcall IDirect3DDevice9Wrapper::DrawPrimitive(D3DPRIMITIVETYPE PrimitiveType, UINT StartVertex, UINT PrimitiveCount) 
{
	return m_pDirect3dDevice->DrawPrimitive(PrimitiveType, StartVertex, PrimitiveCount); 
}

HRESULT __stdcall IDirect3DDevice9Wrapper::DrawIndexedPrimitive(D3DPRIMITIVETYPE Type, INT BaseVertexIndex, UINT MinVertexIndex, UINT NumVertices, UINT startIndex, UINT primCount) 
{
	if (m_bRender)
		return m_pDirect3dDevice->DrawIndexedPrimitive(Type, BaseVertexIndex, MinVertexIndex, NumVertices, startIndex, primCount); 
	else
		return S_OK;
}

HRESULT __stdcall IDirect3DDevice9Wrapper::DrawPrimitiveUP(D3DPRIMITIVETYPE PrimitiveType, UINT PrimitiveCount, CONST void* pVertexStreamZeroData, UINT VertexStreamZeroStride) 
{
	if (m_bRender)
		return m_pDirect3dDevice->DrawPrimitiveUP(PrimitiveType, PrimitiveCount, pVertexStreamZeroData, VertexStreamZeroStride); 
	else
		return S_OK;
}

HRESULT __stdcall IDirect3DDevice9Wrapper::DrawIndexedPrimitiveUP(D3DPRIMITIVETYPE PrimitiveType, UINT MinVertexIndex, UINT NumVertices, UINT PrimitiveCount, CONST void* pIndexData, D3DFORMAT IndexDataFormat, CONST void* pVertexStreamZeroData, UINT VertexStreamZeroStride) 
{
	return m_pDirect3dDevice->DrawIndexedPrimitiveUP(PrimitiveType, MinVertexIndex, NumVertices, PrimitiveCount, pIndexData, IndexDataFormat, pVertexStreamZeroData, VertexStreamZeroStride); 
}

HRESULT __stdcall IDirect3DDevice9Wrapper::ProcessVertices(UINT SrcStartIndex, UINT DestIndex, UINT VertexCount, IDirect3DVertexBuffer9* pDestBuffer, IDirect3DVertexDeclaration9* pVertexDecl, DWORD Flags) 
{
	return m_pDirect3dDevice->ProcessVertices(SrcStartIndex, DestIndex, VertexCount, pDestBuffer, pVertexDecl, Flags);
}

HRESULT __stdcall IDirect3DDevice9Wrapper::CreateVertexDeclaration(CONST D3DVERTEXELEMENT9* pVertexElements, IDirect3DVertexDeclaration9** ppDecl) 
{
	return m_pDirect3dDevice->CreateVertexDeclaration(pVertexElements, ppDecl);
}

HRESULT __stdcall IDirect3DDevice9Wrapper::SetVertexDeclaration(IDirect3DVertexDeclaration9* pDecl) 
{
	return m_pDirect3dDevice->SetVertexDeclaration(pDecl);
}

HRESULT __stdcall IDirect3DDevice9Wrapper::GetVertexDeclaration(IDirect3DVertexDeclaration9** ppDecl) 
{
	return m_pDirect3dDevice->GetVertexDeclaration(ppDecl);
}

HRESULT __stdcall IDirect3DDevice9Wrapper::SetFVF(DWORD FVF) 
{
	return m_pDirect3dDevice->SetFVF(FVF); 
}

HRESULT __stdcall IDirect3DDevice9Wrapper::GetFVF(DWORD* pFVF) 
{
	return m_pDirect3dDevice->GetFVF(pFVF); 
}

HRESULT __stdcall IDirect3DDevice9Wrapper::CreateVertexShader(CONST DWORD* pFunction, IDirect3DVertexShader9** ppShader) 
{
	return m_pDirect3dDevice->CreateVertexShader(pFunction, ppShader);
}

HRESULT __stdcall IDirect3DDevice9Wrapper::SetVertexShader(IDirect3DVertexShader9* pShader) 
{
	return m_pDirect3dDevice->SetVertexShader(pShader);
}

HRESULT __stdcall IDirect3DDevice9Wrapper::GetVertexShader(IDirect3DVertexShader9** ppShader) 
{
	return m_pDirect3dDevice->GetVertexShader(ppShader);
}

HRESULT __stdcall IDirect3DDevice9Wrapper::SetVertexShaderConstantF(UINT StartRegister, CONST float* pConstantData, UINT Vector4fCount) 
{
	return m_pDirect3dDevice->SetVertexShaderConstantF(StartRegister, pConstantData, Vector4fCount) ;
}

HRESULT __stdcall IDirect3DDevice9Wrapper::GetVertexShaderConstantF(UINT StartRegister, float* pConstantData, UINT Vector4fCount) 
{
	return m_pDirect3dDevice->GetVertexShaderConstantF(StartRegister, pConstantData, Vector4fCount); 
}

HRESULT __stdcall IDirect3DDevice9Wrapper::SetVertexShaderConstantI(UINT StartRegister, CONST int* pConstantData, UINT Vector4iCount) 
{
	return m_pDirect3dDevice->SetVertexShaderConstantI(StartRegister, pConstantData, Vector4iCount); 
}

HRESULT __stdcall IDirect3DDevice9Wrapper::GetVertexShaderConstantI(UINT StartRegister, int* pConstantData, UINT Vector4iCount) 
{
	return m_pDirect3dDevice->GetVertexShaderConstantI(StartRegister, pConstantData, Vector4iCount); 
}

HRESULT __stdcall IDirect3DDevice9Wrapper::SetVertexShaderConstantB(UINT StartRegister, CONST BOOL* pConstantData, UINT BoolCount) 
{
	return m_pDirect3dDevice->SetVertexShaderConstantB(StartRegister, pConstantData, BoolCount); 
}


HRESULT __stdcall IDirect3DDevice9Wrapper::GetVertexShaderConstantB(UINT StartRegister, BOOL* pConstantData, UINT BoolCount) 
{
	return m_pDirect3dDevice->GetVertexShaderConstantB(StartRegister, pConstantData, BoolCount); 
}


HRESULT __stdcall IDirect3DDevice9Wrapper::SetStreamSource(UINT StreamNumber, IDirect3DVertexBuffer9* pStreamData, UINT OffsetInBytes, UINT Stride) 
{
	return m_pDirect3dDevice->SetStreamSource(StreamNumber, pStreamData , OffsetInBytes, Stride);
}


HRESULT __stdcall IDirect3DDevice9Wrapper::GetStreamSource(UINT StreamNumber, IDirect3DVertexBuffer9** ppStreamData, UINT* OffsetInBytes, UINT* pStride) 
{
	return m_pDirect3dDevice->GetStreamSource(StreamNumber, ppStreamData, OffsetInBytes, pStride);
}


HRESULT __stdcall IDirect3DDevice9Wrapper::SetStreamSourceFreq(UINT StreamNumber, UINT Divider) 
{
	return m_pDirect3dDevice->SetStreamSourceFreq(StreamNumber, Divider); 
}

HRESULT __stdcall IDirect3DDevice9Wrapper::GetStreamSourceFreq(UINT StreamNumber, UINT* Divider) 
{
	return m_pDirect3dDevice->GetStreamSourceFreq(StreamNumber, Divider); 
}

HRESULT __stdcall IDirect3DDevice9Wrapper::SetIndices(IDirect3DIndexBuffer9* pIndexData) 
{
	return m_pDirect3dDevice->SetIndices(pIndexData);
}


HRESULT __stdcall IDirect3DDevice9Wrapper::GetIndices(IDirect3DIndexBuffer9** ppIndexData) 
{
	return m_pDirect3dDevice->GetIndices(ppIndexData);
}


HRESULT __stdcall IDirect3DDevice9Wrapper::CreatePixelShader(CONST DWORD* pFunction, IDirect3DPixelShader9** ppShader) 
{
	return m_pDirect3dDevice->CreatePixelShader(pFunction, ppShader);
}


HRESULT __stdcall IDirect3DDevice9Wrapper::SetPixelShader(IDirect3DPixelShader9* pShader) 
{
	return m_pDirect3dDevice->SetPixelShader(pShader);
}


HRESULT __stdcall IDirect3DDevice9Wrapper::GetPixelShader(IDirect3DPixelShader9** ppShader) 
{
	return m_pDirect3dDevice->GetPixelShader(ppShader);
}


HRESULT __stdcall IDirect3DDevice9Wrapper::SetPixelShaderConstantF(UINT StartRegister, CONST float* pConstantData, UINT Vector4fCount) 
{
	return m_pDirect3dDevice->SetPixelShaderConstantF(StartRegister, pConstantData, Vector4fCount); 
}


HRESULT __stdcall IDirect3DDevice9Wrapper::GetPixelShaderConstantF(UINT StartRegister, float* pConstantData, UINT Vector4fCount) 
{
	return m_pDirect3dDevice->GetPixelShaderConstantF(StartRegister, pConstantData, Vector4fCount); 
}

HRESULT __stdcall IDirect3DDevice9Wrapper::SetPixelShaderConstantI(UINT StartRegister, CONST int* pConstantData, UINT Vector4iCount) 
{
	return m_pDirect3dDevice->SetPixelShaderConstantI(StartRegister, pConstantData, Vector4iCount); 
}

HRESULT __stdcall IDirect3DDevice9Wrapper::GetPixelShaderConstantI(UINT StartRegister, int* pConstantData, UINT Vector4iCount) 
{
	return m_pDirect3dDevice->GetPixelShaderConstantI(StartRegister, pConstantData, Vector4iCount); 
}

HRESULT __stdcall IDirect3DDevice9Wrapper::SetPixelShaderConstantB(UINT StartRegister, CONST BOOL* pConstantData, UINT BoolCount) 
{
	return m_pDirect3dDevice->SetPixelShaderConstantB(StartRegister, pConstantData, BoolCount); 
}

HRESULT __stdcall IDirect3DDevice9Wrapper::GetPixelShaderConstantB(UINT StartRegister, BOOL* pConstantData, UINT BoolCount) 
{
	return m_pDirect3dDevice->GetPixelShaderConstantB(StartRegister, pConstantData, BoolCount); 
}

HRESULT __stdcall IDirect3DDevice9Wrapper::DrawRectPatch(UINT Handle, CONST float* pNumSegs, CONST D3DRECTPATCH_INFO* pRectPatchInfo) 
{
	return m_pDirect3dDevice->DrawRectPatch(Handle, pNumSegs, pRectPatchInfo); 
}

HRESULT __stdcall IDirect3DDevice9Wrapper::DrawTriPatch(UINT Handle, CONST float* pNumSegs, CONST D3DTRIPATCH_INFO* pTriPatchInfo) 
{
	return m_pDirect3dDevice->DrawTriPatch(Handle, pNumSegs, pTriPatchInfo); 
}

HRESULT __stdcall IDirect3DDevice9Wrapper::DeletePatch(UINT Handle) 
{
	return m_pDirect3dDevice->DeletePatch(Handle); 
}

HRESULT __stdcall IDirect3DDevice9Wrapper::CreateQuery(D3DQUERYTYPE Type, IDirect3DQuery9** ppQuery)
{
	return m_pDirect3dDevice->CreateQuery(Type, ppQuery);
}

/*
HRESULT __stdcall IDirect3DDevice9Wrapper::SetConvolutionMonoKernel(UINT width,UINT height,float* rows,float* columns)
{
	return m_pDirect3dDevice->SetConvolutionMonoKernel(width, height, rows, columns);
}

HRESULT __stdcall IDirect3DDevice9Wrapper::ComposeRects(IDirect3DSurface9* pSrc,IDirect3DSurface9* pDst,IDirect3DVertexBuffer9* pSrcRectDescs,UINT NumRects,IDirect3DVertexBuffer9* pDstRectDescs,D3DCOMPOSERECTSOP Operation,int Xoffset,int Yoffset)
{
	return ComposeRects(pSrc, pDst, pSrcRectDescs, NumRects, pDstRectDescs, Operation, Xoffset, Yoffset);
}

HRESULT __stdcall IDirect3DDevice9Wrapper::PresentEx(CONST RECT* pSourceRect,CONST RECT* pDestRect,HWND hDestWindowOverride,CONST RGNDATA* pDirtyRegion,DWORD dwFlags)
{
	return PresentEx(pSourceRect, pDestRect, hDestWindowOverride, pDirtyRegion, dwFlags);
}

HRESULT __stdcall IDirect3DDevice9Wrapper::GetGPUThreadPriority(INT* pPriority)
{
	return GetGPUThreadPriority(pPriority);
}

HRESULT __stdcall IDirect3DDevice9Wrapper::SetGPUThreadPriority(INT Priority)
{
	return SetGPUThreadPriority(Priority);
}

HRESULT __stdcall IDirect3DDevice9Wrapper::WaitForVBlank(UINT iSwapChain)
{
	return WaitForVBlank(iSwapChain);
}

HRESULT __stdcall IDirect3DDevice9Wrapper::CheckResourceResidency(IDirect3DResource9** pResourceArray,UINT32 NumResources)
{
	return CheckResourceResidency(pResourceArray, NumResources);
}

HRESULT __stdcall IDirect3DDevice9Wrapper::SetMaximumFrameLatency(UINT MaxLatency)
{
	return SetMaximumFrameLatency(MaxLatency);
}

HRESULT __stdcall IDirect3DDevice9Wrapper::GetMaximumFrameLatency(UINT* pMaxLatency)
{
	return GetMaximumFrameLatency(pMaxLatency);
}

HRESULT __stdcall IDirect3DDevice9Wrapper::CheckDeviceState(HWND hDestinationWindow)
{
	return CheckDeviceState(hDestinationWindow);
}

HRESULT __stdcall IDirect3DDevice9Wrapper::CreateRenderTargetEx(UINT Width,UINT Height,D3DFORMAT Format,D3DMULTISAMPLE_TYPE MultiSample,DWORD MultisampleQuality,BOOL Lockable,IDirect3DSurface9** ppSurface,HANDLE* pSharedHandle,DWORD Usage)
{
	return CreateRenderTargetEx(Width, Height, Format, MultiSample, MultisampleQuality, Lockable, ppSurface, pSharedHandle, Usage);
}

HRESULT __stdcall IDirect3DDevice9Wrapper::CreateOffscreenPlainSurfaceEx(UINT Width,UINT Height,D3DFORMAT Format,D3DPOOL Pool,IDirect3DSurface9** ppSurface,HANDLE* pSharedHandle,DWORD Usage)
{
	return CreateOffscreenPlainSurfaceEx(Width, Height, Format, Pool, ppSurface, pSharedHandle, Usage);
}

HRESULT __stdcall IDirect3DDevice9Wrapper::CreateDepthStencilSurfaceEx(UINT Width,UINT Height,D3DFORMAT Format,D3DMULTISAMPLE_TYPE MultiSample,DWORD MultisampleQuality,BOOL Discard,IDirect3DSurface9** ppSurface,HANDLE* pSharedHandle,DWORD Usage)
{
	return CreateDepthStencilSurfaceEx(Width, Height, Format, MultiSample, MultisampleQuality, Discard, ppSurface, pSharedHandle, Usage);
}

HRESULT __stdcall IDirect3DDevice9Wrapper::ResetEx(D3DPRESENT_PARAMETERS* pPresentationParameters,D3DDISPLAYMODEEX *pFullscreenDisplayMode)
{
	return ResetEx(pPresentationParameters, pFullscreenDisplayMode);
}

HRESULT __stdcall IDirect3DDevice9Wrapper::GetDisplayModeEx(UINT iSwapChain,D3DDISPLAYMODEEX* pMode,D3DDISPLAYROTATION* pRotation)
{
	return GetDisplayModeEx(iSwapChain, pMode, pRotation);
}
*/