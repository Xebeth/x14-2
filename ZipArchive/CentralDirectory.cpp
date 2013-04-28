/**************************************************************************
	created		:	2013-04-25
	filename	: 	CentralDirectory.cpp
	author		:	Xebeth`
	copyright	:	North Edge (2013)
	purpose		:	
**************************************************************************/
#include "stdafx.h"

#include "CentralDirectory.h"

#include "BaseEntry.h"
#include "ZipFileEntry.h"
#include "CentralDirectoryEntry.h"
#include "EndOfCentralDirectory.h"

namespace ZipArchiveLib
{ 
	//! \brief CentralDirectory default constructor
	CentralDirectory::CentralDirectory()
		: m_pEndOfCentralDirectory(NULL) {}

	//! \brief CentralDirectory destructor
	CentralDirectory::~CentralDirectory()
	{
		CentralDirectoryEntries::const_iterator EntryIt, EndIt = m_Entries.cend();

		for (EntryIt = m_Entries.cbegin(); EntryIt != EndIt; ++EntryIt)
			delete EntryIt->second;

		m_Entries.clear();

		if (m_pEndOfCentralDirectory != NULL)
		{
			delete m_pEndOfCentralDirectory;
			m_pEndOfCentralDirectory = NULL;
		}
	}

	/*! \brief Adds an entry at the specified index
		\param[in] EntryIndex_in : the index at which to add the entry
		\param[in] pEntry_in : the entry to add
		\return true if the entry was added; false otherwise
		\remarks any existing entry for the same index will be deleted
	*/
	bool CentralDirectory::AddEntry(unsigned long EntryIndex_in, CentralDirectoryEntry *pEntry_in)
	{
		if (pEntry_in != NULL)
		{
			CentralDirectoryEntries::const_iterator EntryIt = m_Entries.find(EntryIndex_in);

			if (EntryIt != m_Entries.cend())
				delete EntryIt->second;

			m_Entries[EntryIndex_in] = pEntry_in;

			return true;
		}

		return false;
	}

	/*! \brief Removes and deletes the entry at the specified index
		\param[in] EntryIndex_in : the index at which to remove an entry
		\return true if the entry was removed; false otherwise
	*/
	bool CentralDirectory::RemoveEntry(unsigned long EntryIndex_in)
	{
		CentralDirectoryEntries::const_iterator EntryIt = m_Entries.find(EntryIndex_in);

		if (EntryIt != m_Entries.cend())
		{
			delete EntryIt->second;
			m_Entries.erase(EntryIt);

			return true;
		}

		return false;
	}

	/*! \brief Sets the end of central directory record
		\param[in] pEndOfCentralDirectory_in : the new end of central directory record
	*/
	void CentralDirectory::SetEndOfCentralDirectory(EndOfCentralDirectory *pEndOfCentralDirectory_in)
	{
		if (m_pEndOfCentralDirectory != NULL)
			delete m_pEndOfCentralDirectory;

		m_pEndOfCentralDirectory = pEndOfCentralDirectory_in;
	}

	/*! \brief Retrieves the number of central directory entries
		\return the number of central directory entries
	*/
	unsigned long CentralDirectory::GetEntryCount() const
	{ return m_Entries.size(); }

	/*! \brief Retrieves the map of central directory entries
		\return the map of central directory entries
	*/
	CentralDirectoryEntries& CentralDirectory::GetEntries()
	{ return m_Entries; }
}