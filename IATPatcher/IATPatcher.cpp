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
	/*! \brief Replace the function pointer in a module's IAT
		\param[in] hmMod_in : Module to use IAT from
		\param[in] psImportedModuleName_in : Name of imported DLL from which function is imported
		\param[in] psImportedProcName_in : Name of imported function
		\param[in] ppvOriginalProc_out : Original function
		\param[in] pvHookingProc_in : Function to be written to IAT
		\return true if the IAT was successfully patched; false otherwise
	*/
	bool PatchIAT(HMODULE hmMod_in, const char *psImportedModuleName_in,
				  const char *psImportedProcName_in,
				  PVOID *ppvOriginalProc_out, const PVOID pvHookingProc_in)
	{
		PIMAGE_IMPORT_DESCRIPTOR pImportDescriptor;
		PIMAGE_DOS_HEADER pDOSHeader;
		PIMAGE_NT_HEADERS pNTHeader;
		UINT uiIter;	
	
		pDOSHeader = (PIMAGE_DOS_HEADER)hmMod_in;
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
			
			if (_stricmp(psDLLName, psImportedModuleName_in) == 0)
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
						if (_stricmp((char*)pImportByName->Name, psImportedProcName_in) == 0)
						{
							MEMORY_BASIC_INFORMATION memInfoThunk;
							DWORD dwDummy;

							// Make page writable.
							VirtualQuery(pFirstThunkIter, &memInfoThunk, sizeof(memInfoThunk));
							if (VirtualProtect(memInfoThunk.BaseAddress, memInfoThunk.RegionSize, PAGE_READWRITE, &memInfoThunk.Protect))
							{
								// Replace function pointers (non-atomically).
								if (ppvOriginalProc_out)
									*ppvOriginalProc_out = (PVOID)pFirstThunkIter->u1.Function;

								pFirstThunkIter->u1.Function = (DWORD_PTR)pvHookingProc_in;
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

	/*! \brief Replace the function pointer in a module's IAT
		\param[in] hmMod_in : Module to use IAT from
		\param[in] psImportedModuleName_in : Name of imported DLL from which function is imported
		\param[in] psImportedProcName_in : Name of imported function
		\param[in] pvOriginalProc_in : Original function
		\return true if the IAT was successfully patched; false otherwise
	*/
	bool RestoreIAT(HMODULE hmMod_in, const char *psImportedModuleName_in,
					const char *psImportedProcName_in, const PVOID pvOriginalProc_in)
	{
		PIMAGE_IMPORT_DESCRIPTOR pImportDescriptor;
		PIMAGE_DOS_HEADER pDOSHeader;
		PIMAGE_NT_HEADERS pNTHeader;
		UINT uiIter;	

		pDOSHeader = (PIMAGE_DOS_HEADER)hmMod_in;
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

			if (_stricmp(psDLLName, psImportedModuleName_in) == 0)
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
						if (_stricmp((char*)pImportByName->Name, psImportedProcName_in) == 0)
						{
							MEMORY_BASIC_INFORMATION memInfoThunk;
							DWORD dwDummy;

							// Make page writable.
							VirtualQuery(pFirstThunkIter, &memInfoThunk, sizeof(memInfoThunk));
							if (VirtualProtect(memInfoThunk.BaseAddress, memInfoThunk.RegionSize, PAGE_READWRITE, &memInfoThunk.Protect))
							{
								pFirstThunkIter->u1.Function = (DWORD_PTR)pvOriginalProc_in;
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