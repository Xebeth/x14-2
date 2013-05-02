/**************************************************************************
	created		:	2013-04-24
	filename	: 	ZipArchive.cpp
	author		:	Xebeth`
	copyright	:	North Edge (2013)
	purpose		:	
**************************************************************************/
#include "stdafx.h"

#include "ZipArchive.h"

#include "ZipTypes.h"
#include "IZipData.h"
#include "BaseEntry.h"
#include "ZipFileEntry.h"
#include "ZipDataBuffer.h"
#include "ZipDataParser.h"
#include "CentralDirectory.h"
#include "CentralDirectoryEntry.h"
#include "EndOfCentralDirectory.h"

#include "IDataCompression.h"
#include "zLibCompression.h"

namespace ZipArchiveLib
{
	/*! \brief ZipArchive constructor using an external buffer
		\param[in] pBuffer_in : an external buffer containing ZIP data
		\param[in] BufferSize_in : the size of the buffer
	*/
	ZipArchive::ZipArchive(const unsigned char *pBuffer_in, unsigned long BufferSize_in)
		: m_pZipData(new ZipDataBuffer(pBuffer_in, BufferSize_in)),
		  m_pDirectory(new CentralDirectory) {}

	/*! \brief ZipArchive constructor using a file
		\param[in] pFileName_in : the path of the ZIP file
		\param[in] pOpenMode_in : the open mode
	*/
	ZipArchive::ZipArchive(const TCHAR *pFileName_in, const TCHAR *pOpenMode_in)
		: m_pZipData(NULL), m_pDirectory(new CentralDirectory) {}

	//! \brief ZipArchive destructor
	ZipArchive::~ZipArchive()
	{
		CompressorMap::const_iterator ComprIt, EndIt = m_CompressorMap.cend();

		for (ComprIt = m_CompressorMap.cbegin(); ComprIt != EndIt; ++ComprIt)
			delete ComprIt->second;

		m_CompressorMap.clear();

		if (m_pDirectory != NULL)
		{
			delete m_pDirectory;
			m_pDirectory = NULL;
		}

		if (m_pZipData != NULL)
		{
			delete(m_pZipData);
			m_pZipData = NULL;
		}
	}

	/*! \brief Parses the ZIP data
		\return the number of entries found
	*/
	bool ZipArchive::ParseZipData()
	{
		if (m_pZipData != NULL)
		{
			ZipDataParser Parser(m_pZipData);

			if (Parser.Initialize())
			{
				unsigned long FileEntryIndex = 0UL, CentralDirEntryIndex = 0UL;
				ZipDataParser::eEntryType EntryType = Parser.GetCurrentType();
				EndOfCentralDirectory *pEndOfCentralDir = NULL;
				CentralDirectoryEntry *pCentralDir = NULL;
				ZipFileEntry *pFileEntry = NULL;
				FileEntryMap FileEntries;

				while (Parser.IsValid()								// check for errors
					&& EntryType != ZipDataParser::ENTRY_TYPE_NONE)	// check for end
				{
					switch(EntryType)
					{
						// local file header
						case ZipDataParser::LOCAL_FILE_HEADER:
						{
							pFileEntry = new ZipFileEntry;

							if (Parser.ReadFileEntry(*pFileEntry))
								FileEntries[FileEntryIndex++] = pFileEntry;
							else
								delete pFileEntry;
						}
						break;
						// central directory header
						case ZipDataParser::CENTRAL_DIRECTORY:
						{
							pCentralDir = new CentralDirectoryEntry;

							if (Parser.ReadCentralDirectoryEntry(*pCentralDir))
								m_pDirectory->AddEntry(CentralDirEntryIndex++, pCentralDir);
							else
								delete pCentralDir;
						}							
						break;
						// end of central directory
						case ZipDataParser::END_OF_CENTRAL_DIRECTORY:
						{
							pEndOfCentralDir = new EndOfCentralDirectory;

							if (Parser.ReadEndOfCentralDirectory(*pEndOfCentralDir))
								m_pDirectory->SetEndOfCentralDirectory(pEndOfCentralDir);
							else
								delete pEndOfCentralDir;
						}
						break;
					}
					// get the next type of entry
					EntryType = Parser.GetCurrentType();
				}

				if (ValidateEntries(FileEntries))
				{
					// the source data is not needed anymore
					delete m_pZipData;
					m_pZipData = NULL;

					return true;
				}
			}
		}

		return false;
	}

