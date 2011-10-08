/**************************************************************************
	created		:	2011-05-29
	filename	: 	FileIterator.cpp
	author		:	xebeth`
	copyright	:	North Edge (2011)
	purpose		:	File iterator
**************************************************************************/
#include "stdafx.h"
#include "FileIterator.h"

/*! \brief FileIterator constructor
	\param[in] Directory_in : the directory in which to look for files
	\param[in] pWildcard_in : the wildcard used to select files (e.g: *.dll)
*/
FileIterator::FileIterator(const string_t &Directory_in, const TCHAR *pWildcard_in)
			: m_Directory(Directory_in), m_hFile(INVALID_HANDLE_VALUE)
{
	NormalizePath(m_Directory);

	if (pWildcard_in == NULL)
		m_Wildcard = _T("*.*");
	else
		m_Wildcard = pWildcard_in;
}

/*! \brief FileIterator destructor */
FileIterator::~FileIterator()
{
	::FindClose(m_hFile);
}

/*! \brief Starts the file iteration
	\return a handle to the first valid file or INVALID_HANDLE_VALUE if nothing was found
*/
HANDLE FileIterator::Begin()
{
	string_t SearchParams;

	format (SearchParams, _T("%s%s"), m_Directory.c_str(), m_Wildcard.c_str());
	m_hFile  = ::FindFirstFile (SearchParams.c_str(), &m_FindData);

	// skip directories 
	if (m_hFile != INVALID_HANDLE_VALUE && IsValid() == false)
		m_hFile = operator++();

	return m_hFile;
}

/*! \brief Returns the value that marks the end of the iteration
	\return always INVALID_HANDLE_VALUE
*/
HANDLE FileIterator::End()
{
	return INVALID_HANDLE_VALUE;
}

/*! \brief The filename of the current file
	\return the filename of the current file is valid; NULL otherwise
*/
const TCHAR* FileIterator::Current()
{
	if (m_hFile != INVALID_HANDLE_VALUE)
		return m_FindData.cFileName;
	else
		return NULL;
}

/*! \brief Moves the iterator to the next file (prefix increment)
	\return a handle to the next valid file or INVALID_HANDLE_VALUE if no more valid file is found
*/
HANDLE FileIterator::operator++()
{
	BOOL bFileFound = FALSE;

	if (m_hFile != INVALID_HANDLE_VALUE)
	{
		do 
		{
			// move to the next file
			bFileFound = ::FindNextFile(m_hFile, &m_FindData);
		}
		while (bFileFound && FileIterator::IsValid() == false);
	}

	return (bFileFound && IsValid()) ? m_hFile : INVALID_HANDLE_VALUE;
}

/*! \brief Moves the iterator to the next file (postfix increment)
	\param[in] int : unused; needed for postfix increment
	\return a handle to the next valid file or INVALID_HANDLE_VALUE if no more valid file is found
*/
HANDLE FileIterator::operator++(int)
{
	return operator++();
}

/*! \brief Checks if the current file is valid (e.g: not a directory)
	\return true if the current file is valid; false otherwise
*/
bool FileIterator::IsValid()
{
	// skip directories
	return (m_hFile != INVALID_HANDLE_VALUE && (m_FindData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) == 0);
}

/*! \brief Checks the directory for a trailing backslash and adds it if necessary
	\param[in] Directory_in : the directory name to normalize
*/
void FileIterator::NormalizePath(string_t &Directory_in)
{
	if (Directory_in.empty() == false)
	{
		// find the last backslash in the directory
		size_t TrailingBackslashPos = Directory_in.rfind('\\');

		if (TrailingBackslashPos != string_t::npos)
		{
			size_t StrLength = Directory_in.length();

			if (TrailingBackslashPos + 1 < StrLength)
			{
				// the last backslash is not at the end of the string
				Directory_in += '\\';
			}
		}
	}
}