/**************************************************************************
	created		:	2013-04-25
	filename	: 	ZipDataParser.h
	author		:	Xebeth`
	copyright	:	North Edge (2013)
	purpose		:	ZIP data parser
**************************************************************************/
#ifndef __ZIP_PARSER_H__
#define __ZIP_PARSER_H__

namespace ZipArchiveLib
{
	// forward declaration
	class EndOfCentralDirectory;
	class CentralDirectoryEntry;
	class ZipFileEntry;
	class IZipData;

	typedef std::map<unsigned long, CentralDirectoryEntry*> CentralDirEntryMap;
	typedef std::map<unsigned long, ZipFileEntry*> FileEntryMap;

	class ZipDataParser
	{
		enum eParserError
		{
			PARSE_NO_ERROR = 0U,
			PARSE_ERROR_NO_DATA,
			PARSER_ERROR_NO_BUFFER,
			PARSE_ERROR_INCOHERENT_DATA,
			PARSE_ERROR_ENTRY_COUNT_MISMATCH,
			PARSE_ERROR_OUT_OF_MEMORY,
			PARSE_ERROR_COUNT
		};

	public:
		enum eEntryType
		{
			ENTRY_TYPE_NONE = 0U,
			LOCAL_FILE_HEADER,
			CENTRAL_DIRECTORY,
			END_OF_CENTRAL_DIRECTORY,
			ENTRY_TYPE_COUNT
		};

		explicit ZipDataParser(IZipData *pZipData_in);

		eEntryType GetCurrentType();
		bool IsValid() const;
		bool Initialize();

		bool ReadEndOfCentralDirectory(EndOfCentralDirectory &EndOfCentralDir_in_out);
		bool ReadCentralDirectoryEntry(CentralDirectoryEntry &CentralDir_in_out);
		bool ReadFileEntry(ZipFileEntry &FileEntry_in_out);

		// common
		template<typename EntryT> bool ReadExtraData(EntryT &Entry_in_out);
		template<typename EntryT> bool ReadFilename(EntryT &Entry_in_out);
		// local file entry
		bool ReadEncryptionHeader(ZipFileEntry &FileEntry_in_out);
		bool ReadLocalFileHeader(ZipFileEntry &FileEntry_in_out);
		bool ReadCompressedData(ZipFileEntry &FileEntry_in_out);
		bool ReadDataDescriptor(ZipFileEntry &FileEntry_in_out);
		// central directory entry
		bool ReadCentralDirectoryHeader(CentralDirectoryEntry &CentralDir_in_out);
		bool ReadFileComment(CentralDirectoryEntry &CentralDir_in_out);
		// end of central directory
		bool ReadEndOfCentralDirectoryHeader(EndOfCentralDirectory &CentralDir_in_out);
		bool ReadZipComment(EndOfCentralDirectory &EndOfCentralDir_in_out);

	private:
		template<typename T> bool CopyBufferFromZipData(T **pBuffer_in_out,
														   unsigned long AllocCount_in,
														   unsigned long ReadSize_in);
		bool ReadZipData(void *pDest_in_out, unsigned long DataToRead_in);
		bool Fail(eParserError Error_in);
		
		//! pointer to a ZIP data container
		IZipData *m_pZipData;
		//! last error code
		eParserError m_ErrorCode;
		//! expected type of entries
		eEntryType m_EntryType;
		//! current header signature
		unsigned long m_Signature;
		//! current position
		unsigned long m_Position;
	};
}

#endif//__ZIP_PARSER_H__
