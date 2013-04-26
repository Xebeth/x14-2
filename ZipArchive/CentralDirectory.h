/**************************************************************************
	created		:	2013-04-24
	filename	: 	CentralDirectory.h
	author		:	Xebeth`
	copyright	:	North Edge (2013)
	purpose		:	
**************************************************************************/
#ifndef __CENTRAL_DIRECTORY_H__
#define __CENTRAL_DIRECTORY_H__

namespace ZipArchiveLib
{
	// forward declarations
	class EndOfCentralDirectory;
	class CentralDirectoryEntry;

	//! map of entry index, central directory entry pairs
	typedef std::map<unsigned long, CentralDirectoryEntry*> CentralDirectoryEntries;

	class CentralDirectory
	{
	public:
		CentralDirectory();
		~CentralDirectory();

		void SetEndOfCentralDirectory(EndOfCentralDirectory *pEndOfCentralDirectory_in);
		bool AddEntry(unsigned long EntryIndex_in, CentralDirectoryEntry *pEntry_in);		
		bool RemoveEntry(unsigned long EntryIndex_in);

		CentralDirectoryEntries& GetEntries();
		unsigned long GetEntryCount() const;

	protected:
		//! map of the central directory entries
		CentralDirectoryEntries m_Entries;
		//! end of central directory
		EndOfCentralDirectory *m_pEndOfCentralDirectory;
	};
}

#endif//__CENTRAL_DIRECTORY_H__