	/*! \brief Validates the entries from the parsed ZIP data
		\param[in] FileEntries_in_out : the 
	*/
	bool ZipArchive::ValidateEntries(FileEntryMap &FileEntries_in_out)
	{
		if (m_pDirectory != NULL)
		{
			CentralDirectoryEntries &CentralDirEntries = m_pDirectory->GetEntries();

			if (FileEntries_in_out.size() == CentralDirEntries.size())
			{
				CentralDirectoryEntries::const_iterator DirEntryIt, DirEndIt = CentralDirEntries.cend();
				FileEntryMap::const_iterator FileEntryIt, FileEndIt = FileEntries_in_out.cend();
				CentralDirectoryEntry *pCentralDirEntry = NULL;
				std::string LocalFilename, CentralDirFilename;
				ZipFileEntry *pFileEntry = NULL;
				bool bMatch = false;

				for (DirEntryIt = CentralDirEntries.cbegin(); DirEntryIt != DirEndIt; ++DirEntryIt)
				{
					for (FileEntryIt = FileEntries_in_out.cbegin(); FileEntryIt != FileEndIt; )
					{
						pCentralDirEntry = DirEntryIt->second;
						pFileEntry = FileEntryIt->second;
						bMatch = false;

						if (pFileEntry != NULL && pCentralDirEntry != NULL
						 && pFileEntry->GetCRC() == pCentralDirEntry->GetCRC())
						{
							// compare the filenames last (most expansive comparison)
							CentralDirFilename = pCentralDirEntry->GetFilename();
							LocalFilename = pFileEntry->GetFilename();

							if (LocalFilename.compare(CentralDirFilename) == 0)
							{
								FileEntryIt = FileEntries_in_out.erase(FileEntryIt);
								pCentralDirEntry->SetFileEntry(pFileEntry);
								bMatch = true;
								break;
							}
						}
						// move the iterator forward
						++FileEntryIt;
					}
					// a match should have been found
					if (bMatch == false)
						break;
				}

				if (FileEntries_in_out.empty() == false)
				{
					// clean up the rest of the file entries
					while (FileEntries_in_out.empty() == false)
						delete FileEntries_in_out.cbegin()->second;
				}
				else
					return true;
			}
		}

		return false;
	}

	/*! \brief Extracts the file(s) represented by the mask to the specified path
		\param[in] FileMask_in : mask representing the files to extract
		\param[in] Path_in : the path where the files are to be extracted
		\param[in,out] pExtractedFileList_in_out : an optional list to receive the files extracted
												   the caller is responsible for freeing the strings
		\return the number of extracted files
		\remarks for now only *<name part> wildcards are supported
	*/
	unsigned long ZipArchive::Extract(const std::string &FileMask_in, const std::string &Path_in,
									  std::list<char*> *pExtractedFileList_in_out)
	{
		unsigned long FileCount = 0UL;

		if (m_pZipData == NULL || ParseZipData())
		{
			CentralDirectoryEntries &CentralDirEntries = m_pDirectory->GetEntries();
			CentralDirectoryEntries::const_iterator DirEntryIt, DirEndIt = CentralDirEntries.cend();
			CentralDirectoryEntry *pCentralDirEntry = NULL;
			std::string ExtractPath;

			// iterate through all the files to look for matches
			for (DirEntryIt = CentralDirEntries.cbegin(); DirEntryIt != DirEndIt; ++DirEntryIt)
			{
				pCentralDirEntry = DirEntryIt->second;

				if (pCentralDirEntry != NULL)
				{
					const std::string &Filename = pCentralDirEntry->GetFilename();

					if (IsFilenameMatching(Filename, FileMask_in)
					 && Uncompress(pCentralDirEntry->GetFileEntry()))
					{
						// construct the full extraction path

						// add the file to the list, if needed
						if (pExtractedFileList_in_out != NULL)
							pExtractedFileList_in_out->push_back(_strdup(ExtractPath.c_str()));

						++FileCount;
					}
				}
			}
		}

		return FileCount;
	}

