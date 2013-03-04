/**************************************************************************
	created		:	2011-10-30
	filename	: 	SigScan.cpp
	author		:	Xebeth`
	copyright	:	North Edge (2011)
	purpose		:	Memory scanner based on Aikar's
**************************************************************************/
#include "stdafx.h"
#include "SigScan.h"

#include <tlhelp32.h>
#include <tchar.h>

namespace SigScan
{
	//! \brief SigScan default constructor
	SigScan::SigScan()
	{
		m_pBaseAddress = m_pProcessMemory = NULL;
		m_bIsLocal = m_bInitialized = false;
	}

	//! \brief SigScan destructor
	SigScan::~SigScan()
	{
		TerminateSigScan();
	}

	/*! \brief Initializes the scanning process
		\param[in] ProcessID_in : the process ID of the scanned process
		\param[in] pModule_in : the name of the process
	*/
	void SigScan::Initialize(DWORD ProcessID_in, const TCHAR *pModule_in)
	{
		MODULEENTRY32 uModule;

		SecureZeroMemory(&uModule, sizeof(uModule));
		uModule.dwSize = sizeof(uModule); 
		// Create snapshot of modules and Iterate them
		HANDLE hModuleSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE | TH32CS_SNAPMODULE32, ProcessID_in);

		for(BOOL bContinue = Module32First(hModuleSnapshot, &uModule); bContinue; bContinue = Module32Next(hModuleSnapshot, &uModule))
		{
			uModule.dwSize = sizeof(uModule); 

			if(_tcsicmp(uModule.szModule, pModule_in) == 0)
			{
				TerminateSigScan();
				m_pBaseAddress = uModule.modBaseAddr;
				m_dwModSize = uModule.modBaseSize;

				if(GetCurrentProcessId() == ProcessID_in)
				{
					m_bIsLocal = m_bInitialized = true;
					m_pProcessMemory = m_pBaseAddress;
				}
				else
				{
					HANDLE hProcess = OpenProcess(PROCESS_VM_READ, FALSE, ProcessID_in);

					m_pProcessMemory = new BYTE[m_dwModSize];
					m_bIsLocal = false;

					if(hProcess)
					{
						if(ReadProcessMemory(hProcess,(LPCVOID)m_pBaseAddress,m_pProcessMemory,m_dwModSize,NULL))
							m_bInitialized = true;

						CloseHandle(hProcess);
					}
				}
				break;
			}
		}

