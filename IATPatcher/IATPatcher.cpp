/**************************************************************************
	created		:	2011-06-09
	filename	: 	IATPatcher.cpp
	author		:	Xebeth`
	copyright	:	North Edge (2011)
	purpose		:	IAT patching functions inspired by Visual leak detector
**************************************************************************/
#include "stdafx.h"
#include "IATPatcher.h"

namespace IATPatcher
{
	/*++
	  Routine Description:
		Replace the function pointer in a module's IAT.

	  Parameters:
		hmMod              - Module to use IAT from.
		psImportedModuleName  - Name of imported DLL from which
							  function is imported.
		psImportedProcName    - Name of imported function.
		pvHookingProc       - Function to be written to IAT.
		ppvOriginalProc             - Original function.

	  Return Value:
		S_OK on success.
		(any HRESULT) on failure.
	--*/
	bool PatchIAT(HMODULE hmMod, const char *psImportedModuleName,
				  const char *psImportedProcName,
				  PVOID *ppvOriginalProc, PVOID pvHookingProc)
	{
		PIMAGE_IMPORT_DESCRIPTOR pImportDescriptor;
		PIMAGE_DOS_HEADER pDOSHeader;
		PIMAGE_NT_HEADERS pNTHeader;
		UINT uiIter;	
	
		pDOSHeader = (PIMAGE_DOS_HEADER)hmMod;
		pNTHeader = (PIMAGE_NT_HEADERS)RVA2PTR(pDOSHeader, pDOSHeader->e_lfanew);

		if (IMAGE_NT_SIGNATURE != pNTHeader->Signature)
			return false;

		pImportDescriptor = (PIMAGE_IMPORT_DESCRIPTOR)RVA2PTR(pDOSHeader, pNTHeader->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_IMPORT].VirtualAddress);
		
		// Iterate over import descriptors/DLLs.
		for (uiIter = 0; pImportDescriptor[uiIter].Characteristics != 0; ++uiIter)
		{
			const char *psDLLName = (char*)RVA2PTR(pDOSHeader, pImportDescriptor[uiIter].Name);
			PIMAGE_THUNK_DATA pOriginalFirstThunkIter;
			PIMAGE_IMPORT_BY_NAME pImportByName;
			PIMAGE_THUNK_DATA pFirstThunkIter;
			
			if (_stricmp(psDLLName, psImportedModuleName) == 0)
			{
				if (pImportDescriptor[uiIter].FirstThunk == NULL || pImportDescriptor[uiIter].OriginalFirstThunk == NULL)
					return false;

				pOriginalFirstThunkIter = (PIMAGE_THUNK_DATA) RVA2PTR(pDOSHeader, pImportDescriptor[uiIter].OriginalFirstThunk);
				pFirstThunkIter = (PIMAGE_THUNK_DATA) RVA2PTR(pDOSHeader, pImportDescriptor[uiIter].FirstThunk);

				for (; pOriginalFirstThunkIter->u1.Function != NULL; ++pOriginalFirstThunkIter, ++pFirstThunkIter)
				{
					// Ordinal import - we can handle named imports only, so skip it.
					if ((pOriginalFirstThunkIter->u1.Ordinal & IMAGE_ORDINAL_FLAG) == 0)
					{
						pImportByName = (PIMAGE_IMPORT_BY_NAME) RVA2PTR(pDOSHeader, pOriginalFirstThunkIter->u1.AddressOfData);
						// compare the function name
						if (_stricmp((char*)pImportByName->Name, psImportedProcName) == 0)
						{
							MEMORY_BASIC_INFORMATION memInfoThunk;
							DWORD dwDummy;

							// Make page writable.
							VirtualQuery(pFirstThunkIter, &memInfoThunk, sizeof(memInfoThunk));
							if (VirtualProtect(memInfoThunk.BaseAddress, memInfoThunk.RegionSize, PAGE_READWRITE, &memInfoThunk.Protect))
							{
								// Replace function pointers (non-atomically).
								if (ppvOriginalProc)
									*ppvOriginalProc = (PVOID)pFirstThunkIter->u1.Function;

								pFirstThunkIter->u1.Function = (DWORD_PTR)pvHookingProc;
								// Restore page protection.
								return (VirtualProtect(memInfoThunk.BaseAddress, memInfoThunk.RegionSize, memInfoThunk.Protect, &dwDummy) != FALSE);
							}
						}
					}
				}
			}
		}

