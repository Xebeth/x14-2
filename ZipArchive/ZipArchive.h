/**************************************************************************
	created		:	2013-04-24
	filename	: 	ZipArchive.h
	author		:	Xebeth`
	copyright	:	North Edge (2013)
	purpose		:	
**************************************************************************/
#ifndef __ZIP_ARCHIVE_H__
#define __ZIP_ARCHIVE_H__

#ifndef _ZIP_ARCHIVE_SDK_INCLUDE_
	#error Please include the global header file 'ZipArchiveLib.h'
#endif

namespace ZipArchiveLib
{
	// forward declarations
	class CentralDirectoryEntry;
	class CentralDirectory;
	class IDataCompression;
	class ZipFileEntry;
	class IZipData;

	typedef std::map<unsigned long, CentralDirectoryEntry*> CentralDirEntryMap;
	typedef std::map<unsigned long, ZipFileEntry*> FileEntryMap;
	typedef std::map<int, IDataCompression*> CompressorMap;

	class ZipArchive
	{
	public:
		ZipArchive(const unsigned char *pBuffer_in, unsigned long BufferSize_in);
		ZipArchive(const TCHAR *pFileName_in, const TCHAR *pOpenMode_in);
		virtual ~ZipArchive();

		unsigned long Extract(const std::string &FileMask_in, const std::string &Path_in,
							  std::list<char*> *pExtractedFileList_in_out = NULL);

		bool IsFilenameMatching(const std::string &Filename_in,
								const std::string &Mask_in) const;

	private:
		IDataCompression* FindCompressor(int CompressedMethod_in);
		IDataCompression* GetCompressor(int CompressedMethod_in);
		bool ValidateEntries(FileEntryMap &FileEntries_in_out);
		bool Uncompress(ZipFileEntry *pFileEntry_in_out);
		bool ParseZipData();
		//! a map of compressors (key: compression method, value: IDataCompressor*)
		CompressorMap m_CompressorMap;
		//! the archive central directory
		CentralDirectory *m_pDirectory;
		//! ZIP data container
		IZipData *m_pZipData;
	};
}

#endif//__ZIP_ARCHIVE_H__
