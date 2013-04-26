/**************************************************************************
	created		:	2013-04-24
	filename	: 	ZipFileEntry.cpp
	author		:	Xebeth`
	copyright	:	North Edge (2013)
	purpose		:	Zip file entry
**************************************************************************/
#include "stdafx.h"

#include "BaseEntry.h"
#include "ZipFileEntry.h"
#include "ZipTypes.h"

namespace ZipArchiveLib
{
	//! \brief ZipFileEntry default constructor
	ZipFileEntry::ZipFileEntry()
		: m_pEncryptionHeader(NULL), m_pCompressedData(NULL),
		  m_pUncompressedData(NULL), m_pDataDescriptor(NULL),
		  m_pExtraData(NULL), m_pFileHeader(NULL)
	{
		CreateHeader();
	}

	//! \brief ZipFileEntry destructor
	ZipFileEntry::~ZipFileEntry()
	{
		if (m_pFileHeader != NULL)
		{
			delete m_pFileHeader;
			m_pFileHeader = NULL;
		}

		if (m_pExtraData != NULL)
		{
			free(m_pExtraData);
			m_pExtraData = NULL;
		}

		if (m_pEncryptionHeader != NULL)
		{
			delete m_pEncryptionHeader;
			m_pEncryptionHeader = NULL;
		}

		if (m_pDataDescriptor != NULL)
		{
			delete m_pDataDescriptor;
			m_pDataDescriptor = NULL;
		}

		if (m_pCompressedData != NULL)
		{
			free(m_pCompressedData);
			m_pCompressedData = NULL;
		}

		if (m_pUncompressedData != NULL)
		{
			free(m_pUncompressedData);
			m_pUncompressedData = NULL;
		}
	}

	//! \brief Creates the file header structure
	void ZipFileEntry::CreateHeader()
	{
		if (m_pFileHeader != NULL)
			delete m_pFileHeader;

		m_pFileHeader = new LocalFileHeader;

		if (m_pFileHeader != NULL)
			SetDefaults(*m_pFileHeader);
	}

	/*! \brief Sets default values for the local file header
		\param[in,out] Header_in_out : the local file header to initialize
	*/
	void ZipFileEntry::SetDefaults(LocalFileHeader &Header_in_out) const
	{
		memset(&Header_in_out, 0, sizeof(LocalFileHeader));

		// local file header signature
		Header_in_out.HeaderSignature = LOCAL_FILE_HEADER_SIGNATURE;
		// default to deflate version (zLib)
		Header_in_out.Version = VERSION_NEEDED_DEFLATE;
		// default to deflate method (zLib)
		Header_in_out.CompressionMethod = COMPRESSION_DEFLATED;
		// default to maximum compression for deflate
		Header_in_out.BitFlag = FLAG_DEFLATING_MAXIMUM;
	}

	/*! \brief Sets the local file header of the entry
		\param[in] FileHeader_in : the new local file header
		\param[in] Position_in : the position of the header in the ZIP data
	*/
	void ZipFileEntry::SetLocalFileHeader(const LocalFileHeader &FileHeader_in, unsigned long Position_in)
	{
		if (m_pFileHeader != NULL)
			delete m_pFileHeader;

		m_pFileHeader = new LocalFileHeader(FileHeader_in);
		m_OffsetInFile = Position_in;
	}

	/*! \brief Sets the filename of the entry
		\param[in] pFilename_in : the new filename
	*/
	void ZipFileEntry::SetFilename(const char *pFilename_in)
	{
		if (pFilename_in != NULL)
			m_Filename.assign(pFilename_in);
	}

	/*! \brief Sets the compressed data of the entry
		\param[in] pCompressedData_in : the new compressed data
	*/
	void ZipFileEntry::SetCompressedData(unsigned char *pCompressedData_in)
	{
		if (m_pCompressedData != NULL)
			free(m_pCompressedData);

		m_pCompressedData = pCompressedData_in;
	}

	/*! \brief Sets the data descriptor header of the entry
		\param[in] DataDescriptor_in : the new data descriptor
	*/
	void ZipFileEntry::SetDataDescriptor(const DataDescriptorSigned &DataDescriptor_in)
	{
		if (m_pDataDescriptor != NULL)
			delete m_pDataDescriptor;

		m_pDataDescriptor = new DataDescriptorSigned(DataDescriptor_in);
	}

	/*! \brief Sets the file encryption header of the entry
		\param[in] EncryptionHeader_in : the new encryption header
	*/
	void ZipFileEntry::SetEncryptionHeader(const FileEncryptionHeader &EncryptionHeader_in)
	{
		if (m_pEncryptionHeader != NULL)
			delete m_pEncryptionHeader;

		m_pEncryptionHeader = new FileEncryptionHeader(EncryptionHeader_in);
	}
	