		CloseHandle(hModuleSnapshot);
	}

	//! \brief Terminates the scanning process and cleans memory up
	void SigScan::TerminateSigScan()
	{
		if (m_pProcessMemory != NULL)
		{
			if (m_bIsLocal == false)
				delete m_pProcessMemory;

			m_pProcessMemory = NULL;
			m_bInitialized = false;
		}
	}
	/*! \brief Scans the process memory for the specified pattern of byte codes
		\param[in] pPattern_in : the pattern to search for
		\param[in] Offset_in : an offset to apply to the address once the pattern is found
		\return the address of the pattern + offset
	*/
	DWORD_PTR SigScan::Scan(const char *pPattern_in, int Offset_in)
	{
		// Get Pattern length
		size_t PatternLength = strlen(pPattern_in);
		// Pattern must be divisible by 2 to be valid.
		if (PatternLength % 2 != 0 || PatternLength < 2 || !m_bInitialized || !m_pProcessMemory || !m_pBaseAddress)
			return NULL;
		// Get the buffer size
		size_t buffersize = PatternLength / 2;
		//Setup custom ptr location. Default to buffersize(first byte after signature)+offset
		size_t PtrOffset = buffersize + Offset_in;
		bool Dereference = true;
		if(memcmp(pPattern_in,"##",2) == 0)
		{
			Dereference = false;
			pPattern_in += 2;
			PtrOffset = 0 + Offset_in;
			PatternLength -= 2;
			--buffersize;
		}
		//Don't follow the pointer, return the exact end of signature+offset.
		if(memcmp(pPattern_in,"@@",2) == 0)
		{
			Dereference = false;
			pPattern_in += 2;
			PatternLength -= 2;
			--buffersize;
			--PtrOffset;
		}

		//Capitalize the strings and create a string for cache key.
		char Pattern[1024];
		ZeroMemory(Pattern,sizeof(Pattern));
		strcpy_s(Pattern,sizeof(Pattern),pPattern_in);
		_strupr_s(Pattern,sizeof(Pattern));

		//Create the buffer
		unsigned char * buffer = new unsigned char[buffersize];
		SecureZeroMemory(buffer,buffersize);

		//array for bytes we need to check and temporary holders for size/start
		MemoryCheck memchecks[32];
		size_t cmpcount = 0;
		size_t cmpsize = 0;
		size_t cmpstart = 0;
		//Iterate the pattern and build the buffer.
		for(size_t i = 0; i < PatternLength / 2 ; ++i)
		{
			//Read the values of the bytes for usage to reduce use of STL.
			unsigned char byte1 = Pattern[i*2];
			unsigned char byte2 = Pattern[(i*2)+1];
			//Check for valid hexadecimal digits.
			if(((byte1 >= '0' && byte1 <= '9') || (byte1 <= 'F' && byte1 >= 'A')) || ((byte2 >= '0' && byte2 <= '9') || (byte2 <= 'F' && byte2 >= 'A')))
			{
				//Increase the comparison size.
				++cmpsize;
				//convert the 2 byte string to a byte value ("14" == 0x14-2 == 20)
				if (byte1 <= '9')
					buffer[i] += byte1 - '0';
				else
					buffer[i] += byte1 - 'A' + 10;
				buffer[i] *= 16;
				if (byte2 <= '9')
					buffer[i] += byte2 - '0';
				else
					buffer[i] += byte2 - 'A' + 10;

				continue;
			}
			//Wasn't valid hex, is it a custom ptr location?
			else if(byte1 == 'X' && byte2 == byte1 && (PatternLength / 2) - i > 3) 
			{
				//Set the ptr to this current location + offset.
				PtrOffset = i + Offset_in;
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
				buffer[i] = '?';
			}
			//Add the check to the array.
			if(cmpsize>0)
				memchecks[cmpcount++] = MemoryCheck(cmpstart, cmpsize);
			//Increase the starting check byte and reset the size comparison size.
			cmpstart = i+1;
			cmpsize = 0;
		}

		//Add the final check 
		if(cmpsize > 0)
			memchecks[cmpcount++] = MemoryCheck(cmpstart, cmpsize);
	
		//Get the current base address and module size.
		BYTE *pBaseAddr = m_pProcessMemory;
		unsigned int mModSize = m_dwModSize;
		//Boolean that returns true or false for matching.
		bool bMatching = true;
		//Iterate the Module.
		for	(BYTE *pAddr = (BYTE*)memchr(pBaseAddr, buffer[0], mModSize - buffersize);
			 pAddr && pAddr < (pBaseAddr + mModSize - buffersize); 
			 pAddr = (BYTE*)memchr(pAddr+1, buffer[0], mModSize - buffersize - (pAddr+1 - pBaseAddr)))
		{
			bMatching = true;
			//Iterate each comparison we need to do. (separated by wildcards)
			for(size_t c = 0; c < cmpcount; ++c)
			{
				//Compare the memory.
				if(memcmp(buffer + memchecks[c].Start,(LPVOID)(pAddr + memchecks[c].Start),memchecks[c].Size) != 0)
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
				LPVOID Address = NULL;

				if(Dereference)
					Address = *((LPVOID*)(pAddr + PtrOffset));
				else
					Address = m_pBaseAddress + ((pAddr + PtrOffset) - m_pProcessMemory);
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