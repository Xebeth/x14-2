/**************************************************************************
	created		:	2013-04-29
	filename	: 	Direct3DDevice9WrapperImpl.h
	author		:	Xebeth`
	copyright	:	North Edge (2013)
	purpose		:	
**************************************************************************/
#ifndef __DIRECT3DDEVICE9_WRAPPER_IMPL_H__
#define __DIRECT3DDEVICE9_WRAPPER_IMPL_H__

class IRenderable;

namespace D3DDevice9_vTable
{
	enum
	{
		iQueryInterface = 0UL,
		iAddRef,
		iRelease,

		iTestCooperativeLevel,
		iGetAvailableTextureMem,
		iEvictManagedResources,
		iGetDirect3D,
		iGetDeviceCaps,
		iGetDisplayMode,
		iGetCreationParameters,
		iSetCursorProperties,
		iSetCursorPosition,
		iShowCursor,
		iCreateAdditionalSwapChain,
		iGetSwapChain,
		iGetNumberOfSwapChains,
		iReset,
		iPresent,
		iGetBackBuffer,
		iGetRasterStatus,
		iSetDialogBoxMode,
		iSetGammaRamp,
		iGetGammaRamp,
		iCreateTexture,
		iCreateVolumeTexture,
		iCreateCubeTexture,
		iCreateVertexBuffer,
		iCreateIndexBuffer,
		iCreateRenderTarget,
		iCreateDepthStencilSurface,
		iUpdateSurface,
		iUpdateTexture,
		iGetRenderTargetData,
		iGetFrontBufferData,
		iStretchRect,
		iColorFill,
		iCreateOffscreenPlainSurface,
		iSetRenderTarget,
		iGetRenderTarget,
		iSetDepthStencilSurface,
		iGetDepthStencilSurface,
		iBeginScene,
		iEndScene,
		iClear,
		iSetTransform,
		iGetTransform,
		iMultiplyTransform,
		iSetViewport,
		iGetViewport,
		iSetMaterial,
		iGetMaterial,
		iSetLight,
		iGetLight,
		iLightEnable,
		iGetLightEnable,
		iSetClipPlane,
		iGetClipPlane,
		iSetRenderState,
		iGetRenderState,
		iCreateStateBlock,
		iBeginStateBlock,
		iEndStateBlock,
		iSetClipStatus,
		iGetClipStatus,
		iGetTexture,
		iSetTexture,
		iGetTextureStageState,
		iSetTextureStageState,
		iGetSamplerState,
		iSetSamplerState,
		iValidateDevice,
		iSetPaletteEntries,
		iGetPaletteEntries,
		iSetCurrentTexturePalette,
		iGetCurrentTexturePalette,
		iSetScissorRect,
		iGetScissorRect,
		iSetSoftwareVertexProcessing,
		iGetSoftwareVertexProcessing,
		iSetNPatchMode,
		iGetNPatchMode,
		iDrawPrimitive,
		iDrawIndexedPrimitive,
		iDrawPrimitiveUP,
		iDrawIndexedPrimitiveUP,
		iProcessVertices,
		iCreateVertexDeclaration,
		iSetVertexDeclaration,
		iGetVertexDeclaration,
		iSetFVF,
		iGetFVF,
		iCreateVertexShader,
		iSetVertexShader,
		iGetVertexShader,
		iSetVertexShaderConstantF,
		iGetVertexShaderConstantF,
		iSetVertexShaderConstantI,
		iGetVertexShaderConstantI,
		iSetVertexShaderConstantB,
		iGetVertexShaderConstantB,
		iSetStreamSource,
		iGetStreamSource,
		iSetStreamSourceFreq,
		iGetStreamSourceFreq,
		iSetIndices,
		iGetIndices,
		iCreatePixelShader,
		iSetPixelShader,
		iGetPixelShader,
		iSetPixelShaderConstantF,
		iGetPixelShaderConstantF,
		iSetPixelShaderConstantI,
		iGetPixelShaderConstantI,
		iSetPixelShaderConstantB,
		iGetPixelShaderConstantB,
		iDrawRectPatch,
		iDrawTriPatch,
		iDeletePatch,
		iCreateQuery,
		vfTableEntryCount
	};

