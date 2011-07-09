/**************************************************************************
	created		:	2011-05-29
	filename	: 	string_t.h
	author		:	xebeth`
	copyright	:	North Edge (2011)
	purpose		:	TCHAR based std::string with formatting support
**************************************************************************/
#ifndef __STRING_T_H__
#define __STRING_T_H__

#include <tchar.h>
#include <string>

#ifndef STRING_T_NPOS
	#define STRING_T_NPOS string_t::npos
#endif

typedef std::basic_string<TCHAR> string_t;
typedef string_t::size_type pos_t;

typedef std::basic_ostream<TCHAR> ostream_t;
typedef std::basic_istream<TCHAR> istream_t;

string_t& format(string_t &String_out, const TCHAR* pFormat_in, va_list ArgList_in);
string_t& format(string_t &String_out, const TCHAR* pFormat_in, ...);
string_t format(const TCHAR* pFormat_in, ...);

string_t& append_format(string_t &String_out, const TCHAR* pFormat_in, ...);

std::string& format(std::string &String_out, const char* pFormat_in, va_list ArgList_in);
std::string& format(std::string &String_out, const char* pFormat_in, ...);
std::string format(const char* pFormat_in, ...);

std::string& append_format(std::string &String_out, const char* pFormat_in, ...);

#endif//__STRING_T_H__