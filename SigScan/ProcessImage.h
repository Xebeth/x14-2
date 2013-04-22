/**************************************************************************
	created		:	2013-04-21
	filename	: 	ProcessImage.h
	author		:	Xebeth`
	copyright	:	North Edge (2013)
	purpose		:	
**************************************************************************/
#ifndef __PROCESS_INFO_H__
#define __PROCESS_INFO_H__

#ifndef _MEMORY_SCANNER_SDK_INCLUDE_
	#error Please include the global header file 'MemoryScanner.h'
#endif

namespace SigScan
{
	class ScanResult;
	//! map of signature hash / address
	typedef std::map<long, ScanResult*> ResultMap;

	class ProcessImage
	{
	public:
		ProcessImage(DWORD ProcessID_in, const string_t &ModuleName_in,
					 BYTE *pBaseAddr_in, DWORD ImageSize_in, bool IsCurrent_in);
		virtual ~ProcessImage();

		virtual ScanResult* AddResult(const std::string &Pattern_in,
									  DWORD_PTR RawAddress_in,
				 					  long Offset_in);
		virtual ScanResult* FindResult(const std::string & Pattern_in, long Offset_in);
		virtual const BYTE* GetProcessImage() const;
		virtual const BYTE* GetBaseAddress() const;
		virtual bool InitializeImage();

		bool IsCurrentProcess() const;
		DWORD GetProcessID() const;
		bool IsInitialized() const;
		long GetProcessKey() const;
		DWORD GetImageSize() const;

	protected:
		//! a map of results found in the image
		ResultMap m_ResultMap;
		//! the process image
		BYTE *m_pProcessImage;
		//! the process base address
		BYTE *m_pBaseAddress;
		//! the size of the image
		DWORD m_ImageSize;
		//! the ID of the process
		DWORD m_ProcessID;
		//! flag specifying if the process is the current one
		bool m_IsCurrent;
		//! a key used for insertion in a map
		long m_MapKey;
	};
}

#endif//__PROCESSINFO_H__