	// set the structure alignment to 1 byte
	#pragma pack(push, 1)

	/*** IUnknown methods ***/
	typedef HRESULT	(__stdcall IUnknown::*fnQueryInterface)(REFIID, void**);
	typedef ULONG	(__stdcall IUnknown::*fnAddRef)(void);
	typedef ULONG	(__stdcall IUnknown::*fnRelease)(void);
	/*** IDirect3DDevice9 methods ***/
	typedef HRESULT (__stdcall IDirect3DDevice9::*fnTestCooperativeLevel)();
	typedef UINT	(__stdcall IDirect3DDevice9::*fnGetAvailableTextureMem)();
	typedef HRESULT (__stdcall IDirect3DDevice9::*fnEvictManagedResources)();
	typedef HRESULT (__stdcall IDirect3DDevice9::*fnGetDirect3D)(IDirect3D9**);
	typedef HRESULT (__stdcall IDirect3DDevice9::*fnGetDeviceCaps)(D3DCAPS9*);
	typedef HRESULT (__stdcall IDirect3DDevice9::*fnGetDisplayMode)(UINT,D3DDISPLAYMODE*);
	typedef HRESULT (__stdcall IDirect3DDevice9::*fnGetCreationParameters)(D3DDEVICE_CREATION_PARAMETERS*);
	typedef HRESULT (__stdcall IDirect3DDevice9::*fnSetCursorProperties)(UINT,UINT,IDirect3DSurface9*);
	typedef void    (__stdcall IDirect3DDevice9::*fnSetCursorPosition)(int,int,DWORD);
	typedef BOOL    (__stdcall IDirect3DDevice9::*fnShowCursor)(BOOL);
	typedef HRESULT (__stdcall IDirect3DDevice9::*fnCreateAdditionalSwapChain)(D3DPRESENT_PARAMETERS*,IDirect3DSwapChain9**);
	typedef HRESULT (__stdcall IDirect3DDevice9::*fnGetSwapChain)(UINT,IDirect3DSwapChain9**);
	typedef UINT    (__stdcall IDirect3DDevice9::*fnGetNumberOfSwapChains)();
	typedef HRESULT (__stdcall IDirect3DDevice9::*fnReset)(D3DPRESENT_PARAMETERS*);
	typedef HRESULT (__stdcall IDirect3DDevice9::*fnPresent)(CONST RECT*,CONST RECT*,HWND,CONST RGNDATA*);
	typedef HRESULT (__stdcall IDirect3DDevice9::*fnGetBackBuffer)(UINT,UINT,D3DBACKBUFFER_TYPE,IDirect3DSurface9**);
	typedef HRESULT (__stdcall IDirect3DDevice9::*fnGetRasterStatus)(UINT,D3DRASTER_STATUS*);
	typedef HRESULT (__stdcall IDirect3DDevice9::*fnSetDialogBoxMode)(BOOL);
	typedef void    (__stdcall IDirect3DDevice9::*fnSetGammaRamp)(UINT,DWORD,CONST D3DGAMMARAMP*);
	typedef void    (__stdcall IDirect3DDevice9::*fnGetGammaRamp)(UINT,D3DGAMMARAMP*);
	typedef HRESULT (__stdcall IDirect3DDevice9::*fnCreateTexture)(UINT,UINT,UINT,DWORD,D3DFORMAT,D3DPOOL,IDirect3DTexture9**,HANDLE*);
	typedef HRESULT (__stdcall IDirect3DDevice9::*fnCreateVolumeTexture)(UINT,UINT,UINT,UINT,DWORD,D3DFORMAT,D3DPOOL,IDirect3DVolumeTexture9**,HANDLE*);
	typedef HRESULT (__stdcall IDirect3DDevice9::*fnCreateCubeTexture)(UINT,UINT,DWORD,D3DFORMAT,D3DPOOL,IDirect3DCubeTexture9**,HANDLE*);
	typedef HRESULT (__stdcall IDirect3DDevice9::*fnCreateVertexBuffer)(UINT,DWORD,DWORD,D3DPOOL,IDirect3DVertexBuffer9**,HANDLE*);
	typedef HRESULT (__stdcall IDirect3DDevice9::*fnCreateIndexBuffer)(UINT,DWORD,D3DFORMAT,D3DPOOL,IDirect3DIndexBuffer9**,HANDLE*);
	typedef HRESULT (__stdcall IDirect3DDevice9::*fnCreateRenderTarget)(UINT,UINT,D3DFORMAT,D3DMULTISAMPLE_TYPE,DWORD,BOOL,IDirect3DSurface9**,HANDLE*);
	typedef HRESULT (__stdcall IDirect3DDevice9::*fnCreateDepthStencilSurface)(UINT,UINT,D3DFORMAT,D3DMULTISAMPLE_TYPE,DWORD,BOOL,IDirect3DSurface9**,HANDLE*);
	typedef HRESULT (__stdcall IDirect3DDevice9::*fnUpdateSurface)(IDirect3DSurface9*,CONST RECT*,IDirect3DSurface9*,CONST POINT*);
	typedef HRESULT (__stdcall IDirect3DDevice9::*fnUpdateTexture)(IDirect3DBaseTexture9*,IDirect3DBaseTexture9*);
	typedef HRESULT (__stdcall IDirect3DDevice9::*fnGetRenderTargetData)(IDirect3DSurface9*,IDirect3DSurface9*);
	typedef HRESULT (__stdcall IDirect3DDevice9::*fnGetFrontBufferData)(UINT,IDirect3DSurface9*);
	typedef HRESULT (__stdcall IDirect3DDevice9::*fnStretchRect)(IDirect3DSurface9*,CONST RECT*,IDirect3DSurface9*,CONST RECT*,D3DTEXTUREFILTERTYPE);
	typedef HRESULT (__stdcall IDirect3DDevice9::*fnColorFill)(IDirect3DSurface9*,CONST RECT*,D3DCOLOR);
	typedef HRESULT (__stdcall IDirect3DDevice9::*fnCreateOffscreenPlainSurface)(UINT,UINT,D3DFORMAT,D3DPOOL,IDirect3DSurface9**,HANDLE*);
	typedef HRESULT (__stdcall IDirect3DDevice9::*fnSetRenderTarget)(DWORD,IDirect3DSurface9*);
	typedef HRESULT (__stdcall IDirect3DDevice9::*fnGetRenderTarget)(DWORD,IDirect3DSurface9**);
	typedef HRESULT (__stdcall IDirect3DDevice9::*fnSetDepthStencilSurface)(IDirect3DSurface9*);
	typedef HRESULT (__stdcall IDirect3DDevice9::*fnGetDepthStencilSurface)(IDirect3DSurface9**);
	typedef HRESULT (__stdcall IDirect3DDevice9::*fnBeginScene)();
	typedef HRESULT (__stdcall IDirect3DDevice9::*fnEndScene)();
	typedef HRESULT (__stdcall IDirect3DDevice9::*fnClear)(DWORD,CONST D3DRECT*,DWORD,D3DCOLOR,float,DWORD);
	typedef HRESULT (__stdcall IDirect3DDevice9::*fnSetTransform)(D3DTRANSFORMSTATETYPE,CONST D3DMATRIX*);
	typedef HRESULT (__stdcall IDirect3DDevice9::*fnGetTransform)(D3DTRANSFORMSTATETYPE,D3DMATRIX*);
	typedef HRESULT (__stdcall IDirect3DDevice9::*fnMultiplyTransform)(D3DTRANSFORMSTATETYPE,CONST D3DMATRIX*);
	typedef HRESULT (__stdcall IDirect3DDevice9::*fnSetViewport)(CONST D3DVIEWPORT9*);
	typedef HRESULT (__stdcall IDirect3DDevice9::*fnGetViewport)(D3DVIEWPORT9*);
	typedef HRESULT (__stdcall IDirect3DDevice9::*fnSetMaterial)(CONST D3DMATERIAL9*);
	typedef HRESULT (__stdcall IDirect3DDevice9::*fnGetMaterial)(D3DMATERIAL9*);
	typedef HRESULT (__stdcall IDirect3DDevice9::*fnSetLight)(DWORD,CONST D3DLIGHT9*);
	typedef HRESULT (__stdcall IDirect3DDevice9::*fnGetLight)(DWORD,D3DLIGHT9*);
	typedef HRESULT (__stdcall IDirect3DDevice9::*fnLightEnable)(DWORD,BOOL);
	typedef HRESULT (__stdcall IDirect3DDevice9::*fnGetLightEnable)(DWORD,BOOL*);
	typedef HRESULT (__stdcall IDirect3DDevice9::*fnSetClipPlane)(DWORD,CONST float*);
	typedef HRESULT (__stdcall IDirect3DDevice9::*fnGetClipPlane)(DWORD,float*);
	typedef HRESULT (__stdcall IDirect3DDevice9::*fnSetRenderState)(D3DRENDERSTATETYPE,DWORD);
	typedef HRESULT (__stdcall IDirect3DDevice9::*fnGetRenderState)(D3DRENDERSTATETYPE,DWORD*);
	typedef HRESULT (__stdcall IDirect3DDevice9::*fnCreateStateBlock)(D3DSTATEBLOCKTYPE,IDirect3DStateBlock9**);
	typedef HRESULT (__stdcall IDirect3DDevice9::*fnBeginStateBlock)();
	typedef HRESULT (__stdcall IDirect3DDevice9::*fnEndStateBlock)(IDirect3DStateBlock9**);
	typedef HRESULT (__stdcall IDirect3DDevice9::*fnSetClipStatus)(CONST D3DCLIPSTATUS9*);
	typedef HRESULT (__stdcall IDirect3DDevice9::*fnGetClipStatus)(D3DCLIPSTATUS9*);
	typedef HRESULT (__stdcall IDirect3DDevice9::*fnGetTexture)(DWORD,IDirect3DBaseTexture9**);
	typedef HRESULT (__stdcall IDirect3DDevice9::*fnSetTexture)(DWORD,IDirect3DBaseTexture9*);
	typedef HRESULT (__stdcall IDirect3DDevice9::*fnGetTextureStageState)(DWORD,D3DTEXTURESTAGESTATETYPE,DWORD*);
	typedef HRESULT (__stdcall IDirect3DDevice9::*fnSetTextureStageState)(DWORD,D3DTEXTURESTAGESTATETYPE,DWORD);
	typedef HRESULT (__stdcall IDirect3DDevice9::*fnGetSamplerState)(DWORD,D3DSAMPLERSTATETYPE,DWORD*);
	typedef HRESULT (__stdcall IDirect3DDevice9::*fnSetSamplerState)(DWORD,D3DSAMPLERSTATETYPE,DWORD);
	typedef HRESULT (__stdcall IDirect3DDevice9::*fnValidateDevice)(DWORD*);
	typedef HRESULT (__stdcall IDirect3DDevice9::*fnSetPaletteEntries)(UINT,CONST PALETTEENTRY*);
	typedef HRESULT (__stdcall IDirect3DDevice9::*fnGetPaletteEntries)(UINT,PALETTEENTRY*);
	typedef HRESULT (__stdcall IDirect3DDevice9::*fnSetCurrentTexturePalette)(UINT);
	typedef HRESULT (__stdcall IDirect3DDevice9::*fnGetCurrentTexturePalette)(UINT*);
	typedef HRESULT (__stdcall IDirect3DDevice9::*fnSetScissorRect)(CONST RECT*);
	typedef HRESULT (__stdcall IDirect3DDevice9::*fnGetScissorRect)(RECT*);
	typedef HRESULT (__stdcall IDirect3DDevice9::*fnSetSoftwareVertexProcessing)(BOOL);
	typedef BOOL    (__stdcall IDirect3DDevice9::*fnGetSoftwareVertexProcessing)();
	typedef HRESULT (__stdcall IDirect3DDevice9::*fnSetNPatchMode)(float);
	typedef float   (__stdcall IDirect3DDevice9::*fnGetNPatchMode)();
	typedef HRESULT (__stdcall IDirect3DDevice9::*fnDrawPrimitive)(D3DPRIMITIVETYPE,UINT,UINT);
	typedef HRESULT (__stdcall IDirect3DDevice9::*fnDrawIndexedPrimitive)(D3DPRIMITIVETYPE,INT,UINT,UINT,UINT,UINT);
	typedef HRESULT (__stdcall IDirect3DDevice9::*fnDrawPrimitiveUP)(D3DPRIMITIVETYPE,UINT,CONST void*,UINT);
	typedef HRESULT (__stdcall IDirect3DDevice9::*fnDrawIndexedPrimitiveUP)(D3DPRIMITIVETYPE,UINT,UINT,UINT,CONST void*,D3DFORMAT,CONST void*,UINT);
	typedef HRESULT (__stdcall IDirect3DDevice9::*fnProcessVertices)(UINT,UINT,UINT,IDirect3DVertexBuffer9*,IDirect3DVertexDeclaration9*,DWORD);
	typedef HRESULT (__stdcall IDirect3DDevice9::*fnCreateVertexDeclaration)(CONST D3DVERTEXELEMENT9*,IDirect3DVertexDeclaration9**);
	typedef HRESULT (__stdcall IDirect3DDevice9::*fnSetVertexDeclaration)(IDirect3DVertexDeclaration9*);
	typedef HRESULT (__stdcall IDirect3DDevice9::*fnGetVertexDeclaration)(IDirect3DVertexDeclaration9**);
	typedef HRESULT (__stdcall IDirect3DDevice9::*fnSetFVF)(DWORD);
	typedef HRESULT (__stdcall IDirect3DDevice9::*fnGetFVF)(DWORD*);
	typedef HRESULT (__stdcall IDirect3DDevice9::*fnCreateVertexShader)(CONST DWORD*,IDirect3DVertexShader9**);
	typedef HRESULT (__stdcall IDirect3DDevice9::*fnSetVertexShader)(IDirect3DVertexShader9*);
	typedef HRESULT (__stdcall IDirect3DDevice9::*fnGetVertexShader)(IDirect3DVertexShader9**);
	typedef HRESULT (__stdcall IDirect3DDevice9::*fnSetVertexShaderConstantF)(UINT,CONST float*,UINT);
	typedef HRESULT (__stdcall IDirect3DDevice9::*fnGetVertexShaderConstantF)(UINT,float*,UINT);
	typedef HRESULT (__stdcall IDirect3DDevice9::*fnSetVertexShaderConstantI)(UINT,CONST int*,UINT);
	typedef HRESULT (__stdcall IDirect3DDevice9::*fnGetVertexShaderConstantI)(UINT,int*,UINT);
	typedef HRESULT (__stdcall IDirect3DDevice9::*fnSetVertexShaderConstantB)(UINT,CONST BOOL*,UINT);
	typedef HRESULT (__stdcall IDirect3DDevice9::*fnGetVertexShaderConstantB)(UINT,BOOL*,UINT);
	typedef HRESULT (__stdcall IDirect3DDevice9::*fnSetStreamSource)(UINT,IDirect3DVertexBuffer9*,UINT,UINT);
	typedef HRESULT (__stdcall IDirect3DDevice9::*fnGetStreamSource)(UINT,IDirect3DVertexBuffer9**,UINT*,UINT*);
	typedef HRESULT (__stdcall IDirect3DDevice9::*fnSetStreamSourceFreq)(UINT,UINT);
	typedef HRESULT (__stdcall IDirect3DDevice9::*fnGetStreamSourceFreq)(UINT,UINT*);
	typedef HRESULT (__stdcall IDirect3DDevice9::*fnSetIndices)(IDirect3DIndexBuffer9*);
	typedef HRESULT (__stdcall IDirect3DDevice9::*fnGetIndices)(IDirect3DIndexBuffer9**);
	typedef HRESULT (__stdcall IDirect3DDevice9::*fnCreatePixelShader)(CONST DWORD*,IDirect3DPixelShader9**);
	typedef HRESULT (__stdcall IDirect3DDevice9::*fnSetPixelShader)(IDirect3DPixelShader9*);
	typedef HRESULT (__stdcall IDirect3DDevice9::*fnGetPixelShader)(IDirect3DPixelShader9**);
	typedef HRESULT (__stdcall IDirect3DDevice9::*fnSetPixelShaderConstantF)(UINT,CONST float*,UINT);
	typedef HRESULT (__stdcall IDirect3DDevice9::*fnGetPixelShaderConstantF)(UINT,float*,UINT);
	typedef HRESULT (__stdcall IDirect3DDevice9::*fnSetPixelShaderConstantI)(UINT,CONST int*,UINT);
	typedef HRESULT (__stdcall IDirect3DDevice9::*fnGetPixelShaderConstantI)(UINT,int*,UINT);
	typedef HRESULT (__stdcall IDirect3DDevice9::*fnSetPixelShaderConstantB)(UINT,CONST BOOL*,UINT);
	typedef HRESULT (__stdcall IDirect3DDevice9::*fnGetPixelShaderConstantB)(UINT,BOOL*,UINT);
	typedef HRESULT (__stdcall IDirect3DDevice9::*fnDrawRectPatch)(UINT,CONST float*,CONST D3DRECTPATCH_INFO*);
	typedef HRESULT (__stdcall IDirect3DDevice9::*fnDrawTriPatch)(UINT,CONST float*,CONST D3DTRIPATCH_INFO*);
	typedef HRESULT (__stdcall IDirect3DDevice9::*fnDeletePatch)(UINT);
	typedef HRESULT (__stdcall IDirect3DDevice9::*fnCreateQuery)(D3DQUERYTYPE,IDirect3DQuery9**);

