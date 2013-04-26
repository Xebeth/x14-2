/**************************************************************************
	created		:	2013-04-25
	filename	: 	EndOfCentralDirectory.cpp
	author		:	Xebeth`
	copyright	:	North Edge (2013)
	purpose		:	
**************************************************************************/
#include "stdafx.h"

#include "BaseEntry.h"
#include "EndOfCentralDirectory.h"
#include "ZipTypes.h"

namespace ZipArchiveLib
{
	//! \brief EndOfCentralDirectory default constructor
	EndOfCentralDirectory::EndOfCentralDirectory()
		: m_pEndOfCentralDirHeader(NULL), m_pZip64ECDLocator(NULL),
		  m_pZip64ECDv1(NULL), m_pZip64ExtensibleData(NULL),
		  m_pZip64ECDv2(NULL), m_pZip64HashData(NULL)
	{
		CreateHeader();
	}

	//! \brief EndOfCentralDirectory destructor
	EndOfCentralDirectory::~EndOfCentralDirectory()
	{
		if (m_pEndOfCentralDirHeader != NULL)
		{
			delete m_pEndOfCentralDirHeader;
			m_pEndOfCentralDirHeader = NULL;
		}

		if (m_pZip64ECDLocator != NULL)
		{
			delete m_pZip64ECDLocator;
			m_pZip64ECDLocator = NULL;
		}

		if (m_pZip64ECDv1 != NULL)
		{
			delete m_pZip64ECDv1;
			m_pZip64ECDv1 = NULL;
		}

		if (m_pZip64ExtensibleData != NULL)
		{
			delete m_pZip64ExtensibleData;
			m_pZip64ExtensibleData = NULL;
		}

		if (m_pZip64ECDv2 != NULL)
		{
			delete m_pZip64ECDv2;
			m_pZip64ECDv2 = NULL;
		}

		if (m_pZip64HashData != NULL)
		{
			delete m_pZip64HashData;
			m_pZip64HashData = NULL;
		}
	}

	
	//! \brief Creates the central directory header structure
	void EndOfCentralDirectory::CreateHeader()
	{
		if (m_pEndOfCentralDirHeader != NULL)
			delete m_pEndOfCentralDirHeader;

		m_pEndOfCentralDirHeader = new EndOfCentralDirectoryHeader;

		if (m_pEndOfCentralDirHeader != NULL)
			SetDefaults(*m_pEndOfCentralDirHeader);
	}
	
	/*! \brief Sets default values for the central directory header
		\param[in,out] Header_in_out : the central directory header to initialize
	*/
	void EndOfCentralDirectory::SetDefaults(EndOfCentralDirectoryHeader &Header_in_out) const
	{
		memset(&Header_in_out, 0, sizeof(EndOfCentralDirectoryHeader));

		// set the header signature
		Header_in_out.HeaderSignature = END_OF_CENTRAL_DIRECTORY_SIGNATURE;
	}

	/*! \brief Sets the end of central directory header
		\param[in] EndOfCentralDir_in : the new end of central directory header
		\param[in] Position_in : the position of the header in the ZIP file
	*/
	void EndOfCentralDirectory::SetEndOfCentralDirectory(const EndOfCentralDirectoryHeader &EndOfCentralDir_in,
														 unsigned long Position_in)
	{
		if (m_pEndOfCentralDirHeader != NULL)
			delete m_pEndOfCentralDirHeader;

		m_pEndOfCentralDirHeader = new EndOfCentralDirectoryHeader(EndOfCentralDir_in);
		m_OffsetInFile = Position_in;
	}

	/*! \brief Sets the global zip comment
		\param[in] pZipComment_in : the new global zip comment
	*/
	void EndOfCentralDirectory::SetZipComment(const char *pZipComment_in)
	{
		if (pZipComment_in != NULL)
			m_ZipComment.assign(pZipComment_in);
	}

	/*! \brief Retrieves the length of the zip comment
		\return the length of the zip comment 
		\remarks the null terminator is excluded
	*/
	unsigned long EndOfCentralDirectory::GetZipCommentLength() const
	{
		if (m_pEndOfCentralDirHeader != NULL)
			return m_pEndOfCentralDirHeader->ZipCommentLength;

		return 0UL;
	}
}