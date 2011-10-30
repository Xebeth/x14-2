/**************************************************************************
	created		:	2011-10-30
	filename	: 	SigScan.h
	author		:	Xebeth`
	copyright	:	North Edge (2011)
	purpose		:	Memory scanner based on Aikar's
**************************************************************************/
#ifndef __SIGSCAN_H__
#define __SIGSCAN_H__

namespace SigScan
{
	//! \brief internal structure used in the memory scan
	class MemoryCheck
	{
	public:
		//! \brief MemoryCheck default constructor
		MemoryCheck() : Start(NULL), Size(0U) {}
		/*! \brief MemoryCheck constructor
			\param[in] Start_in : address of the memory chunk to check
			\param[in] Size_in : size of the memory chunk
		*/
		MemoryCheck(size_t Start_in, size_t Size_in)
			: Start(Start_in), Size(Size_in) {}
		//! address of the memory chunk to check
		size_t Start;
		//! size of the memory chunk
		size_t Size;
	};

	//! \brief Memory scanner
	class SigScan
	{
	public:
		SigScan();
		~SigScan();

		DWORD_PTR Scan(const char* pPattern_in, int Offset_in = 0);
		void Initialize(DWORD ProcessID_in, const TCHAR* pModule_in);

	private:
		void TerminateSigScan();

		//! a chunk of the target memory
		BYTE *m_pProcessMemory;
		//! the base address of the target process
		BYTE *m_pBaseAddress;
		//! the 
		DWORD m_dwModSize;
		//! flag specifying if the scanning process has been initialized
		bool m_bInitialized;
		//! flag specifying if the memory chunk is within the local process
		bool m_bIsLocal;
	};
}

#endif//__SIGSCANDLL_H__