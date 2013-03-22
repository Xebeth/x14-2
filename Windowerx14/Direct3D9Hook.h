/**************************************************************************
	created		:	2011-06-03
	filename	: 	Direct3D9Hook.h
	author		:	Xebeth`
	copyright	:	North Edge (2011)
	purpose		:	Direct3D 9 hook definitions
**************************************************************************/
#ifndef __DIRECT3D9_HOOK_H__
#define __DIRECT3D9_HOOK_H__

struct IDirect3D9Ex;
struct IDirect3D9;

typedef IDirect3D9*	(WINAPI *fnDirect3DCreate9)(UINT SDKVersion_in);
IDirect3D9* WINAPI Direct3DCreate9Hook(UINT SDKVersion_in);

typedef HRESULT (WINAPI *fnDirect3DCreate9Ex)(UINT SDKVersion_in, IDirect3D9Ex **ppD3D_out);
HRESULT WINAPI Direct3DCreate9ExHook(UINT SDKVersion_in, IDirect3D9Ex **ppD3D_out);

#endif//__DIRECT3D9_HOOK_H__