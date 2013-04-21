/**************************************************************************
	created		:	2013-04-21
	filename	: 	ScanResult.cpp
	author		:	Xebeth`
	copyright	:	North Edge (2013)
	purpose		:	
**************************************************************************/
#include "stdafx.h"

#include "ScanResult.h"

namespace SigScan
{
	/*! \brief ScanResult constructor
		\param[in] PatternStr_in : the pattern leading to the result
		\param[in] Offset_in : the offset applied to the intermediate result
	*/
	ScanResult::ScanResult(const std::string &PatternStr_in, long Offset_in)
	{
		string_t Pattern;

		convert_utf8(PatternStr_in, Pattern);
		m_ResultKey = CryptUtils::Hash(Pattern) + Offset_in;
	}

	/*! \brief Sets the value of the result
		\param[in] Address_in : the value of the result
	*/
	void ScanResult::SetAddress(DWORD_PTR Address_in)
	{ m_Address = Address_in; }

	/*! \brief Retrieves the value of the result
		\return the value of the result
	*/
	DWORD_PTR ScanResult::GetAddress() const
	{ return m_Address; }

	/*! \brief Retrieves the key of the result for insertion in a map
		\return the key of the result
	*/
	long ScanResult::GetResultKey() const
	{ return m_ResultKey; }
}