	/*! \brief Checks if a filename matches the specified mask
		\param[in] Filename_in : the filename to check
		\param[in] Mask_in : the mask to check against
		\return true if the filename matches the mask; false otherwise
	*/
	bool ZipArchive::IsFilenameMatching(const std::string &Filename_in,
										const std::string &Mask_in) const
	{
		if (Mask_in.empty() == false && Filename_in.empty() == false)
		{
			if (Mask_in[0] == '*')
				return (Filename_in.find(Mask_in.c_str() + 1) != std::string::npos);
			else
				return (Filename_in.compare(Mask_in) == 0);			
		}

		return false;
	}

	/*! \brief Uncompress a file entry
		\param[in] pFileEntry_in_out : the file entry to uncompress
		\return true if the file entry was uncompressed; false otherwise
	*/
	bool ZipArchive::Uncompress(ZipFileEntry *pFileEntry_in_out)
	{
		if (pFileEntry_in_out != NULL)
		{
			int Method = pFileEntry_in_out->GetCompressionMethod();

			if (Method != COMPRESSION_STORE)
			{
				unsigned char **pUncompressedData = pFileEntry_in_out->GetUncompressedDataPtr();

				if (pUncompressedData != NULL && *pUncompressedData == NULL)
				{
					const unsigned char *pCompressedData = pFileEntry_in_out->GetCompressedData();

					if (pCompressedData != *pUncompressedData)
					{
						IDataCompression *pCompressor = GetCompressor(Method);

						if (pCompressor != NULL)
						{
							unsigned long UncompressedSize = pFileEntry_in_out->GetUncompressedDataSize();
							unsigned long CompressedSize = pFileEntry_in_out->GetCompressedDataSize();							

							return pCompressor->Uncompress(pCompressedData, CompressedSize,
														   pUncompressedData, UncompressedSize);
						}
					}
				}
			}

			return (Method == COMPRESSION_STORE);
		}

		return false;
	}

	/*! \brief Retrieves the appropriate compressor for the specified method
		\return a pointer to the compressor if the method is supported; NULL otherwise
		\remarks COMPRESSION_STORE is supported but doesn't require a compressor
	*/
	IDataCompression* ZipArchive::GetCompressor(int CompressionMethod_in)
	{
		IDataCompression *pResult = FindCompressor(CompressionMethod_in);

		if (pResult == NULL)
		{
			switch(CompressionMethod_in)
			{
				case COMPRESSION_DEFLATED:
					pResult = new zLibCompression;
				break;
			}
			// store the compressor in the map for re-use
			if (pResult != NULL)
				m_CompressorMap[CompressionMethod_in] = pResult;
		}

		return pResult;
	}

	/*! \brief Retrieves an existing compressor from the compressor map
		\param[in] CompressedMethod_in : the compression method
		\return a pointer to a compressor if found; NULL otherwise
	*/
	IDataCompression* ZipArchive::FindCompressor(int CompressedMethod_in)
	{
		CompressorMap::const_iterator ComprIt = m_CompressorMap.find(CompressedMethod_in);

		if (ComprIt != m_CompressorMap.cend())
			return ComprIt->second;

		return NULL;
	}
}