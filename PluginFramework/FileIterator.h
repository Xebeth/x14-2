/**************************************************************************
	created		:	2011-05-29
	filename	: 	FileIterator.h
	author		:	xebeth`
	copyright	:	North Edge (2011)
	purpose		:	File iterator
**************************************************************************/
#ifndef __FILE_ITERATOR_H__
#define __FILE_ITERATOR_H__

#ifndef _PLUGIN_FRAMEWORK_SDK_INCLUDE_
	#error Please include the global header file 'PluginFramework.h'
#endif

/*! \brief File iterator */
class FileIterator : public NonCopyable
{
public:
	explicit FileIterator(const string_t &Directory_in, const TCHAR *pWildcard_in = _T("*.*"));
	~FileIterator();
	
	const TCHAR* Current();
	HANDLE Begin();
	HANDLE End();

	HANDLE operator++();
	HANDLE operator++(int);

	virtual bool IsValid();

protected:
	WIN32_FIND_DATA m_FindData;
	string_t m_Directory;
	string_t m_Wildcard;
	HANDLE m_hFile;

	void NormalizePath(string_t &Directory_in);
};

#endif//__FILE_ITERATOR_H__