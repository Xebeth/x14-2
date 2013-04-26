/**************************************************************************
	created		:	2013-04-25
	filename	: 	ZipDataParser.cpp
	author		:	Xebeth`
	copyright	:	North Edge (2013)
	purpose		:	ZIP data parser
**************************************************************************/
#include "stdafx.h"

#include "ZipDataParser.h"

#include "IZipData.h"
#include "ZipTypes.h"
#include "BaseEntry.h"
#include "ZipFileEntry.h"
#include "CentralDirectoryEntry.h"
#include "EndOfCentralDirectory.h"

namespace ZipArchiveLib
{
	/*! \brief ZipDataParser constructor
		\param[in] pZipData_in : a pointer to a ZIP data container
	*/
	ZipDataParser::ZipDataParser(IZipData *pZipData_in)
		: m_pZipData(pZipData_in), m_Signature(0UL), m_Position(0UL),
		  m_ErrorCode(PARSE_NO_ERROR), m_EntryType(ENTRY_TYPE_NONE) {}

	/*! \brief Initializes the parser
		\return true if successful; false otherwise
	*/
	bool ZipDataParser::Initialize()
	{
		m_Position = 0UL;

		if (m_pZipData != NULL)
		{
			// read the first 4 bytes of the data
			m_Signature = m_pZipData->MoveTo(m_Position, 0UL);

			switch(GetCurrentType())
			{
				// normal case
				case LOCAL_FILE_HEADER:
				// empty zip
				case CENTRAL_DIRECTORY_SIGNATURE:
					return true;
				break;
				// local file header / central directory count mismatch
				case END_OF_CENTRAL_DIRECTORY:
					return Fail(PARSE_ERROR_ENTRY_COUNT_MISMATCH);
				break;
				// unknown error cases
				default:
					return Fail(PARSE_ERROR_INCOHERENT_DATA);
				break;
			}
		}

		// empty ZIP data container
		return Fail(PARSE_ERROR_NO_DATA);
	}

	/*! \brief Read a complete file entry from the ZIP data
		\param[out] FileEntry_in_out : the file entry receiving the data
		\return true if the data was read correctly; false otherwise
	*/
	bool ZipDataParser::ReadFileEntry(ZipFileEntry &FileEntry_in_out)
	{
		if (m_pZipData != NULL)
		{
			if (m_EntryType == LOCAL_FILE_HEADER
			 && m_Signature == LOCAL_FILE_HEADER_SIGNATURE)
			{
				return (ReadLocalFileHeader	(FileEntry_in_out)		// read the local file header
					 && ReadFilename		(FileEntry_in_out)		// read the filename
					 && ReadExtraData		(FileEntry_in_out)		// read the extra data
					 && ReadEncryptionHeader(FileEntry_in_out)		// read the encryption header
					 && ReadCompressedData	(FileEntry_in_out)		// read the compressed data buffer
					 && ReadDataDescriptor	(FileEntry_in_out));	// read the data descriptor
			}
			else
				return Fail(PARSE_ERROR_INCOHERENT_DATA);
		}

		// empty ZIP data container
		return Fail(PARSE_ERROR_NO_DATA);
	}

	/*! \brief Reads a local file header structure from the ZIP data
		\param[out] FileEntry_in_out : the file entry receiving the data
		\return true if the data was read correctly; false otherwise
	*/
	bool ZipDataParser::ReadLocalFileHeader(ZipFileEntry &FileEntry_in_out)
	{
		unsigned long Position = m_Position;
		LocalFileHeader FileHeader;

		if (ReadZipData(&FileHeader, sizeof(FileHeader)))
		{
			FileEntry_in_out.SetLocalFileHeader(FileHeader, Position);

			return true;
		}

		return false;
	}