	typedef struct _Entries
	{
		fnQueryInterface pfnQueryInterface;
		fnAddRef pfnAddRef;
		fnRelease pfnRelease;

		fnTestCooperativeLevel pfnTestCooperativeLevel;
		fnGetAvailableTextureMem pfnGetAvailableTextureMem;
		fnEvictManagedResources pfnEvictManagedResources;
		fnGetDirect3D pfnGetDirect3D;
		fnGetDeviceCaps pfnGetDeviceCaps;
		fnGetDisplayMode pfnGetDisplayMode;
		fnGetCreationParameters pfnGetCreationParameters;
		fnSetCursorProperties pfnSetCursorProperties;
		fnSetCursorPosition pfnSetCursorPosition;
		fnShowCursor pfnShowCursor;
		fnCreateAdditionalSwapChain pfnCreateAdditionalSwapChain;
		fnGetSwapChain pfnGetSwapChain;
		fnGetNumberOfSwapChains pfnGetNumberOfSwapChains;
		fnReset pfnReset;
		fnPresent pfnPresent;
		fnGetBackBuffer pfnGetBackBuffer;
		fnGetRasterStatus pfnGetRasterStatus;
		fnSetDialogBoxMode pfnSetDialogBoxMode;
		fnSetGammaRamp pfnSetGammaRamp;
		fnGetGammaRamp pfnGetGammaRamp;
		fnCreateTexture pfnCreateTexture;
		fnCreateVolumeTexture pfnCreateVolumeTexture;
		fnCreateCubeTexture pfnCreateCubeTexture;
		fnCreateVertexBuffer pfnCreateVertexBuffer;
		fnCreateIndexBuffer pfnCreateIndexBuffer;
		fnCreateRenderTarget pfnCreateRenderTarget;
		fnCreateDepthStencilSurface pfnCreateDepthStencilSurface;
		fnUpdateSurface pfnUpdateSurface;
		fnUpdateTexture pfnUpdateTexture;
		fnGetRenderTargetData pfnGetRenderTargetData;
		fnGetFrontBufferData pfnGetFrontBufferData;
		fnStretchRect pfnStretchRect;
		fnColorFill pfnColorFill;
		fnCreateOffscreenPlainSurface pfnCreateOffscreenPlainSurface;
		fnSetRenderTarget pfnSetRenderTarget;
		fnGetRenderTarget pfnGetRenderTarget;
		fnSetDepthStencilSurface pfnSetDepthStencilSurface;
		fnGetDepthStencilSurface pfnGetDepthStencilSurface;
		fnBeginScene pfnBeginScene;
		fnEndScene pfnEndScene;
		fnClear pfnClear;
		fnSetTransform pfnSetTransform;
		fnGetTransform pfnGetTransform;
		fnMultiplyTransform pfnMultiplyTransform;
		fnSetViewport pfnSetViewport;
		fnGetViewport pfnGetViewport;
		fnSetMaterial pfnSetMaterial;
		fnGetMaterial pfnGetMaterial;
		fnSetLight pfnSetLight;
		fnGetLight pfnGetLight;
		fnLightEnable pfnLightEnable;
		fnGetLightEnable pfnGetLightEnable;
		fnSetClipPlane pfnSetClipPlane;
		fnGetClipPlane pfnGetClipPlane;
		fnSetRenderState pfnSetRenderState;
		fnGetRenderState pfnGetRenderState;
		fnCreateStateBlock pfnCreateStateBlock;
		fnBeginStateBlock pfnBeginStateBlock;
		fnEndStateBlock pfnEndStateBlock;
		fnSetClipStatus pfnSetClipStatus;
		fnGetClipStatus pfnGetClipStatus;
		fnGetTexture pfnGetTexture;
		fnSetTexture pfnSetTexture;
		fnGetTextureStageState pfnGetTextureStageState;
		fnSetTextureStageState pfnSetTextureStageState;
		fnGetSamplerState pfnGetSamplerState;
		fnSetSamplerState pfnSetSamplerState;
		fnValidateDevice pfnValidateDevice;
		fnSetPaletteEntries pfnSetPaletteEntries;
		fnGetPaletteEntries pfnGetPaletteEntries;
		fnSetCurrentTexturePalette pfnSetCurrentTexturePalette;
		fnGetCurrentTexturePalette pfnGetCurrentTexturePalette;
		fnSetScissorRect pfnSetScissorRect;
		fnGetScissorRect pfnGetScissorRect;
		fnSetSoftwareVertexProcessing pfnSetSoftwareVertexProcessing;
		fnGetSoftwareVertexProcessing pfnGetSoftwareVertexProcessing;
		fnSetNPatchMode pfnSetNPatchMode;
		fnGetNPatchMode pfnGetNPatchMode;
		fnDrawPrimitive pfnDrawPrimitive;
		fnDrawIndexedPrimitive pfnDrawIndexedPrimitive;
		fnDrawPrimitiveUP pfnDrawPrimitiveUP;
		fnDrawIndexedPrimitiveUP pfnDrawIndexedPrimitiveUP;
		fnProcessVertices pfnProcessVertices;
		fnCreateVertexDeclaration pfnCreateVertexDeclaration;
		fnSetVertexDeclaration pfnSetVertexDeclaration;
		fnGetVertexDeclaration pfnGetVertexDeclaration;
		fnSetFVF pfnSetFVF;
		fnGetFVF pfnGetFVF;
		fnCreateVertexShader pfnCreateVertexShader;
		fnSetVertexShader pfnSetVertexShader;
		fnGetVertexShader pfnGetVertexShader;
		fnSetVertexShaderConstantF pfnSetVertexShaderConstantF;
		fnGetVertexShaderConstantF pfnGetVertexShaderConstantF;
		fnSetVertexShaderConstantI pfnSetVertexShaderConstantI;
		fnGetVertexShaderConstantI pfnGetVertexShaderConstantI;
		fnSetVertexShaderConstantB pfnSetVertexShaderConstantB;
		fnGetVertexShaderConstantB pfnGetVertexShaderConstantB;
		fnSetStreamSource pfnSetStreamSource;
		fnGetStreamSource pfnGetStreamSource;
		fnSetStreamSourceFreq pfnSetStreamSourceFreq;
		fnGetStreamSourceFreq pfnGetStreamSourceFreq;
		fnSetIndices pfnSetIndices;
		fnGetIndices pfnGetIndices;
		fnCreatePixelShader pfnCreatePixelShader;
		fnSetPixelShader pfnSetPixelShader;
		fnGetPixelShader pfnGetPixelShader;
		fnSetPixelShaderConstantF pfnSetPixelShaderConstantF;
		fnGetPixelShaderConstantF pfnGetPixelShaderConstantF;
		fnSetPixelShaderConstantI pfnSetPixelShaderConstantI;
		fnGetPixelShaderConstantI pfnGetPixelShaderConstantI;
		fnSetPixelShaderConstantB pfnSetPixelShaderConstantB;
		fnGetPixelShaderConstantB pfnGetPixelShaderConstantB;
		fnDrawRectPatch pfnDrawRectPatch;
		fnDrawTriPatch pfnDrawTriPatch;
		fnDeletePatch pfnDeletePatch;
		fnCreateQuery pfnCreateQuery;
	} Entries;

