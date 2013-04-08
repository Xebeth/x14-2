/**************************************************************************
	created		:	2011-05-29
	filename	: 	string_t.cpp
	author		:	xebeth`
	copyright	:	North Edge (2011)
	purpose		:	TCHAR based std::string with formatting support
**************************************************************************/
#include "stdafx.h"

#include "string_t.h"

template<> size_t get_format_length(const wchar_t* pFormat_in, va_list ArgList_in)
{
	// compute the size of the formatted message -- +1 => _vscwprintf doesn't count the null terminating character
	return _vscwprintf(pFormat_in, ArgList_in) + 1;
}

template<> size_t get_format_length(const char* pFormat_in, va_list ArgList_in)
{
	// compute the size of the formatted message -- +1 => _vscprintf doesn't count the null terminating character
	return _vscprintf(pFormat_in, ArgList_in) + 1;
}

template<> size_t format_arglist(wchar_t** pBuffer_in_out, size_t BufferSize_in, const wchar_t* pFormat_in, va_list ArgList_in)
{
	// format the resulting message -- +1 => vswprintf_s doesn't count the null terminating character
	return vswprintf_s(*pBuffer_in_out, BufferSize_in, pFormat_in, ArgList_in) + 1;
}

template<> size_t format_arglist(char** pBuffer_in_out, size_t BufferSize_in, const char* pFormat_in, va_list ArgList_in)
{
	// format the resulting message -- +1 => vsprintf_s doesn't count the null terminating character
	return vsprintf_s(*pBuffer_in_out, BufferSize_in, pFormat_in, ArgList_in) + 1;
}