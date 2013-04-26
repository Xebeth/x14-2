/**************************************************************************
	created		:	2013-04-24
	filename	: 	CentralDirectoryEntry.h
	author		:	Xebeth`
	copyright	:	North Edge (2013)
	purpose		:	
**************************************************************************/
#ifndef __CENTRAL_DIRECTORY_ENTRY_H__
#define __CENTRAL_DIRECTORY_ENTRY_H__

namespace ZipArchiveLib
{
	// forward declarations
	typedef struct _CentralDirectoryHeader CentralDirectoryHeader;
	class ZipFileEntry;

	class CentralDirectoryEntry : public BaseEntry
	{
	public:
		CentralDirectoryEntry();
		~CentralDirectoryEntry();

		void SetDefaults(CentralDirectoryHeader &Header_in_out) const;

		void SetCentralDirectoryHeader(const CentralDirectoryHeader &CentralDirHeader_in,
									   unsigned long Position_in);
		void SetExtraData(unsigned char *pExtraData_in);
		void SetFileEntry(ZipFileEntry *pFileEntry_in);
		void SetFileComment(const char * pFilename_in);
		void SetFilename(const char *pFilename_in);
		

		unsigned long GetFileCommentLength() const;
		unsigned long GetFilenameLength() const;
		const std::string& GetFilename() const;
		unsigned long GetExtraDataSize() const;
		ZipFileEntry* GetFileEntry() const;
		unsigned int GetCRC() const;

	private:
		void CreateHeader();

		//! ZIP central directory
		CentralDirectoryHeader *m_pCentralDirectoryHeader;
		//! local file entry matching this entry
		ZipFileEntry *m_pFileEntry;
		//! filename
		std::string m_Filename;
		//! extra data
		unsigned char *m_pExtraData;
		//! file comment
		std::string m_FileComment;
	};
}

#endif//__CENTRAL_DIRECTORY_ENTRY_H__
