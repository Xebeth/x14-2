/**************************************************************************
	created		:	2013-04-24
	filename	: 	BaseEntry.cpp
	author		:	Xebeth`
	copyright	:	North Edge (2013)
	purpose		:	Base entry object
**************************************************************************/
#include "stdafx.h"

#include "BaseEntry.h"

namespace ZipArchiveLib
{
	//! \brief BaseEntry default constructor
	BaseEntry::BaseEntry() : m_OffsetInFile(0UL),
		m_EntrySize(0UL), m_EntryIndex(0UL) {}

	//! \brief BaseEntry destructor
	BaseEntry::~BaseEntry() {}

	/*! \brief Sets the offset in the ZIP file
		\param[in] OffsetInFile_in : the offset in the ZIP file
	*/
	void BaseEntry::SetOffsetInFile(unsigned long OffsetInFile_in)
	{ m_OffsetInFile = OffsetInFile_in; }

	/*! \brief Retrieves the offset of the entry in the ZIP file
		\return the offset of the entry in the ZIP file
	*/
	unsigned long BaseEntry::GetOffsetInFile() const
	{ return m_OffsetInFile; }

	/*! \brief Sets the index of the entry
		\param[in] EntryIndex_in : the index of the entry
	*/
	void BaseEntry::SetEntryIndex(unsigned long EntryIndex_in)
	{ m_EntryIndex = EntryIndex_in; }

	/*! \brief Retrieves the index of the entry
		\return the index of the entry
	*/
	unsigned long BaseEntry::GetEntryIndex() const
	{ return m_EntryIndex; }

	/*! \brief Sets the size of the entry
		\param[in] EntrySize_in : the size of the entry
	*/
	void BaseEntry::SetEntrySize(unsigned long EntrySize_in)
	{ m_EntrySize = EntrySize_in; }

	/*! \brief Retrieves the size of the entry
		\return the size of the entry
	*/
	unsigned long BaseEntry::GetEntrySize() const
	{ return m_EntrySize; }
}