	#pragma pack(pop)
}

#define CALL_D3DDEVICE_VTABLE(Impl, fn) (Impl->m_pDirect3DDevice->*Impl->m_OriginalVTable.fn)

class Direct3DDevice9WrapperImpl
{
	friend class IDirect3DSwapChain9Wrapper;
	friend class IDirect3DDevice9Wrapper;
public:
	Direct3DDevice9WrapperImpl(IDirect3DDevice9 *pDevice_in, const D3DPRESENT_PARAMETERS &PresentParams_in);
	~Direct3DDevice9WrapperImpl();

	bool AddRenderable(unsigned long ID_in, IRenderable *pRenderable_in);
	bool RemoveRenderable(unsigned long ID_in);

	void SetRendering(bool bEnable_in) { m_bRender = bEnable_in; }
	bool IsFullscreen() const { return m_bFullscreen; }
	void ToggleRendering()	{ m_bRender = !m_bRender; }
	bool IsRendering() const { return m_bRender; }	

	void Draw();
	
private:
	void OnDeviceLost(D3DPRESENT_PARAMETERS &PresentationParameters_in_out);
	void PatchVTable(bool bRestore_in);
	void OnDeviceReset();
	void SetHooks();

	D3DDevice9_vTable::Entries m_OriginalVTable;
	D3DDevice9_vTable::Entries *m_pHookVTable;
	D3DPRESENT_PARAMETERS m_PresentParams;	
	IDirect3DDevice9 *m_pDirect3DDevice;
	
	RenderableMap m_UiElements;	
	bool m_bFullscreen;
	bool m_bRender;
};

#endif//__DIRECT3DDEVICE9_WRAPPER_IMPL_H__