		return false;
	}

	bool RestoreIAT(HMODULE hmMod, const char *psImportedModuleName,
					const char *psImportedProcName, PVOID pvOriginalProc)
	{
		PIMAGE_IMPORT_DESCRIPTOR pImportDescriptor;
		PIMAGE_DOS_HEADER pDOSHeader;
		PIMAGE_NT_HEADERS pNTHeader;
		UINT uiIter;	

		pDOSHeader = (PIMAGE_DOS_HEADER)hmMod;
		pNTHeader = (PIMAGE_NT_HEADERS)RVA2PTR(pDOSHeader, pDOSHeader->e_lfanew);

		if (IMAGE_NT_SIGNATURE != pNTHeader->Signature)
			return false;

		pImportDescriptor = (PIMAGE_IMPORT_DESCRIPTOR)RVA2PTR(pDOSHeader, pNTHeader->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_IMPORT].VirtualAddress);

		// Iterate over import descriptors/DLLs.
		for (uiIter = 0; pImportDescriptor[uiIter].Characteristics != 0; ++uiIter)
		{
			const char *psDLLName = (char*)RVA2PTR(pDOSHeader, pImportDescriptor[uiIter].Name);
			PIMAGE_THUNK_DATA pOriginalFirstThunkIter;
			PIMAGE_IMPORT_BY_NAME pImportByName;
			PIMAGE_THUNK_DATA pFirstThunkIter;

			if (_stricmp(psDLLName, psImportedModuleName) == 0)
			{
				if (pImportDescriptor[uiIter].FirstThunk == NULL || pImportDescriptor[uiIter].OriginalFirstThunk == NULL)
					return false;

				pOriginalFirstThunkIter = (PIMAGE_THUNK_DATA) RVA2PTR(pDOSHeader, pImportDescriptor[uiIter].OriginalFirstThunk);
				pFirstThunkIter = (PIMAGE_THUNK_DATA) RVA2PTR(pDOSHeader, pImportDescriptor[uiIter].FirstThunk);

				for (; pOriginalFirstThunkIter->u1.Function != NULL; ++pOriginalFirstThunkIter, ++pFirstThunkIter)
				{
					// Ordinal import - we can handle named imports only, so skip it.
					if ((pOriginalFirstThunkIter->u1.Ordinal & IMAGE_ORDINAL_FLAG) == 0)
					{
						pImportByName = (PIMAGE_IMPORT_BY_NAME) RVA2PTR(pDOSHeader, pOriginalFirstThunkIter->u1.AddressOfData);
						// compare the function name
						if (_stricmp((char*)pImportByName->Name, psImportedProcName) == 0)
						{
							MEMORY_BASIC_INFORMATION memInfoThunk;
							DWORD dwDummy;

							// Make page writable.
							VirtualQuery(pFirstThunkIter, &memInfoThunk, sizeof(memInfoThunk));
							if (VirtualProtect(memInfoThunk.BaseAddress, memInfoThunk.RegionSize, PAGE_READWRITE, &memInfoThunk.Protect))
							{
								pFirstThunkIter->u1.Function = (DWORD_PTR)pvOriginalProc;
								// Restore page protection.
								return (VirtualProtect(memInfoThunk.BaseAddress, memInfoThunk.RegionSize, memInfoThunk.Protect, &dwDummy) != FALSE);
							}
						}
					}
				}
			}
		}

		return false;
	}
}