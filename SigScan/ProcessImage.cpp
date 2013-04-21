/**************************************************************************
	created		:	2013-04-21
	filename	: 	ProcessImage.cpp
	author		:	Xebeth`
	copyright	:	North Edge (2013)
	purpose		:	
**************************************************************************/
#include "stdafx.h"

#include "ProcessImage.h"
#include "ScanResult.h"

namespace SigScan
{
	/*! \brief ProcessImage constructor
		\param[in] ProcessID_in : ID of the process
		\param[in] ModuleName_in : Name of the module
		\param[in] pBaseAddr_in : the base address of the process
		\param[in] ImageSize_in : the size of the process image
		\param[in] IsCurrent_in : flag specifying if the process is the current one
	*/
	ProcessImage::ProcessImage(DWORD ProcessID_in, const string_t &ModuleName_in,
							   BYTE *pBaseAddr_in, DWORD ImageSize_in, bool IsCurrent_in)
		: m_IsCurrent(IsCurrent_in), m_MapKey(0L), m_pProcessImage(IsCurrent_in ? pBaseAddr_in : NULL),
		  m_pBaseAddress(pBaseAddr_in), m_ImageSize(ImageSize_in), m_ProcessID(ProcessID_in)
	{
		m_MapKey = CryptUtils::Hash(ModuleName_in) + ProcessID_in;
	}
	
	//! \brief ProcessImage destructor
	ProcessImage::~ProcessImage()
	{
		ResultMap::const_iterator ResultIt = m_ResultMap.cbegin();
		ResultMap::const_iterator EndIt = m_ResultMap.cend();
		
		for (; ResultIt != EndIt; ++ResultIt)
			delete ResultIt->second;

		m_ResultMap.clear();
	}

	/*! \brief Initializes the process memory image
		\return true if the process image was initialized; false otherwise
	*/
	bool ProcessImage::InitializeImage()
	{
		if (m_pProcessImage == NULL)
		{
			// open the process for reading
			HANDLE hProcess = OpenProcess(PROCESS_VM_READ, FALSE, m_ProcessID);

			if(hProcess)
			{
				// allocate the buffer for the memory image
				m_pProcessImage = new BYTE[m_ImageSize];
				// make a copy of the process memory
				if(ReadProcessMemory(hProcess, (LPCVOID)m_pBaseAddress, m_pProcessImage, m_ImageSize, NULL) == FALSE)
				{
					delete[] m_pProcessImage;
					m_pProcessImage = NULL;
				}

				CloseHandle(hProcess);
			}
		}

		return (m_pProcessImage != NULL);
	}

	/*! \brief Adds a result to the process info
		\param[in] Pattern_in : the memory pattern searched
		\param[in] pMatchAddr_in : the matching address
		\param[in] Offset_in : the offset to apply to the result
		\param[in] bDereference_in : flag specifying if the result should be dereferenced
		\return a pointer to the new result
	*/
	ScanResult* ProcessImage::AddResult(const std::string &Pattern_in, const BYTE *pMatchAddr_in,
									    long Offset_in, bool bDereference_in)
	{
		ScanResult *pResult = NULL;

		if (pMatchAddr_in != NULL)
		{
			pResult = new ScanResult(Pattern_in, Offset_in);

			if (bDereference_in)
				pResult->SetAddress(*((DWORD_PTR*)(pMatchAddr_in + Offset_in)));
			else
				pResult->SetAddress((DWORD_PTR)m_pBaseAddress + ((pMatchAddr_in + Offset_in) - m_pProcessImage));

			m_ResultMap[pResult->GetResultKey()] = pResult;
		}

		return pResult;
	}

	/*! \brief Retrieves a result given the pattern and offset used to find it
		\param[in] Pattern_in : the pattern
		\param[in] Offset_in : the offset
		\return a pointer to the result if found; NULL otherwise
	*/
	ScanResult* ProcessImage::FindResult(const std::string & Pattern_in, long Offset_in)
	{
		ScanResult DummyResult(Pattern_in, Offset_in);
		ResultMap::const_iterator ResultIt = m_ResultMap.find(DummyResult.GetResultKey());

		if (ResultIt != m_ResultMap.cend())
			return ResultIt->second;

		return NULL;
	}

	/*! \brief Retrieves the key of the process for insertion in a map
		\return the key of the process
	*/
	long ProcessImage::GetProcessKey() const
	{ return m_MapKey; }

	/*! \brief Checks if the process is initialized
		\return true if the process image is initialized; false otherwise
	*/
	bool ProcessImage::IsInitialized() const
	{ return (m_pProcessImage != NULL); }

	/*! \brief Retrieves the pointer to the process image
		\return the pointer to the process image
	*/
	const BYTE* ProcessImage::GetProcessImage() const
	{ return m_pProcessImage; }

	/*! \brief Retrieves the base address of the process
		\return the base address of the process
	*/
	const BYTE* ProcessImage::GetBaseAddress() const
	{ return m_pBaseAddress; }

	/*! \brief Retrieves the ID of the process
		\return the ID of the process
	*/
	DWORD ProcessImage::GetProcessID() const
	{ return m_ProcessID; }

	/*! \brief Retrieves the size of the process image
		\return the size of the process image
	*/
	DWORD ProcessImage::GetImageSize() const
	{ return m_ImageSize; }
}