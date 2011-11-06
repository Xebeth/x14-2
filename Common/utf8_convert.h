/**************************************************************************
	created		:	2011-10-10
	filename	: 	utf8_convert.h
	author		:	Xebeth`
	copyright	:	North Edge (2011)
	purpose		:	
**************************************************************************/
#ifndef __UTF8_CONVERT_H__
#define __UTF8_CONVERT_H__

std::string& convert_ansi(const string_t &String_in, std::string &String_out);
string_t& convert_utf8(const std::string &String_in, string_t &String_out);

size_t utf8_len(const std::string &String_in);
size_t ansi_len(const string_t &String_in);

#endif//__UTF8_CONVERT_H__