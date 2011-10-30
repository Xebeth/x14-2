/**************************************************************************
	created		:	2011-03-06
	filename	: 	InjectModule.h
	author		:	Xebeth`
	copyright	:	North Edge (2011)
	purpose		:	IAT patching functions inspired by Visual leak detector
**************************************************************************/
#ifndef __IAT_PATCHER_H__
#define __IAT_PATCHER_H__

// Relative Virtual Address to Virtual Address conversion
#define RVA2PTR(base, rva) (((PBYTE) base) + rva)
#define MAX_STRING 256
#define MAXMODULES 1024

namespace IATPatcher
{
	typedef struct _IMAGE_DELAY_IMPORT_DESCRIPTOR
	{
		DWORD grAttrs;
		DWORD szName;
		DWORD phMod;
		DWORD pIAT;
		DWORD pINT;
		DWORD PBountIAT;
		DWORD pUnloadIAT;
		DWORD dwTimeStamp;
	} IMAGE_DELAY_IMPORT_DESCRIPTOR;

	typedef IMAGE_DELAY_IMPORT_DESCRIPTOR UNALIGNED *PIMAGE_DELAY_IMPORT_DESCRIPTOR;

	bool PatchIAT(HMODULE hmMod, const char *psImportedModuleName,
				  const char *psImportedProcName, 
				  PVOID *ppvOriginalProc, PVOID pvHookingProc);
	bool RestoreIAT(HMODULE hmMod_in, const char *psImportedModuleName_in,
					const char *psImportedProcName_in, PVOID pvOriginalProc_in);
}

#endif//__IAT_PATCHER_H__