	/*! \brief Reads a data descriptor from the ZIP data
		\param[out] FileEntry_in_out : the file entry receiving the data
		\return true if the data was read correctly; false otherwise
	*/
	bool ZipDataParser::ReadDataDescriptor(ZipFileEntry &FileEntry_in_out)
	{
		bool bHasDescriptor = FileEntry_in_out.HasDataDescriptor();

		if (bHasDescriptor)
		{
			if (m_pZipData != NULL)
			{
				unsigned long DataToRead = 0UL, DataRead = 0UL;
				DataDescriptorSigned DataDesc;
				void *pDestAddr = NULL;

				// check for a signed data descriptor
				if (m_Signature == DATA_DESCRIPTOR_SIGNATURE)
				{
					DataToRead = sizeof(DataDesc);
					pDestAddr = &DataDesc;
				}
				else
				{
					DataDesc.HeaderSignature = DATA_DESCRIPTOR_SIGNATURE;
					DataToRead = sizeof(DataDescriptor);
					pDestAddr = (&DataDesc + 4);
				}
				// read the data descriptor
				if (ReadZipData(&DataDesc, DataToRead))
				{
					FileEntry_in_out.SetDataDescriptor(DataDesc);

					return true;
				}
			}
		}

		// no descriptor => nothing to do
		return (bHasDescriptor == false);
	}

	/*! \brief Reads the extra data from the ZIP data
		\param[out] Entry_in_out : the entry receiving the data
		\return true if the data was read correctly; false otherwise
	*/
	template<typename EntryT> bool ZipDataParser::ReadExtraData(EntryT &Entry_in_out)
	{
		unsigned long Size = Entry_in_out.GetExtraDataSize();

		if (Size > 0UL)
		{
			unsigned char *pExtraData = NULL;

			if (CopyBufferFromZipData(&pExtraData, Size / sizeof(unsigned char), Size))
			{
				Entry_in_out.SetExtraData(pExtraData);

				return true;
			}

			return false;
		}

		// empty extra data is not an error
		return (Size == 0UL);
	}

	/*! \brief Reads the compressed data from the ZIP data
		\param[out] FileEntry_in_out : the file entry receiving the data
		\return true if the data was read correctly; false otherwise
	*/
	bool ZipDataParser::ReadCompressedData(ZipFileEntry &FileEntry_in_out)
	{
		unsigned long Size = FileEntry_in_out.GetCompressedDataSize();

		if (Size > 0UL)
		{
			unsigned char *pCompressedData = NULL;

			if (CopyBufferFromZipData(&pCompressedData, Size / sizeof(unsigned char), Size))
			{
				FileEntry_in_out.SetCompressedData(pCompressedData);

				return true;
			}

			return false;
		}

		return false;
	}

	/*! \brief Reads a filename from the ZIP data
		\param[out] Entry_in_out : the entry receiving the data
		\return true if the data was read correctly; false otherwise
	*/
	template<typename EntryT> bool ZipDataParser::ReadFilename(EntryT &Entry_in_out)
	{
		unsigned long Length = Entry_in_out.GetFilenameLength();

		if (Length > 0UL)
		{
			char *pFilename = NULL;

			if (CopyBufferFromZipData(&pFilename, Length + 1, Length * sizeof(char)))
			{
				Entry_in_out.SetFilename(pFilename);
				// cleanup (stored as a std::string in the entry)
				free(pFilename);

				return true;
			}

			return false;
		}

		// empty filename is not an error
		return (Length == 0UL);
	}

	/*! \brief Reads an encryption header from the ZIP data
		\param[out] FileEntry_in_out : the file entry receiving the data
		\return true if the data was read correctly; false otherwise
	*/
	bool ZipDataParser::ReadEncryptionHeader(ZipFileEntry &FileEntry_in_out)
	{
		bool bEncrypted = FileEntry_in_out.IsEncrypted();

		if (bEncrypted)
		{
			FileEncryptionHeader EncryptionHeader;

			if (ReadZipData(&EncryptionHeader, sizeof(EncryptionHeader)))
			{
				FileEntry_in_out.SetEncryptionHeader(EncryptionHeader);

				return true;
			}

			return false;
		}

		// no encryption => nothing to do
		return (bEncrypted == false);
	}

