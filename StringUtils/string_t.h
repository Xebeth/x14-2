/**************************************************************************
	created		:	2011-05-29
	filename	: 	string_t.h
	author		:	xebeth`
	copyright	:	North Edge (2011)
	purpose		:	wchar_t based std::basic_string<T> with formatting support
**************************************************************************/
#ifndef __STRING_T_H__
#define __STRING_T_H__

#ifndef _STRING_UTILS_SDK_INCLUDE_
	#error Please include the global header file 'StringUtils.h'
#endif

typedef std::basic_string<TCHAR> string_t;
typedef string_t::size_type pos_t;

typedef std::basic_ostream<TCHAR> ostream_t;
typedef std::basic_istream<TCHAR> istream_t;

#ifndef STRING_T_NPOS
	#define STRING_T_NPOS string_t::npos
#endif

template<typename T> typename std::basic_string<T>::size_type get_format_length(const T* pFormat_in, va_list ArgList_in);
template<> std::basic_string<wchar_t>::size_type get_format_length(const wchar_t* pFormat_in, va_list ArgList_in);
template<> std::basic_string<char>::size_type get_format_length(const char* pFormat_in, va_list ArgList_in);

template<typename T> typename std::basic_string<T>::size_type format_arglist(T** pBuffer_in_out, typename std::basic_string<T>::size_type BufferSize_in, const T* pFormat_in, va_list ArgList_in);
template<> std::basic_string<wchar_t>::size_type format_arglist(wchar_t** pBuffer_in_out, std::basic_string<wchar_t>::size_type BufferSize_in, const wchar_t* pFormat_in, va_list ArgList_in);
template<> std::basic_string<char>::size_type format_arglist(char** pBuffer_in_out, std::basic_string<char>::size_type BufferSize_in, const char* pFormat_in, va_list ArgList_in);

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
		std::basic_string<T>::size_type DataSize, StrLength = get_format_length(pFormat_in, ArgList_in);

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
					// -1 => skip the null terminator
					String_out.assign(pBuffer, StrLength - 1);
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
template<typename T> typename std::basic_string<T>& replace(std::basic_string<T> &String_in_out, const std::basic_string<T> &Find_in, 
															const std::basic_string<T> &Replace_in, typename std::basic_string<T>::size_type StrPos_in = 0UL)
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

template <typename T> class purgeable_chars
{
public:
	purgeable_chars(const T* pExclusions_in, UINT Mask_in, bool Invert_in)
		: m_pExclusions(pExclusions_in), m_PurgeMask(Mask_in), m_Invert(Invert_in ? 0 : 1)  {}

	template<typename T> bool operator() (T val);

	template<> bool operator() (wchar_t val)
	{ return ((m_pExclusions == NULL || wcschr(m_pExclusions, val) == NULL) && iswctype(val, m_PurgeMask) == m_Invert); }
	template<> bool operator() (char val)
	{ return ((m_pExclusions == NULL || strchr(m_pExclusions, val) == NULL) && _isctype(val, m_PurgeMask) == m_Invert); }

private:
	const T* m_pExclusions;
	UINT m_PurgeMask;
	int m_Invert;
};

/*! \brief Removes all control characters
	\param[in,out] String_in_out : the string to clean up
*/
template<typename T> std::basic_string<T>& purge(std::basic_string<T> &String_in_out, UINT Mask_in, T* pExcludedChars = NULL, bool Invert_in = false)
{
	purgeable_chars<T> PurgeablePredicate(pExcludedChars, Mask_in, Invert_in);

	String_in_out.erase(remove_if(String_in_out.begin(),String_in_out.end(), PurgeablePredicate), String_in_out.end());  

	return String_in_out;
}

/*! \brief Normalizes a path by settings all the slashes forward and adding a final slash if needed
	\param[in,out] Path_in_out : the path to normalize
*/
template<typename T> std::basic_string<T>& normalize_path(std::basic_string<T> &Path_in_out, bool bForward_in = false, bool LastSlash_in = true)
{
	if (Path_in_out.empty() == false)
	{
		std::basic_string<T> FindStr, ReplaceStr;
		const T cForward = '/', cBack = '\\';
		
		ReplaceStr += (bForward_in ? cForward : cBack);
		FindStr += (bForward_in ? cBack : cForward);

		replace(Path_in_out, FindStr, ReplaceStr);

		if (LastSlash_in && Path_in_out.back() != ReplaceStr[0])
			Path_in_out += ReplaceStr[0];
	}

	return Path_in_out;
}

/*! \brief Initializes a string with a path and normalizes it
	\param[in] pPath_in : the string containing the raw path
	\param[out] Path_out : the normalized path
	\return the length of the normalized path
*/
template<typename T> typename std::basic_string<T>::size_type initialize_path(const T* pPath_in, std::basic_string<T> &Path_out, bool bForward_in = false)
{
	if (pPath_in != NULL)
	{
		Path_out.assign(pPath_in);
		normalize_path(Path_out, bForward_in);
		Path_out.shrink_to_fit();

		return Path_out.size();
	}

	return 0UL;
}

/*! \brief Tokenize a string given a delimiter and separator
	\param[in] String_in : the string to tokenize
	\param[out] Tokens_out : the tokens parsed from the string
	\param[in] Separator_in : the separator used to parse the string
	\param[in] Delimiter_in : the delimiter used to parse the string
	\return the number of tokens found
*/
template<typename T> typename std::basic_string<T>::size_type tokenize(const std::basic_string<T> &String_in, std::list< std::basic_string<T> > &Tokens_out,
																	   const T* Separator_in, const T* Delimiter_in)
{
	std::basic_string<T>::size_type DelimiterPos = 0, SeparatorPos = 0, NotSeparatorPos = 0, LastPos;

	SeparatorPos = String_in.find_first_of(Separator_in);

	if (SeparatorPos == std::basic_string<T>::npos)
	{
		// no separator found => no parameter
		if (String_in.empty() == false)
			Tokens_out.push_back(String_in);
	}
	else
	{
		std::basic_string<T> TmpStr = String_in.substr(0, SeparatorPos);

		Tokens_out.push_back(TmpStr);
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
					Tokens_out.push_back(TmpStr.substr(NotSeparatorPos));

					return Tokens_out.size();
				}
				else
				{
					// if the next valid character is not a delimiter => unquoted token
					if (TmpStr[NotSeparatorPos] != Delimiter_in[0])
					{
						Tokens_out.push_back(TmpStr.substr(NotSeparatorPos, SeparatorPos - NotSeparatorPos));
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
							Tokens_out.push_back(TmpStr.substr(NotSeparatorPos, DelimiterPos - NotSeparatorPos));
							TmpStr = TmpStr.substr(++DelimiterPos);
						}
						// we found a stray delimiter => add it as a parameter
						else
						{
							TmpStr = TmpStr.substr(NotSeparatorPos);
							Tokens_out.push_back(Delimiter_in);
						}
					}
				}
			}
		}
	}

	return Tokens_out.size();
}

#endif//__STRING_T_H__