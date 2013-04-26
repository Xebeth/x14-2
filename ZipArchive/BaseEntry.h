/**************************************************************************
	created		:	2013-04-24
	filename	: 	BaseEntry.h
	author		:	Xebeth`
	copyright	:	North Edge (2013)
	purpose		:	Base entry object
**************************************************************************/
#ifndef __BASE_ENTRY_H__
#define __BASE_ENTRY_H__

namespace ZipArchiveLib
{
	class BaseEntry
	{
	public:
		virtual ~BaseEntry();

		void SetOffsetInFile(unsigned long OffsetInFile_in);
		unsigned long GetOffsetInFile() const;

		void SetEntryIndex(unsigned long EntryIndex_in);
		unsigned long GetEntryIndex() const;

		void SetEntrySize(unsigned long EntrySize_in);
		unsigned long GetEntrySize() const;

		virtual void CreateHeader() =0;

	protected:
		BaseEntry();

		//! offset of the entry in the ZIP file
		unsigned long m_OffsetInFile;
		//! entry index
		unsigned long m_EntryIndex;
		//! entry size
		unsigned long m_EntrySize;
	};
}

#endif//__BASE_ENTRY_H__
