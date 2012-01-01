/**************************************************************************
	created		:	2011-05-29
	filename	: 	string_t.cpp
	author		:	xebeth`
	copyright	:	North Edge (2011)
	purpose		:	TCHAR based std::string with formatting support
**************************************************************************/
#include "string_t.h"
#include <stdarg.h>


/*! \brief Appends a formatted string a returns a reference to it using an argument list and format
	\param[out] String_out : the string to be formatted
	\param[in] pFormat_in : the format of the string
	\param[in] ... : the arguments used to format the string
	\return the formatted string
*/
string_t& append_format(string_t &String_out, const TCHAR* pFormat_in, ...)
{
	string_t TmpStr;
	va_list ArgList;

	va_start(ArgList, pFormat_in);
	format(TmpStr, pFormat_in, ArgList);
	va_end(ArgList);

	String_out.append(TmpStr.c_str());

	return String_out;
}

/*! \brief Formats a string a returns a reference to it using an argument list and format
	\param[out] String_out : the string to be formatted
	\param[in] pFormat_in : the format of the string
	\param[in] ArgList_in : the arguments used to format the string
	\return the formatted string
*/
string_t& format(string_t &String_out, const TCHAR* pFormat_in, va_list ArgList_in)
{
	if (pFormat_in != NULL)
	{
		int StrLength, DataSize;
		// compute the size of the formatted message -- +1 => _vscprintf doesn't count the null terminating character
		StrLength = _vsctprintf(pFormat_in, ArgList_in) + 1;
		DataSize = StrLength * sizeof(TCHAR);
		// abort if _vsctprintf failed
		if (DataSize != 0)
		{
			// allocate the memory
			TCHAR *pBuffer = (TCHAR*)malloc(DataSize);

			// make sure the allocation succeeded
			if (pBuffer != NULL)
			{
				// clear the buffer
				memset(pBuffer, 0, DataSize);
				// format the resulting message -- -1 => _vstprintf_s doesn't count the null terminating character
				if (_vstprintf_s(pBuffer, StrLength, pFormat_in, ArgList_in) == StrLength - 1)
				{
					String_out.assign(pBuffer, StrLength);
				}
				// cleanup
				free(pBuffer);
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
string_t& format(string_t &String_out, const TCHAR* pFormat_in, ...)
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
string_t format(const TCHAR* pFormat_in, ...)
{
	string_t String;
	va_list ArgList;

	va_start(ArgList, pFormat_in);
	String = format(String, pFormat_in, ArgList);
	va_end(ArgList);

	return String;
}

/*! \brief Replaces all instances of Find_in by Replace_in
	\param[in,out] String_in_out : the string in which the replacing takes place
	\param[in] Find_in : the string to find
	\param[in] Replace_in : the string to replace with
*/
string_t& replace(string_t &String_in_out, const string_t &Find_in, const string_t &Replace_in)
{
	string_t::size_type ReplaceLength = Replace_in.length();
	string_t::size_type FindLength = Find_in.length();
	string_t::size_type StrPos = 0;

	while((StrPos = String_in_out.find(Find_in, StrPos)) != string_t::npos)
	{
		String_in_out.replace(StrPos, FindLength, Replace_in.c_str());
		StrPos += ReplaceLength;
	}

	return String_in_out;
}

/*! \brief Formats a string a returns a reference to it using an argument list and format
	\param[out] String_out : the string to be formatted
	\param[in] pFormat_in : the format of the string
	\param[in] ArgList_in : the arguments used to format the string
	\return the formatted string
*/
std::string& format(std::string &String_out, const char* pFormat_in, va_list ArgList_in)
{
	if (pFormat_in != NULL)
	{
		int StrLength, DataSize;
		// compute the size of the formatted message -- +1 => _vscprintf doesn't count the null terminating character
		StrLength = _vscprintf(pFormat_in, ArgList_in) + 1;
		DataSize = StrLength * sizeof(char);
		// abort if _vsctprintf failed
		if (DataSize != 0)
		{
			// allocate the memory
			char *pBuffer = (char*)malloc(DataSize);

			// make sure the allocation succeeded
			if (pBuffer != NULL)
			{
				// clear the buffer
				memset(pBuffer, 0, DataSize);
				// format the resulting message -- -1 => _vstprintf_s doesn't count the null terminating character
				if (vsprintf_s(pBuffer, StrLength, pFormat_in, ArgList_in) == StrLength - 1)
				{
					String_out.assign(pBuffer, StrLength);
				}
				// cleanup
				free(pBuffer);
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
std::string& format(std::string &String_out, const char* pFormat_in, ...)
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
std::string format(const char* pFormat_in, ...)
{
	std::string String;
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
std::string& append_format(std::string &String_out, const char* pFormat_in, ...)
{
	std::string TmpStr;
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
std::string& replace(std::string &String_in_out, const std::string &Find_in, const std::string &Replace_in)
{
	std::string::size_type ReplaceLength = Replace_in.length();
	std::string::size_type FindLength = Find_in.length();
	std::string::size_type StrPos = 0;

	while((StrPos = String_in_out.find(Find_in, StrPos)) != std::string::npos)
	{
		String_in_out.replace(StrPos, FindLength, Replace_in.c_str());
		StrPos += ReplaceLength;
	}

	return String_in_out;
}