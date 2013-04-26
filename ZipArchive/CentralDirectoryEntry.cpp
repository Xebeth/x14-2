/**************************************************************************
	created		:	2013-04-24
	filename	: 	CentralDirectoryEntry.cpp
	author		:	Xebeth`
	copyright	:	North Edge (2013)
	purpose		:	
**************************************************************************/
#include "stdafx.h"

#include "BaseEntry.h"
#include "ZipFileEntry.h"
#include "CentralDirectoryEntry.h"
#include "ZipTypes.h"

namespace ZipArchiveLib
{
	//! \brief CentralDirectoryEntry default constructor
	CentralDirectoryEntry::CentralDirectoryEntry()
		: m_pFileEntry(NULL), m_pCentralDirectoryHeader(NULL), m_pExtraData(NULL)
	{
		CreateHeader();
	}
	
	//! \brief CentralDirectoryEntry destructor
	CentralDirectoryEntry::~CentralDirectoryEntry()
	{
		if (m_pCentralDirectoryHeader != NULL)
		{
			delete m_pCentralDirectoryHeader;
			m_pCentralDirectoryHeader = NULL;
		}

		if (m_pExtraData != NULL)
		{
			free(m_pExtraData);
			m_pExtraData = NULL;
		}

		if (m_pFileEntry != NULL)
		{
			delete m_pFileEntry;
			m_pFileEntry = NULL;
		}
	}

	//! \brief Creates the central directory header structure
	void CentralDirectoryEntry::CreateHeader()
	{
		if (m_pCentralDirectoryHeader != NULL)
			delete m_pCentralDirectoryHeader;

		m_pCentralDirectoryHeader = new CentralDirectoryHeader;

		if (m_pCentralDirectoryHeader != NULL)
			SetDefaults(*m_pCentralDirectoryHeader);
	}

	/*! \brief Sets default values for the central directory header
		\param[in,out] Header_in_out : the central directory header to initialize
	*/
	void CentralDirectoryEntry::SetDefaults(CentralDirectoryHeader &Header_in_out) const
	{
		memset(&Header_in_out, 0, sizeof(CentralDirectoryHeader));

		// set the header signature
		Header_in_out.HeaderSignature = CENTRAL_DIRECTORY_SIGNATURE;
		// default to the MSDOS version
		Header_in_out.VersionMadeBy = VERSION_BY_MSDOS;
		// default to deflate version (zLib)
		Header_in_out.VersionNeededToExtract = VERSION_NEEDED_DEFLATE;
		// default to deflate method (zLib)
		Header_in_out.CompressionMethod = COMPRESSION_DEFLATED;
		// default to maximum compression for deflate
		Header_in_out.BitFlag = FLAG_DEFLATING_MAXIMUM;
	}

	/*! \brief Sets the central directory header of the entry
		\param[in] CentralDirHeader_in : the new central directory header
		\param[in] Position_in : the position of the header in the ZIP file
	*/
	void CentralDirectoryEntry::SetCentralDirectoryHeader(const CentralDirectoryHeader &CentralDirHeader_in, unsigned long Position_in)
	{
		if (m_pCentralDirectoryHeader != NULL)
			delete m_pCentralDirectoryHeader;

		m_pCentralDirectoryHeader = new CentralDirectoryHeader(CentralDirHeader_in);
		m_OffsetInFile = Position_in;
	}

	/*! \brief Sets the filename of the entry
		\param[in] pFilename_in : the new filename
	*/
	void CentralDirectoryEntry::SetFilename(const char *pFilename_in)
	{
		if (pFilename_in != NULL)
			m_Filename.assign(pFilename_in);
	}
	
	/*! \brief Sets the file extra data of the entry
		\param[in] pExtraData_in : the new extra data
	*/
	void CentralDirectoryEntry::SetExtraData(unsigned char *pExtraData_in)
	{
		if (m_pExtraData != NULL)
			free(m_pExtraData);

		m_pExtraData = pExtraData_in;
	}
	
	/*! \brief Sets the file comment of the entry
		\param[in] pFileComment_in : the new file comment
	*/
	void CentralDirectoryEntry::SetFileComment(const char * pFileComment_in)
	{
		if (pFileComment_in != NULL)
			m_FileComment.assign(pFileComment_in);
	}

	/*! \brief Retrieves the length of the file comment
		\return the length of the file comment 
		\remarks the null terminator is excluded
	*/
	unsigned long CentralDirectoryEntry::GetFileCommentLength() const
	{
		if (m_pCentralDirectoryHeader != NULL)
			return m_pCentralDirectoryHeader->CommentLength;

		return 0UL;
	}

	/*! \brief Retrieves the length of the filename
		\return the length of the filename 
		\remarks the null terminator is excluded
	*/
	unsigned long CentralDirectoryEntry::GetFilenameLength() const
	{
		if (m_pCentralDirectoryHeader != NULL)
			return m_pCentralDirectoryHeader->FilenameLength;

		return 0UL;
	}

	/*! \brief Retrieves the size of the extra data
		\return the size of the extra data
	*/
	unsigned long CentralDirectoryEntry::GetExtraDataSize() const
	{
		if (m_pCentralDirectoryHeader != NULL)
			return m_pCentralDirectoryHeader->ExtraFieldLength;

		return 0UL;
	}

	/*! \brief Retrieves the CRC of the entry
		\return the CRC of the entry
	*/
	unsigned int CentralDirectoryEntry::GetCRC() const
	{
		if (m_pCentralDirectoryHeader != NULL)
			return m_pCentralDirectoryHeader->CRC32;
		
		return 0U;
	}

	/*! \brief Retrieves the filename of the entry
		\return the filename of the entry
	*/
	const std::string& CentralDirectoryEntry::GetFilename() const
	{ return m_Filename; }

	
	/*! \brief Retrieves the file entry of the central directory
		\return the file entry of the central directory
	*/
	ZipFileEntry* CentralDirectoryEntry::GetFileEntry() const
	{ return m_pFileEntry; }

	/*! \brief Sets the matching file entry of the central directory
		\param[in] pFileEntry_in : the new file entry
	*/
	void CentralDirectoryEntry::SetFileEntry(ZipFileEntry *pFileEntry_in)
	{
		if (m_pFileEntry != NULL)
			delete m_pFileEntry;

		m_pFileEntry = pFileEntry_in;
	}
}