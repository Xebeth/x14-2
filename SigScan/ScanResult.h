/**************************************************************************
	created		:	2013-04-21
	filename	: 	ScanResult.h
	author		:	Xebeth`
	copyright	:	North Edge (2013)
	purpose		:	
**************************************************************************/
#ifndef __SCAN_RESULT_H__
#define __SCAN_RESULT_H__

#ifndef _MEMORY_SCANNER_SDK_INCLUDE_
	#error Please include the global header file 'MemoryScanner.h'
#endif

namespace SigScan
{
	class ScanResult
	{
	public:
		ScanResult(const std::string &PatternStr_in, long Offset_in);

		void SetAddress(DWORD_PTR Address_in);
		DWORD_PTR GetAddress() const;		
		long GetResultKey() const;

	protected:
		//! the value of the result
		DWORD_PTR m_Address;
		//! a key for insertion in a map
		long m_ResultKey;
	};
}

#endif//__SCAN_RESULT_H__