	/*! \brief Reads a central directory entry from the ZIP data
		\param[out] CentralDir_in_out : the central directory entry receiving the data
		\return true if the data was read correctly; false otherwise
	*/
	bool ZipDataParser::ReadCentralDirectoryEntry(CentralDirectoryEntry &CentralDir_in_out)
	{
		if (m_pZipData != NULL)
		{
			if (m_EntryType == CENTRAL_DIRECTORY
				&& m_Signature == CENTRAL_DIRECTORY_SIGNATURE)
			{
				// @TODO ZIP64 support
				return (ReadCentralDirectoryHeader	(CentralDir_in_out)	// read the central directory header
					 && ReadFilename				(CentralDir_in_out)	// read the filename
					 && ReadExtraData				(CentralDir_in_out)	// read the extra data
					 && ReadFileComment				(CentralDir_in_out));	// read the file comment
			}
			else
				return Fail(PARSE_ERROR_INCOHERENT_DATA);
		}

		// empty ZIP data container
		return Fail(PARSE_ERROR_NO_DATA);
	}

	/*! \brief Reads a central directory header structure from the ZIP data
		\param[out] CentralDir_in_out : the central directory receiving the data
		\return true if the data was read correctly; false otherwise
	*/
	bool ZipDataParser::ReadCentralDirectoryHeader(CentralDirectoryEntry &CentralDir_in_out)
	{
		CentralDirectoryHeader CentralDirHeader;
		unsigned long Position = m_Position;		

		if (ReadZipData(&CentralDirHeader, sizeof(CentralDirHeader)))
		{
			CentralDir_in_out.SetCentralDirectoryHeader(CentralDirHeader, Position);

			return true;
		}

		return false;
	}

	/*! \brief Reads a file comment from the ZIP data
		\param[out] FileEntry_out : the file entry receiving the data
		\return true if the data was read correctly; false otherwise
	*/
	bool ZipDataParser::ReadFileComment(CentralDirectoryEntry &CentralDir_in_out)
	{
		unsigned long Length = CentralDir_in_out.GetFileCommentLength();

		if (Length > 0UL)
		{
			char *pFilename = NULL;

			if (CopyBufferFromZipData(&pFilename, Length + 1, Length * sizeof(char)))
			{
				CentralDir_in_out.SetFileComment(pFilename);
				// cleanup (stored as a std::string in the entry)
				free(pFilename);

				return true;
			}

			return false;
		}

		// empty comment is not an error
		return (Length == 0UL);
	}

	/*! \brief Reads the central directory header structure from the ZIP data
		\param[out] EndOfCentralDir_in_out : the end of central directory entry receiving the data
		\return true if the data was read correctly; false otherwise
	*/
	bool ZipDataParser::ReadEndOfCentralDirectory(EndOfCentralDirectory &EndOfCentralDir_in_out)
	{
		if (m_pZipData != NULL)
		{
			if (m_EntryType == END_OF_CENTRAL_DIRECTORY
				&& m_Signature == END_OF_CENTRAL_DIRECTORY_SIGNATURE)
			{
				// @TODO ZIP64 support and encryption
				return (ReadEndOfCentralDirectoryHeader	(EndOfCentralDir_in_out)	// read the end of central directory header
					 && ReadZipComment					(EndOfCentralDir_in_out));	// read the file comment
			}
			else
				return Fail(PARSE_ERROR_INCOHERENT_DATA);
		}

		// empty ZIP data container
		return Fail(PARSE_ERROR_NO_DATA);
	}

	/*! \brief Reads the end of central directory header structure from the ZIP data
		\param[out] CentralDir_in_out : the central directory receiving the data
		\return true if the data was read correctly; false otherwise
	*/
	bool ZipDataParser::ReadEndOfCentralDirectoryHeader(EndOfCentralDirectory &CentralDir_in_out)
	{
		EndOfCentralDirectoryHeader CentralDirHeader;
		unsigned long Position = m_Position;		

		if (ReadZipData(&CentralDirHeader, sizeof(CentralDirHeader)))
		{
			CentralDir_in_out.SetEndOfCentralDirectory(CentralDirHeader, Position);

			return true;
		}

		return false;
	}

