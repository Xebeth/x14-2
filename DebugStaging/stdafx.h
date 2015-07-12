/**************************************************************************
	created		:	2011-05-09
	filename	: 	stdafx.h
	author		:	Xebeth`
	copyright	:	North Edge (2011)
	purpose		:	Precompiled headers support
**************************************************************************/
#ifndef __BOOTSTRAP_AFX_H__
#define __BOOTSTRAP_AFX_H__

// #define __USE_VLD
#include <stdhdr.h>
#include <StringUtils.h>

#define DLL_QUERY_HMODULE		6

#define DEREF( name )*(UINT_PTR *)(name)
#define DEREF_64( name )*(DWORD64 *)(name)
#define DEREF_32( name )*(DWORD *)(name)
#define DEREF_16( name )*(WORD *)(name)
#define DEREF_8( name )*(BYTE *)(name)

typedef ULONG_PTR(WINAPI * REFLECTIVELOADER)(VOID);
typedef BOOL(WINAPI * DLLMAIN)(HINSTANCE, DWORD, LPVOID);

#define DLLEXPORT   __declspec( dllexport ) 

#endif//__BOOTSTRAP_AFX_H__