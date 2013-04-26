/**************************************************************************
	created		:	2013-04-25
	filename	: 	EndOfCentralDirectory.h
	author		:	Xebeth`
	copyright	:	North Edge (2013)
	purpose		:	End of central directory
**************************************************************************/
#ifndef __END_OF_CENTRAL_DIRECTORY_H__
#define __END_OF_CENTRAL_DIRECTORY_H__

namespace ZipArchiveLib
{
	// forward declarations
	typedef struct _Zip64EndOfCentralDirectoryLocator Zip64EndOfCentralDirectoryLocator;
	typedef struct _EndOfCentralDirectoryHeader EndOfCentralDirectoryHeader;
	typedef struct _Zip64EndOfCentralDirectory2 Zip64EndOfCentralDirectory2;
	typedef struct _Zip64EndOfCentralDirectory Zip64EndOfCentralDirectory;

	//! End of central directory
	class EndOfCentralDirectory : public BaseEntry
	{
	public:
		EndOfCentralDirectory();
		virtual ~EndOfCentralDirectory();

		void SetDefaults(EndOfCentralDirectoryHeader &Header_in_out) const;

		void SetEndOfCentralDirectory(const EndOfCentralDirectoryHeader &EndOfCentralDir_in,
									  unsigned long Position_in);
		void SetZipComment(const char *pZipComment_in);

		unsigned long GetZipCommentLength() const;

	private:
		void CreateHeader();

		//! ZIP64 end of central directory record version 1
		Zip64EndOfCentralDirectory *m_pZip64ECDv1;
		//! zip64 extensible data sector
		unsigned char *m_pZip64ExtensibleData;
		//! ZIP64 end of central directory record version 2
		Zip64EndOfCentralDirectory2 *m_pZip64ECDv2;
		//!	hash data
		unsigned char *m_pZip64HashData;
		//! ZIP64 end of central directory locator
		Zip64EndOfCentralDirectoryLocator *m_pZip64ECDLocator;
		//! end of central directory record
		EndOfCentralDirectoryHeader *m_pEndOfCentralDirHeader;
		//! global zip comment
		std::string m_ZipComment;
	};
}

#endif//__END_OF_CENTRAL_DIRECTORY_H__
