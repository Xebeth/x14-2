/**************************************************************************
	created		:	2011-10-30
	filename	: 	SigScan.cpp
	author		:	Xebeth`
	copyright	:	North Edge (2011)
	purpose		:	Memory scanner based on Aikar's
**************************************************************************/
#include "stdafx.h"
#include <tlhelp32.h>
#include <Realloc.h>

#include "SigScan.h"
#include "ProcessImage.h"
#include "ScanResult.h"

namespace SigScan
{
	//! \brief SigScan default constructor
	SigScan::SigScan() : m_pCurrentProcess(NULL) {}

	//! \brief SigScan destructor
	SigScan::~SigScan()
	{ Clear(); }

	//! \brief Terminates the scanning process and cleans memory up
	void SigScan::Clear()
	{
		ProcessMap::const_iterator ProcessIt = m_ProcessMap.cbegin();
		ProcessMap::const_iterator EndIt = m_ProcessMap.cend();

		for(; ProcessIt != EndIt; ++ProcessIt)
			delete ProcessIt->second;

		m_pCurrentProcess = NULL;
		m_ProcessMap.clear();
	}
	
	/*! \brief Adds a process to the map of processes
		\param[in] pProcess_in : the process to add
		\return true if the process was added; false otherwise
	*/
	bool SigScan::AddProcess(ProcessImage *pProcess_in)
	{
		if (pProcess_in != NULL)
		{
			m_ProcessMap[pProcess_in->GetProcessKey()] = pProcess_in;

			return true;
		}

		return false;
	}

	/*! \brief Retrieves a result given the process ID and module name
		\param[in] ProcessID_in : the process ID
		\param[in] ModuleName_in : the module name
		\return a pointer to the process image if found; NULL otherwise
	*/
	ProcessImage* SigScan::FindProcess(DWORD ProcessID_in, const string_t & ModuleName_in) const
	{
		ProcessImage DummyInfo(ProcessID_in, ModuleName_in, NULL, 0UL, false);
		ProcessMap::const_iterator ProcessIt = m_ProcessMap.find(DummyInfo.GetProcessKey());

		if (ProcessIt != m_ProcessMap.cend())
			return ProcessIt->second;

		return NULL;
	}

	/*! \brief Initializes the scanning process
		\param[in] ProcessID_in : the process ID of the scanned process
		\param[in] ModuleName_in : the name of the process
	*/
	bool SigScan::Initialize(DWORD ProcessID_in, const string_t &ModuleName_in)
	{
		m_pCurrentProcess = FindProcess(ProcessID_in, ModuleName_in);

		if (m_pCurrentProcess == NULL)
		{
			MODULEENTRY32 uModule;

			ZeroMemory(&uModule, sizeof(uModule));
			uModule.dwSize = sizeof(uModule); 
			// create a snapshot of modules and iterate the list
			HANDLE hModuleSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE | TH32CS_SNAPMODULE32, ProcessID_in);
			bool bCurrentProcess = (GetCurrentProcessId() == ProcessID_in);
			BOOL bContinue = Module32First(hModuleSnapshot, &uModule);
			
			// walk through the process list
			while(bContinue)
			{
				if(ModuleName_in.compare(uModule.szModule) == 0)
				{
					// create the process entry
					m_pCurrentProcess = new ProcessImage(ProcessID_in, ModuleName_in, uModule.modBaseAddr,
														 uModule.modBaseSize, bCurrentProcess);

					if (bCurrentProcess == false && m_pCurrentProcess->InitializeImage() == false)
					{
						// reading the process memory failed
						delete m_pCurrentProcess;
						m_pCurrentProcess = NULL;
					}
					// else the process is the current one, no memory reading necessary
					if (m_pCurrentProcess != NULL)
						AddProcess(m_pCurrentProcess);
					// lookup finished
					break;
				}

				bContinue = Module32Next(hModuleSnapshot, &uModule);
			}

			CloseHandle(hModuleSnapshot);
		}

