/**************************************************************************
	created		:	2011-10-30
	filename	: 	SigScan.h
	author		:	Xebeth`
	copyright	:	North Edge (2011)
	purpose		:	Memory scanner based on Aikar's
**************************************************************************/
#ifndef __SIGSCAN_H__
#define __SIGSCAN_H__

#ifndef _MEMORY_SCANNER_SDK_INCLUDE_
	#error Please include the global header file 'MemoryScanner.h'
#endif

namespace SigScan
{
	class ProcessImage;
	//! map of process ID + module name hash / ProcessImage*
	typedef std::map<long, ProcessImage*> ProcessMap;
	typedef std::vector<DWORD_PTR> MemoryScanResult;

	//! \brief Memory scanner
	class SigScan : public NonCopyable
	{
		//! \brief internal structure used in the memory scan
		class SubPattern
		{
		public:
			//! \brief SubPattern default constructor
			SubPattern() : Start(NULL), Size(0U) {}
			/*! \brief SubPattern constructor
				\param[in] Start_in : address of the memory chunk to check
				\param[in] Size_in : size of the memory chunk
			*/
			SubPattern(size_t Start_in, size_t Size_in)
				: Start(Start_in), Size(Size_in) {}
			//! address of the memory chunk to check
			size_t Start;
			//! size of the memory chunk
			size_t Size;
		};

		typedef std::vector<SubPattern> SubPatternArray;

	public:
		SigScan();
		 ~SigScan();

		ProcessImage* FindProcess(DWORD ProcessID_in, const string_t & ModuleName_in) const;
		bool Initialize(DWORD ProcessID_in, const string_t &ModuleName_in);
		bool AddProcess(ProcessImage *pProcess_in);
		void Clear();

		DWORD_PTR ScanCode(const std::string &Pattern_in, long Offset_in);
		DWORD ScanMemory(const std::string &Pattern_in, long Offset_in, 
						 MemoryScanResult &ScanResults_in_out);

	protected:
		DWORD_PTR Scan(ProcessImage *pProcess_in, const BYTE* MemoryBlock_in,
					   DWORD BlockSize_in, const std::string &Pattern_in, long Offset_in);

		//! pointer to the current process image
		ProcessImage *m_pCurrentProcess;
		//! a map of process images
		ProcessMap m_ProcessMap;
	};
}

#endif//__SIGSCANDLL_H__