/**************************************************************************
	created		:	2011-05-29
	filename	: 	string_t.h
	author		:	xebeth`
	copyright	:	North Edge (2011)
	purpose		:	wchar_t based std::basic_string<T> with formatting support
**************************************************************************/
#ifndef __STRING_T_H__
#define __STRING_T_H__

#include <stdarg.h>
#include <tchar.h>
#include <string>
#include <queue>

typedef std::basic_string<TCHAR> string_t;
typedef string_t::size_type pos_t;

typedef std::basic_ostream<TCHAR> ostream_t;
typedef std::basic_istream<TCHAR> istream_t;

#ifndef STRING_T_NPOS
	#define STRING_T_NPOS string_t::npos
#endif

template<typename T> size_t get_format_length(const T* pFormat_in, va_list ArgList_in);
template<> size_t get_format_length(const wchar_t* pFormat_in, va_list ArgList_in);
template<> size_t get_format_length(const char* pFormat_in, va_list ArgList_in);

template<typename T> size_t format_arglist(T** pBuffer_in_out, size_t BufferSize_in, const T* pFormat_in, va_list ArgList_in);
template<> size_t format_arglist(wchar_t** pBuffer_in_out, size_t BufferSize_in, const wchar_t* pFormat_in, va_list ArgList_in);
template<> size_t format_arglist(char** pBuffer_in_out, size_t BufferSize_in, const char* pFormat_in, va_list ArgList_in);

/*! \brief Formats a string a returns a reference to it using an argument list and format
	\param[out] String_out : the string to be formatted
	\param[in] pFormat_in : the format of the string
	\param[in] ArgList_in : the arguments used to format the string
	\return the formatted string
*/
template<typename T> std::basic_string<T>& format(std::basic_string<T> &String_out, const T* pFormat_in, va_list ArgList_in)
{
	if (pFormat_in != NULL)
	{
		size_t DataSize, StrLength = get_format_length(pFormat_in, ArgList_in);

		DataSize = StrLength * sizeof(T);
		// abort if _vsctprintf failed
		if (DataSize != 0)
		{
			// allocate the memory
			T *pBuffer = new T[DataSize];

			// make sure the allocation succeeded
			if (pBuffer != NULL)
			{
				// clear the buffer
				memset(pBuffer, 0, DataSize);
				
				if (format_arglist(&pBuffer, StrLength, pFormat_in, ArgList_in) == StrLength)
				{
					String_out.assign(pBuffer, StrLength);
				}
				// cleanup
				delete[](pBuffer);
			}
		}
	}

	return String_out;
}

/*! \brief Formats a string a returns a reference to it using an argument list and format
	\param[out] String_out : the string to be formatted
	\param[in] pFormat_in : the format of the string
	\param[in] ... : the arguments used to format the string
	\return the formatted string
*/
template<typename T> std::basic_string<T>& format(std::basic_string<T> &String_out, const T* pFormat_in, ...)
{
	va_list ArgList;

	va_start(ArgList, pFormat_in);
	format(String_out, pFormat_in, ArgList);
	va_end(ArgList);

	return String_out;
}

/*! \brief Formats a string using an argument list and format
	\param[in] pFormat_in : the format of the string
	\param[in] ... : the arguments used to format the string
	\return the formatted string
*/
template<typename T> std::basic_string<T> format(const T* pFormat_in, ...)
{
	std::basic_string<T> String;
	va_list ArgList;

	va_start(ArgList, pFormat_in);
	String = format(String, pFormat_in, ArgList);
	va_end(ArgList);

	return String;
}

/*! \brief Appends a formatted string a returns a reference to it using an argument list and format
	\param[out] String_out : the string to be formatted
	\param[in] pFormat_in : the format of the string
	\param[in] ... : the arguments used to format the string
	\return the formatted string
*/
template<typename T> std::basic_string<T>& append_format(std::basic_string<T> &String_out, const T* pFormat_in, ...)
{
	std::basic_string<T> TmpStr;
	va_list ArgList;

	va_start(ArgList, pFormat_in);
	format(TmpStr, pFormat_in, ArgList);
	va_end(ArgList);

	String_out.append(TmpStr.c_str());

	return String_out;
}

/*! \brief Replaces all instances of Find_in by Replace_in
	\param[in,out] String_in_out : the string in which the replacing takes place
	\param[in] Find_in : the string to find
	\param[in] Replace_in : the string to replace with
*/
template<typename T> std::basic_string<T>& replace(std::basic_string<T> &String_in_out, const std::basic_string<T> &Find_in, 
														 const std::basic_string<T> &Replace_in, size_t StrPos_in = 0)
{
	std::basic_string<T>::size_type ReplaceLength = Replace_in.length();
	std::basic_string<T>::size_type FindLength = Find_in.length();

	while((StrPos_in = String_in_out.find(Find_in, StrPos_in)) != std::basic_string<T>::npos)
	{
		String_in_out.replace(StrPos_in, FindLength, Replace_in.c_str());
		StrPos_in += ReplaceLength;
	}

	return String_in_out;
}

/*! \brief Tokenize a string given a delimiter and separator
	\param[in] String_in : the string to tokenize
	\param[out] Tokens_out : the tokens parsed from the string
	\param[in] Separator_in : the separator used to parse the string
	\param[in] Delimiter_in : the delimiter used to parse the string
	\return the number of tokens found
*/
template<typename T> size_t tokenize(const std::basic_string<T> &String_in, std::queue< std::basic_string<T> > &Tokens_out,
									 const T* Separator_in, const T* Delimiter_in)
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