		return (m_pCurrentProcess != NULL);
	}

	/*! \brief Scans the process image for the specified pattern of opcodes
		\param[in] Pattern_in : the pattern to search for
		\param[in] Offset_in : an offset to apply to the address once the pattern is found
		\return the address of the first match
	*/
	DWORD_PTR SigScan::ScanCode(const std::string &Pattern_in, long Offset_in)
	{
		DWORD_PTR Result = NULL;

		if (m_pCurrentProcess != NULL)
		{
			const BYTE *pBaseAddress = m_pCurrentProcess->GetBaseAddress();

			if (pBaseAddress != NULL)
			{
				Result = Scan(m_pCurrentProcess, pBaseAddress,
							  m_pCurrentProcess->GetImageSize(),
							  Pattern_in, Offset_in);

				if (Result != NULL)
					m_pCurrentProcess->AddResult(Pattern_in, Result, Offset_in);
			}
		}

		return Result;
	}

	/*! \brief Scans the process memory pages for the specified pattern of bytes
		\param[in] Pattern_in : the pattern to search for
		\param[in] Offset_in : an offset to apply to the address once the pattern is found
		\param[in,out] ScanResults_in_out : array receiving the results
		\return the number of results matching the pattern
	*/
	DWORD SigScan::ScanMemory(const std::string &Pattern_in, long Offset_in, 
							  MemoryScanResult &ScanResults_in_out)
	{
		if (m_pCurrentProcess != NULL)
		{
			// open the process for reading
			HANDLE hProcess = OpenProcess(PROCESS_VM_READ | PROCESS_QUERY_INFORMATION, FALSE, m_pCurrentProcess->GetProcessID());

			if (hProcess != NULL)
			{
				bool bIsCurrent = m_pCurrentProcess->IsCurrentProcess();
				const BYTE *pAddr = m_pCurrentProcess->GetBaseAddress()
								  + m_pCurrentProcess->GetImageSize();				
				MEMORY_BASIC_INFORMATION MemoryInfo;				
				BYTE *pMemoryBlock = NULL;
				DWORD_PTR Result = NULL;

				memset(&MemoryInfo, 0, sizeof(MemoryInfo));

				while (VirtualQueryEx(hProcess, pAddr, &MemoryInfo, sizeof(MemoryInfo)) != 0UL)
				{
					if ((MemoryInfo.State & MEM_COMMIT) == MEM_COMMIT
					 && ((MemoryInfo.Protect & (PAGE_NOACCESS | PAGE_GUARD))) == 0x0)
					{
						if (bIsCurrent == false)
						{
							if (Buffer::Realloc(&pMemoryBlock, MemoryInfo.RegionSize)
							 && ReadProcessMemory(hProcess, (LPCVOID)MemoryInfo.BaseAddress,
												  (LPVOID)pMemoryBlock, MemoryInfo.RegionSize, NULL) == FALSE)
							{
								// failed to read from the process memory
								// => try the next memory block
								continue;
							}
						}
						else
							pMemoryBlock = (BYTE*)MemoryInfo.BaseAddress;

						if (pMemoryBlock != NULL)
						{
							Result = Scan(m_pCurrentProcess, pMemoryBlock,
										  MemoryInfo.RegionSize, Pattern_in, Offset_in);

							if (Result != NULL)
							{
								ScanResults_in_out.push_back(Result);
								Result = NULL;
							}
						}
					}

					pAddr = (const BYTE *)MemoryInfo.BaseAddress + MemoryInfo.RegionSize;
				}

				CloseHandle(hProcess);

				if (bIsCurrent == false && pMemoryBlock != NULL)
					free(pMemoryBlock);
			}
		}

		return ScanResults_in_out.size();
	}

	/*! \brief Scans the process memory for the specified pattern of byte codes
		\param[in] pProcess_in : the process image to scan
		\param[in] pMemoryBlock_in : the block of memory to scan
		\param[in] Pattern_in : the pattern to search for
		\param[in] Offset_in : an offset to apply to the address once the pattern is found
		\return the address of the first match
	*/
	DWORD_PTR SigScan::Scan(ProcessImage *pProcess_in, const BYTE* pMemoryBlock_in,
							DWORD BlockSize_in, const std::string &Pattern_in, long Offset_in)
	{
		DWORD_PTR Result = NULL;

		if (pProcess_in != NULL)
		{
			ScanResult *pResult = pProcess_in->FindResult(Pattern_in, Offset_in);

			if (pResult == NULL)
			{
				size_t PatternLength = Pattern_in.length();

				// make sure the pattern is of even length and at least 2 characters
				if (pMemoryBlock_in != NULL && PatternLength >= 2U && PatternLength % 2 == 0)
				{
					std::string::const_iterator PatternIt = Pattern_in.cbegin();
					std::string::const_iterator EndIt = Pattern_in.cend();
					size_t SubPatternStart = 0U, SubPatternLen = 0U;
					size_t BufferSize = PatternLength / 2;
					BYTE Byte1 = '\0', Byte2 = '\0';
					SubPatternArray SubPatterns;
					BYTE *pMemoryPattern = NULL;
					size_t PatternIndex = 0U;
					bool Dereference = true;
					long ResultOffset = 0L;

					if(Pattern_in[0] == '#' && Pattern_in[1] == '#')
					{
						ResultOffset = Offset_in; // return the start of signature + offset
						Dereference = false; // don't follow the resulting pointer
						PatternLength -= 2; // remove ##
						PatternIt += 2; // skip ##
						--BufferSize; // remove ##														
					}
					else if(Pattern_in[0] == '@' && Pattern_in[1] == '@')
					{
						// don't follow the pointer, return the end of the signature + offset
						ResultOffset = --BufferSize + Offset_in; // decrement => remove @@
						Dereference = false; // don't follow the resulting pointer						
						PatternLength -= 2; // remove @@
						PatternIt += 2; // skip @@
					}
					// initialize the pattern from the string
					pMemoryPattern = new BYTE[BufferSize];
					ZeroMemory(pMemoryPattern, BufferSize);

					for (; PatternIt != EndIt; ++PatternIt, ++PatternIndex)
					{
						Byte1 = *PatternIt;
						Byte2 = *(++PatternIt);

						if (((Byte1 >= '0' && Byte1 <= '9')		// check if Byte1 is hexadecimal
							|| (Byte1 <= 'F' && Byte1 >= 'A'))
							&& ((Byte2 >= '0' && Byte2 <= '9')		// check if Byte2 is hexadecimal
							|| (Byte2 <= 'F' && Byte2 >= 'A')))
						{
							// increase the length of the sub-pattern
							++SubPatternLen;
							// convert the 2 characters to a byte value ("14" == 0x14-2 == 20)
							if (Byte1 <= '9')
								pMemoryPattern[PatternIndex] += Byte1 - '0';
							else
								pMemoryPattern[PatternIndex] += Byte1 - 'A' + 10;

							pMemoryPattern[PatternIndex] <<= 4;	 // * 16

							if (Byte2 <= '9')
								pMemoryPattern[PatternIndex] += Byte2 - '0';
							else
								pMemoryPattern[PatternIndex] += Byte2 - 'A' + 10;
						}
						else
						{
							if(Byte1 == 'X' && Byte2 == 'X' && BufferSize - PatternIndex >= 4)
							{
								// set the result position to position of the custom ptr mask
								ResultOffset = PatternIndex;
								// fill the memory pattern with the custom ptr mask
								pMemoryPattern[PatternIndex++]	= 'X';
								pMemoryPattern[PatternIndex++]	= 'X';
								pMemoryPattern[PatternIndex++]	= 'X';
								pMemoryPattern[PatternIndex]	= 'X';
								// skip all the X characters
								while (PatternIt != EndIt && *PatternIt == 'X')
									++PatternIt;
							}
							else 
							{
								// unknown byte value
								pMemoryPattern[PatternIndex] = '?';
							}

							// add the sub-pattern to the array
							if (SubPatternLen > 0)
								SubPatterns.push_back(SubPattern(SubPatternStart, SubPatternLen));
							// set the starting position of the next sub-pattern
							SubPatternStart = PatternIndex + 1;
							// reset the sub-pattern length
							SubPatternLen = 0;
						}
					}
					// add the last sub-pattern
					if(SubPatternLen > 0)
						SubPatterns.push_back(SubPattern(SubPatternStart, SubPatternLen));

					// start the memory search
					SubPatternArray::const_iterator SubPatternIt, SubEndIt = SubPatterns.cend();
					BYTE *pSearchAddr = NULL;
					bool bMatching = true;

					// search for the pattern in the process image
					for	(pSearchAddr = (BYTE*)memchr(pMemoryBlock_in, pMemoryPattern[0], BlockSize_in - BufferSize);
						 pSearchAddr && pSearchAddr < (pMemoryBlock_in + BlockSize_in - BufferSize); 
						 pSearchAddr = (BYTE*)memchr(++pSearchAddr, pMemoryPattern[0], BlockSize_in - BufferSize - (pSearchAddr - pMemoryBlock_in)))
					{
						for (SubPatternIt = SubPatterns.cbegin(); SubPatternIt != SubEndIt; ++SubPatternIt)
						{
							// compare the sub-patterns one by one
							if(memcmp(pMemoryPattern + SubPatternIt->Start,
									  pSearchAddr + SubPatternIt->Start,
									  SubPatternIt->Size) != 0)
							{
								// the current sub-pattern didn't match => abort
								bMatching = false;
								break;
							}
						}
						// all the sub-patterns matched
						if (bMatching)
						{
							Result = (DWORD_PTR)pSearchAddr + ResultOffset;
							break;
						}
						else
							bMatching = true;
					}
					// cleanup
					delete [] pMemoryPattern;
				}
			}
			else
				Result = pResult->GetAddress();
		}

		return Result;
	}
}