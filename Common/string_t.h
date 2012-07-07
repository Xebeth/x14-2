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
#include <queue>

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

string_t& replace (string_t &String_in_out, const string_t &Find_in, const string_t &Replace_in);

std::string& format(std::string &String_out, const char* pFormat_in, va_list ArgList_in);
std::string& format(std::string &String_out, const char* pFormat_in, ...);
std::string format(const char* pFormat_in, ...);

std::string& append_format(std::string &String_out, const char* pFormat_in, ...);

std::string& replace (std::string &String_in_out, const std::string &Find_in, const std::string &Replace_in);

/*! \brief Tokenize a string given a delimiter and separator
	\param[in] String_in : the string to tokenize
	\param[out] Tokens_out : the tokens parsed from the string
	\param[in] Separator_in : the separator used to parse the string
	\param[in] Delimiter_in : the delimiter used to parse the string
	\return the number of tokens found
*/
template<typename T> int tokenize(const std::basic_string<T> String_in, std::queue<std::basic_string<T> > &Tokens_out,
								  const std::basic_string<T> &Separator_in, const std::basic_string<T> &Delimiter_in)
{
	std::basic_string<T>::size_type DelimiterPos = 0, SeparatorPos = 0, NotSeparatorPos = 0, LastPos;

	SeparatorPos = String_in.find_first_of(Separator_in);

	if (SeparatorPos == std::basic_string<T>::npos)
	{
		// no separator found => no parameter
		if (String_in.empty() == false)
			Tokens_out.push(String_in);
	}
	else
	{
		std::basic_string<T> TmpStr = String_in.substr(0, SeparatorPos);

		Tokens_out.push(TmpStr);
		TmpStr = String_in.substr(++SeparatorPos);

		// loop until the buffer has been consumed or only separators are left
		while (TmpStr.empty() == false && NotSeparatorPos != std::basic_string<T>::npos)
		{
			// find the next character that is not a separator
			NotSeparatorPos = TmpStr.find_first_not_of(Separator_in);

			if (NotSeparatorPos != std::basic_string<T>::npos)
			{
				// find the next separator in the buffer
				SeparatorPos = TmpStr.find_first_of(Separator_in, NotSeparatorPos);
				DelimiterPos = TmpStr.find_first_of(Delimiter_in);

				// no separator found => this is the last token
				if (SeparatorPos == std::basic_string<T>::npos && DelimiterPos == SeparatorPos)
				{
					Tokens_out.push(TmpStr.substr(NotSeparatorPos));

					return Tokens_out.size();
				}
				else
				{
					// if the next valid character is not a delimiter => unquoted token
					if (TmpStr[NotSeparatorPos] != Delimiter_in[0])
					{
						Tokens_out.push(TmpStr.substr(NotSeparatorPos, SeparatorPos - NotSeparatorPos));
						TmpStr = TmpStr.substr(++SeparatorPos);
					}
					else
					{
						// the first valid character is the opening delimiter
						DelimiterPos = NotSeparatorPos++;
						LastPos = TmpStr.length() - 1;

						do
						{
							// look for the closing delimiter (delimiter followed by a separator) until
							DelimiterPos = TmpStr.find_first_of(Delimiter_in, DelimiterPos + 1);
						}
						while (DelimiterPos != std::basic_string<T>::npos		// no delimiter was found or
							&& DelimiterPos != LastPos
							&& TmpStr[DelimiterPos + 1] != Separator_in[0]);	// a delimiter a closing delimiter was found

						// a closing delimiter was found
						if (DelimiterPos != std::basic_string<T>::npos)
						{
							Tokens_out.push(TmpStr.substr(NotSeparatorPos, DelimiterPos - NotSeparatorPos));
							TmpStr = TmpStr.substr(++DelimiterPos);
						}
						// we found a stray delimiter => add it as a parameter
						else
						{
							TmpStr = TmpStr.substr(NotSeparatorPos);
							Tokens_out.push(Delimiter_in);
						}
					}
				}
			}
		}
	}

	return Tokens_out.size();
}

#endif//__STRING_T_H__