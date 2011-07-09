#include "stdafx.h"

#include "SigScan.h"
#include <tlhelp32.h>
#include <tchar.h>

namespace SigScan
{
	BYTE *g_pBaseAddress = NULL;
	BYTE *g_pProcessMemory = NULL;
	DWORD g_dwModSize = NULL;
	bool g_bInitialized = false;
	bool g_bIsLocal = false;

	typedef struct checks
	{
		size_t start;
		size_t size;
		checks() { start = NULL; size = 0; }
		checks(size_t sstart, size_t ssize) { start = sstart; size = ssize; }
	} _checks;


	void InitializeSigScan(DWORD ProcessID, const TCHAR *Module)
	{
		MODULEENTRY32 uModule;
		SecureZeroMemory(&uModule, sizeof(MODULEENTRY32));
		uModule.dwSize = sizeof(MODULEENTRY32); 
		//Create snapshot of modules and Iterate them
		HANDLE hModuleSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE | TH32CS_SNAPMODULE32, ProcessID);
		for(BOOL bModule = Module32First(hModuleSnapshot, &uModule);bModule;bModule = Module32Next(hModuleSnapshot, &uModule))
		{
			uModule.dwSize = sizeof(MODULEENTRY32); 
			if(!_tcsicmp(uModule.szModule, Module))
			{
				TerminateSigScan();
				g_pBaseAddress = uModule.modBaseAddr;
				g_dwModSize = uModule.modBaseSize;
				if(GetCurrentProcessId() == ProcessID)
				{
					g_bIsLocal = true;
					g_bInitialized = true;
					g_pProcessMemory = g_pBaseAddress;
				}else{
					g_bIsLocal = false;
					g_pProcessMemory = new BYTE[g_dwModSize];
					HANDLE hProcess = OpenProcess(PROCESS_VM_READ, FALSE, ProcessID);
					if(hProcess)
					{
						if(ReadProcessMemory(hProcess,(LPCVOID)g_pBaseAddress,g_pProcessMemory,g_dwModSize,NULL))
						{
							g_bInitialized = true;
						}
						CloseHandle(hProcess);
					}
				}
				break;
			}
		}
		CloseHandle(hModuleSnapshot);
	}

	void TerminateSigScan()
	{
		if(g_pProcessMemory)
		{
			if(!g_bIsLocal)
			{
				delete g_pProcessMemory;
			}
			g_pProcessMemory = NULL;
			g_bInitialized = false;
		}
	}

	DWORD_PTR Scan(const char * szPattern, int offset)
	{

		//Get Pattern length
		size_t PatternLength = strlen(szPattern);
		//Pattern must be divisible by 2 to be valid.
		if(PatternLength % 2 != 0 || PatternLength < 2 || !g_bInitialized || !g_pProcessMemory || !g_pBaseAddress) return NULL;
		//Get the buffer size
		size_t buffersize = PatternLength / 2;
		//Setup custom ptr location. Default to buffersize(first byte after signature)+offset
		size_t PtrOffset = buffersize + offset;
		bool Dereference = true;
		if(memcmp(szPattern,"##",2)==0)
		{
			Dereference = false;
			szPattern += 2;
			PtrOffset = 0 + offset;
			PatternLength -= 2;
			buffersize--;
		}
		//Don't follow the pointer, return the exact end of signature+offset.
		if(memcmp(szPattern,"@@",2)==0)
		{
			Dereference = false;
			szPattern += 2;
			PatternLength -= 2;
			buffersize--;
			PtrOffset--;
		}

		//Capitalize the strings and create a string for cache key.
		char Pattern[1024];
		ZeroMemory(Pattern,sizeof(Pattern));
		strcpy_s(Pattern,sizeof(Pattern),szPattern);
		_strupr_s(Pattern,sizeof(Pattern));


		//Create the buffer
		unsigned char * buffer = new unsigned char[buffersize];
		SecureZeroMemory(buffer,buffersize);

		//array for bytes we need to check and temporary holders for size/start
		checks memchecks[32];
		size_t cmpcount = 0;
		size_t cmpsize = 0;
		size_t cmpstart = 0;
		//Iterate the pattern and build the buffer.
		for(size_t i = 0; i < PatternLength / 2 ; i++)
		{
			//Read the values of the bytes for usage to reduce use of STL.
			unsigned char byte1 = Pattern[i*2];
			unsigned char byte2 = Pattern[(i*2)+1];
			//Check for valid hexadecimal digits.
			if(((byte1 >= '0' && byte1 <= '9') || (byte1 <= 'F' && byte1 >= 'A')) || ((byte2 >= '0' && byte2 <= '9') || (byte2 <= 'F' && byte2 >= 'A')))
			{
				//Increase the comparison size.
				cmpsize++;
				//convert the 2 byte string to a byte value ("14" == 0x14 == 20)
				if (byte1 <= '9') buffer[i] += byte1 - '0';
				else buffer[i] += byte1 - 'A' + 10;
				buffer[i] *= 16;	
				if (byte2 <= '9') buffer[i] += byte2 - '0';
				else buffer[i] += byte2 - 'A' + 10;
				continue;
			}
			//Wasn't valid hex, is it a custom ptr location?
			else if(byte1 == 'X' && byte2 == byte1 && (PatternLength/2) - i > 3) 
			{
				//Set the ptr to this current location + offset.
				PtrOffset = i + offset;
				//Fill the buffer with the ptr locations.
				buffer[i++]	= 'X';
				buffer[i++]	= 'X';
				buffer[i++]	= 'X';
				buffer[i]	= 'X';			
			}
			//Wasn't a custom ptr location nor valid hex, so set it as a wildcard.
			else 
			{
				//? for wildcard, unknown byte value.
				buffer[i]	= '?';
			}
			//Add the check to the array.
			if(cmpsize>0) memchecks[cmpcount++] = checks(cmpstart,cmpsize);
			//Increase the starting check byte and reset the size comparison size.
			cmpstart = i+1;
			cmpsize = 0;
		}
		//Add the final check 
		if(cmpsize>0) memchecks[cmpcount++] = checks(cmpstart,cmpsize);
	
		//Get the current base address and module size.
		BYTE *mBaseAddr = g_pProcessMemory;
		unsigned int mModSize = g_dwModSize;
		//Boolean that returns true or false for matching.
		bool bMatching = true;
		//Iterate the Module.
		for	(BYTE *pAddr = (BYTE*)memchr(mBaseAddr, buffer[0], mModSize - buffersize);
			 pAddr && pAddr < (mBaseAddr + mModSize - buffersize); 
			 pAddr = (BYTE*)memchr(pAddr+1, buffer[0], mModSize - buffersize - (pAddr+1 - mBaseAddr)))
		{
			bMatching = true;
			//Iterate each comparison we need to do. (separated by wildcards)
			for(size_t c = 0;c<cmpcount; c++)
			{
				//Compare the memory.
				if(memcmp(buffer + memchecks[c].start,(void*)(pAddr + memchecks[c].start),memchecks[c].size) != 0)
				{
					//Did not match, try next byte.
					bMatching = false;
					break;
				}
			}
			//After full Pattern scan, check if it matched.
			if(bMatching)
			{
				// Find address wanted in the target memory space - not ours.
				void * Address = NULL;
				if(Dereference)
				{
					Address = *((void **)(pAddr + PtrOffset));
				}else{
					Address = g_pBaseAddress + ((pAddr + PtrOffset) - g_pProcessMemory);
				}
				//Clear buffer and return result.
				delete [] buffer;
				return (DWORD_PTR)Address;
			}
		}
		//Nothing matched. Clear buffer
		delete [] buffer;
		return NULL;
	}
}