	/*! \brief Reads the zip comment from the ZIP data
		\param[out] EndOfCentralDir_in_out : the end of central directory entry receiving the data
		\return true if the data was read correctly; false otherwise
	*/
	bool ZipDataParser::ReadZipComment(EndOfCentralDirectory &EndOfCentralDir_in_out)
	{
		unsigned long Length = EndOfCentralDir_in_out.GetZipCommentLength();

		if (Length > 0UL)
		{
			char *pZipComment = NULL;

			if (CopyBufferFromZipData(&pZipComment, Length + 1, Length * sizeof(char)))
			{
				EndOfCentralDir_in_out.SetZipComment(pZipComment);
				// cleanup (stored as a std::string in the entry)
				free(pZipComment);

				return true;
			}

			return false;
		}

		// empty comment is not an error
		return (Length == 0UL);
	}

	/*! \brief Read data from the ZIP data and move the cursor
		\param[in] pDest_in_out : the destination buffer
		\param[in] DataToRead_in : the size of the data to be read in bytes
	*/
	bool ZipDataParser::ReadZipData(void *pDest_in_out, unsigned long DataToRead_in)
	{
		if (m_pZipData != NULL)
		{
			if (pDest_in_out != NULL)
			{
				unsigned long DataRead = m_pZipData->CopyData(pDest_in_out, DataToRead_in);

				if (DataRead == DataToRead_in)
				{
					// move after the copied data
					m_Signature = m_pZipData->MoveTo(m_Position, DataRead);

					return true;
				}
			}

			// incoherent data
			return Fail(PARSE_ERROR_INCOHERENT_DATA);
		}

		// empty ZIP data container
		return Fail(PARSE_ERROR_NO_DATA);
	}

	/*! \brief Reads and allocates a raw buffer from the ZIP data and move the cursor
		\param[in] pBuffer_in_out : the buffer receiving the data
		\param[in] AllocCount_in : the number of items to allocate (not bytes)
		\param[in] AllocSize_in : the amount of data to read in bytes
	*/
	template<typename T> bool ZipDataParser::CopyBufferFromZipData(T **pBuffer_in_out,
																	  unsigned long AllocCount_in,
																	  unsigned long ReadSize_in)
	{
		if (m_pZipData != NULL)
		{
			if (pBuffer_in_out != NULL)
			{
				if (Buffer::Realloc(pBuffer_in_out, AllocCount_in, true))
				{
					if (ReadZipData(*pBuffer_in_out, ReadSize_in) == false)
					{
						free(*pBuffer_in_out);
						*pBuffer_in_out = NULL;

						return false;
					}
					else
						return true;
				}
				else
					return Fail(PARSE_ERROR_OUT_OF_MEMORY);
			}
			else
				return Fail(PARSER_ERROR_NO_BUFFER);
		}

		// empty ZIP data container
		return Fail(PARSE_ERROR_NO_DATA);
	}

	/*! \brief Sets the parser error code to the specified value
		\param[in] Error_in : the parser error code
		\return 0
	*/
	bool ZipDataParser::Fail(eParserError Error_in)
	{
		m_ErrorCode = Error_in;

		return false;
	}

	/*! \brief Retrieves the type of entry currently expected by the parser
		\return the type of entry currently expected by the parser
	*/
	ZipDataParser::eEntryType ZipDataParser::GetCurrentType()
	{ 
		switch(m_Signature)
		{
			case LOCAL_FILE_HEADER_SIGNATURE:
				m_EntryType = LOCAL_FILE_HEADER;
			break;
			case CENTRAL_DIRECTORY_SIGNATURE:
				m_EntryType = CENTRAL_DIRECTORY;
			break;
			case END_OF_CENTRAL_DIRECTORY_SIGNATURE:
				m_EntryType = END_OF_CENTRAL_DIRECTORY;
			break;
			default:
				m_ErrorCode = PARSE_ERROR_INCOHERENT_DATA;
				m_EntryType = ENTRY_TYPE_NONE;
			break;
		}

		return m_EntryType;
	}

	/*! \brief Checks if the parser has encountered an error
		\return true if the parser has no error; false otherwise
	*/
	bool ZipDataParser::IsValid() const
	{ return (m_ErrorCode == PARSE_NO_ERROR); }
}