	/*! \brief Sets the file extra data of the entry
		\param[in] pExtraData_in : the new extra data
	*/
	void ZipFileEntry::SetExtraData(unsigned char *pExtraData_in)
	{
		if (m_pExtraData != NULL)
			free(m_pExtraData);

		m_pExtraData = pExtraData_in;
	}

	/*! \brief Retrieves the length of the filename
		\return the length of the filename 
		\remarks the null terminator is excluded
	*/
	unsigned long ZipFileEntry::GetFilenameLength() const
	{
		if (m_pFileHeader != NULL)
			return m_pFileHeader->FilenameLength;

		return 0UL;
	}

	/*! \brief Retrieves the size of the extra data
		\return the size of the extra data
	*/
	unsigned long ZipFileEntry::GetExtraDataSize() const
	{
		if (m_pFileHeader != NULL)
			return m_pFileHeader->ExtraFieldLength;

		return 0UL;
	}

	/*! \brief Retrieves the size of the compressed data
		\return the size of the compressed data
	*/
	unsigned long ZipFileEntry::GetCompressedDataSize() const
	{
		if (m_pFileHeader != NULL)
			return m_pFileHeader->CompressedSize;

		return 0UL;
	}

	/*! \brief Retrieves the size of the uncompressed data
		\return the size of the uncompressed data
	*/
	unsigned long ZipFileEntry::GetUncompressedDataSize() const
	{
		if (m_pFileHeader != NULL)
			return m_pFileHeader->UncompressedSize;

		return 0UL;
	}

	/*! \brief Retrieves the CRC of the entry
		\return the CRC of the entry
	*/
	unsigned int ZipFileEntry::GetCRC() const
	{
		if (IsFlagSet(FLAG_USE_DATA_DESCRIPTORS)
			&& m_pDataDescriptor != NULL)
		{
			// the CRC from the local entry should be 0 => use the data descriptor
			return m_pDataDescriptor->CRC32;
		}
		else if (m_pFileHeader != NULL)
		{
			return m_pFileHeader->CRC32;
		}

		return 0U;
	}
	
	/*! \brief Retrieves the filename of the entry
		\return the filename of the entry
	*/
	const std::string& ZipFileEntry::GetFilename() const
	{ return m_Filename; }

	/*! \brief Checks if the specified flag is set
		\param[in] Flag_in : the flag to check
		\return true if the flag is set; false otherwise
	*/
	bool ZipFileEntry::IsFlagSet(int Flag_in) const
	{
		if (m_pFileHeader != NULL)
			return ((m_pFileHeader->BitFlag & Flag_in) == Flag_in);

		return false;
	}

	/*! \brief Checks if the file entry is encrypted
		\return true if the file entry is encrypted; false otherwise
	*/
	bool ZipFileEntry::IsEncrypted() const
	{
		return IsFlagSet(FLAG_ENCRYPTED_FILE);
	}

	/*! \brief Checks if the file entry has a data descriptor
		\return true if the file entry has a data descriptor; false otherwise
	*/
	bool ZipFileEntry::HasDataDescriptor() const
	{
		return IsFlagSet(FLAG_USE_DATA_DESCRIPTORS);
	}

	/*! \brief Retrieves the compression method for the file entry
		\return the compression method for the file entry
	*/
	int ZipFileEntry::GetCompressionMethod() const
	{
		if (m_pFileHeader != NULL)
			return m_pFileHeader->CompressionMethod;

		return -1;
	}

	/*! \brief Retrieves a pointer to the uncompressed data
		\return a pointer to the uncompressed data
	*/
	const unsigned char* ZipFileEntry::GetUncompressedData() const
	{
		if (m_pFileHeader != NULL)
		{
			if (m_pFileHeader->CompressionMethod == COMPRESSION_STORE)
				return m_pCompressedData;
			else
				return m_pUncompressedData;
		}

		return NULL;
	}

	/*! \brief Retrieves a pointer to the compressed data
		\return a pointer to the compressed data
	*/
	const unsigned char* ZipFileEntry::GetCompressedData() const
	{
		if (m_pFileHeader != NULL)
			return m_pCompressedData;
		
		return NULL;
	}

	/*! \brief Retrieves the address of the pointer to the uncompressed data
		\return the address of the pointer to the uncompressed data
	*/
	unsigned char** ZipFileEntry::GetUncompressedDataPtr()
	{
		if (m_pFileHeader != NULL && m_pFileHeader->CompressionMethod != COMPRESSION_STORE)
			return &m_pUncompressedData;

		return NULL;
	}

	/*! \brief Retrieves the address of the pointer to the compressed data
		\return the address of the pointer to the compressed data
	*/
	unsigned char** ZipFileEntry::GetCompressedDataPtr()
	{
		if (m_pFileHeader != NULL)
			return &m_pCompressedData;
		
		return NULL;
	}
}