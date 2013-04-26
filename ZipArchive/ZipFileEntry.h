/**************************************************************************
	created		:	2013-04-24
	filename	: 	ZipFileEntry.h
	author		:	Xebeth`
	copyright	:	North Edge (2013)
	purpose		:	Zip file entry
**************************************************************************/
#ifndef __ZIP_FILE_ENTRY_H__
#define __ZIP_FILE_ENTRY_H__

namespace ZipArchiveLib
{
	// forward declarations
	typedef struct _DataDescriptorSigned DataDescriptorSigned;
	typedef struct _FileEncryptionHeader FileEncryptionHeader;
	typedef struct _LocalFileHeader LocalFileHeader;
	
	//! \brief Zip file entry
	class ZipFileEntry : public BaseEntry
	{
	public:
		ZipFileEntry();
		~ZipFileEntry();

		void SetDefaults(LocalFileHeader &Header_in_out) const;

		void SetLocalFileHeader(const LocalFileHeader &FileHeader_in, unsigned long Position_in);
		void SetEncryptionHeader(const FileEncryptionHeader &EncryptionHeader_in);
		void SetDataDescriptor(const DataDescriptorSigned &DataDescriptor_in);
		void SetCompressedData(unsigned char *pCompressedData_in);
		void SetExtraData(unsigned char *pExtraData_in);
		void SetFilename(const char *pFilename_in);

		const unsigned char* GetUncompressedData() const;
		const unsigned char* GetCompressedData() const;
		unsigned char** GetUncompressedDataPtr();
		unsigned char** GetCompressedDataPtr();

		unsigned long GetUncompressedDataSize() const;
		unsigned long GetCompressedDataSize() const;
		unsigned long GetFilenameLength() const;
		const std::string& GetFilename() const;
		unsigned long GetExtraDataSize() const;
		int GetCompressionMethod() const;
		bool HasDataDescriptor() const;
		unsigned int GetCRC() const;
		bool IsEncrypted() const;

	private:
		bool IsFlagSet(int Flag_in) const;
		void CreateHeader();

		//! ZIP local file header
		LocalFileHeader *m_pFileHeader;
		//! filename
		std::string m_Filename;
		//! extra data
		unsigned char *m_pExtraData;
		//! pointer to the compressed data
		unsigned char *m_pCompressedData;
		//! pointer to the uncompressed data
		unsigned char *m_pUncompressedData;
		//! encryption header
		FileEncryptionHeader *m_pEncryptionHeader;
		//! extended file header
		DataDescriptorSigned *m_pDataDescriptor;
	};
}

#endif//__ZIP_FILE_ENTRY_H__
