/**************************************************************************
	created		:	2013-04-24
	filename	: 	ZipTypes.h
	author		:	Xebeth`
	copyright	:	North Edge (2013)
	purpose		:	
**************************************************************************/
#ifndef __ZIP_TYPES_H__
#define __ZIP_TYPES_H__

/*
/////////////////////////////////////////////////
File Format Specification
/////////////////////////////////////////////////

ZIP format
-------------------------------------------------
Files stored in arbitrary order.  Large .ZIP files can span multiple
volumes or be split into user-defined segment sizes. All values
are stored in little-endian byte order unless otherwise specified.

Overall zipfile format:
[local file header 1]
[encryption header 1]
[file data 1]
[data descriptor 1]
. 
.
.
[local file header n]
[encryption header n]
[file data n]
[data descriptor n]
[archive decryption header] 
[archive extra data record] 
[central directory header 1]
.
.
.
[central directory header n]
[zip64 end of central directory record]
[zip64 end of central directory locator]
[end of central directory record]

Local file header:
Offset   Length   Contents
  0      4 bytes  Local file header signature (0x04034b50)
  4      2 bytes  Version needed to extract
  6      2 bytes  General purpose bit flag
  8      2 bytes  Compression method
 10      2 bytes  Last mod file time
 12      2 bytes  Last mod file date
 14      4 bytes  CRC-32
 18      4 bytes  Compressed size (n)
 22      4 bytes  Uncompressed size
 26      2 bytes  Filename length (f)
 28      2 bytes  Extra field length (e)
		(f)bytes  Filename
		(e)bytes  Extra field
		(n)bytes  Compressed data

Data descriptor:
Offset   Length   Contents
  0      4 bytes  Data descriptor header signature (0x08074b50)
  4      4 bytes  CRC-32
  8      4 bytes  Compressed size
 12      4 bytes  Uncompressed size

Archive extra data record: 
Offset   Length   Contents
  0      4 bytes  Archive extra data signature (0x08064b50)
  4      4 bytes  Extra field length (fn)
		(n)bytes  Extra field data

Central directory:
Offset   Length   Contents
  0      4 bytes  Central file header signature (0x02014b50)
  4      2 bytes  Version made by
  6      2 bytes  Version needed to extract
  8      2 bytes  General purpose bit flag
 10      2 bytes  Compression method
 12      2 bytes  Last mod file time
 14      2 bytes  Last mod file date
 16      4 bytes  CRC-32
 20      4 bytes  Compressed size
 24      4 bytes  Uncompressed size
 28      2 bytes  Filename length (f)
 30      2 bytes  Extra field length (e)
 32      2 bytes  File comment length (c)
 34      2 bytes  Disk number start
 36      2 bytes  Internal file attributes
 38      4 bytes  External file attributes
 42      4 bytes  Relative offset of local header
 46     (f)bytes  Filename
		(e)bytes  Extra field
		(c)bytes  File comment

End of central directory record:
Offset   Length   Contents
  0      4 bytes  End of central dir signature (0x06054b50)
  4      2 bytes  Number of this disk
  6      2 bytes  Number of the disk with the start of the central directory
  8      2 bytes  Total number of entries in the central dir on this disk
 10      2 bytes  Total number of entries in the central dir
 12      4 bytes  Size of the central directory
 16      4 bytes  Offset of start of central directory with respect to the starting disk number
 20      2 bytes  zipfile comment length (c)
 22     (c)bytes  zipfile comment

compression method: (2 bytes)
		  0 - The file is stored (no compression)
		  1 - The file is Shrunk
		  2 - The file is Reduced with compression factor 1
		  3 - The file is Reduced with compression factor 2
		  4 - The file is Reduced with compression factor 3
		  5 - The file is Reduced with compression factor 4
		  6 - The file is Imploded
		  7 - Reserved for Tokenizing compression algorithm
		  8 - The file is Deflated
		  9 - Enhanced Deflating using Deflate64(tm)
		 10 - PKWARE Data Compression Library Imploding (old IBM TERSE)
		 11 - Reserved by PKWARE
		 12 - File is compressed using BZIP2 algorithm
		 13 - Reserved by PKWARE
		 14 - LZMA (EFS)
		 15 - Reserved by PKWARE
		 16 - Reserved by PKWARE
		 17 - Reserved by PKWARE
		 18 - File is compressed using IBM TERSE (new)
		 19 - IBM LZ77 z Architecture (PFS)
		 98 - PPMd version I, Rev 1

For a complete review of the Zip format, check APPNOTE.TXT

+--------------------------------------+---------------------------------------------------+
|   	  MS DOS Time  	               |      MS DOS Date                                  |
+---------+---------+--------+---------+--------------+----------------+-------------------+
| bit     |  0 - 4  | 5 - 10 | 11 - 15 | 16 - 20      | 21 - 24        | 25 - 31           |
+---------+---------+--------+---------+--------------+----------------+-------------------+
| value   | second 	| minute | hour    | day (1 - 31) | month (1 - 12) | years from 1980   |
+---------+---------+--------+---------+--------------+----------------+-------------------+
*/


namespace ZipArchiveLib
{
														//	  P K
	#define CENTRAL_DIRECTORY_SIGNATURE					0x02014b50
	#define LOCAL_FILE_HEADER_SIGNATURE					0x04034b50
	#define CENTRAL_DIRECTORY_DIGITAL_SIGNATURE			0x05054b50
	#define END_OF_CENTRAL_DIRECTORY_SIGNATURE			0x06054b50
	#define ZIP64_END_OF_CENTRAL_DIRECTORY_SIGNATURE	0x06064b50
	#define ZIP64_ECD_LOCATOR_SIGNATURE					0x07064b50
	#define ARCHIVE_EXTRA_DATA_SIGNATURE				0x08064b50
	#define DATA_DESCRIPTOR_SIGNATURE					0x08074b50

	// set the structure alignment to 1 byte
	#pragma pack(push, 1)

	//! local file header structure		   38 bytes
	typedef struct _LocalFileHeader
	{
		//! local file header signature     4 bytes  (0x04034b50)
		unsigned int HeaderSignature;
		//! version needed to extract       2 bytes
		unsigned short Version;
		//! general purpose bit flag        2 bytes
		unsigned short BitFlag;
		//! compression method              2 bytes
		unsigned short CompressionMethod;
		//! last mod file time              2 bytes
		unsigned short LastModFileTime;
		//! last mod file date              2 bytes
		unsigned short LastModFileDate;
		//! crc-32                          4 bytes
		unsigned int CRC32;
		//! compressed size                 4 bytes
		unsigned int CompressedSize;
		//! uncompressed size               4 bytes
		unsigned int UncompressedSize;
		//! file name length                2 bytes
		unsigned short FilenameLength;
		//! extra field length              2 bytes
		unsigned short ExtraFieldLength;
		//! file name						(variable size)
		//! extra field						(variable size)
	} LocalFileHeader;

	//! data descriptor with signature structure			   38 bytes
	typedef struct _DataDescriptorSigned
	{
		//! extended local file header signature (0x08074b50)	4 bytes
		unsigned int HeaderSignature;
		//! crc-32												4 bytes
		unsigned int CRC32;
		//! compressed size										4 bytes
		unsigned int CompressedSize;
		//! uncompressed size									4 bytes
		unsigned int UncompressedSize;
	} DataDescriptorSigned;

	//! data descriptor with signature structure			   38 bytes
	typedef struct _DataDescriptor
	{
		//! crc-32												4 bytes
		unsigned int CRC32;
		//! compressed size										4 bytes
		unsigned int CompressedSize;
		//! uncompressed size									4 bytes
		unsigned int UncompressedSize;
	} DataDescriptor;

	//! file encryption header structure   12 bytes
	typedef struct _FileEncryptionHeader
	{
		//! first encryption key			4 bytes
		unsigned int Key1;
		//! second encryption key			4 bytes
		unsigned int Key2;
		//! third encryption key			4 bytes
		unsigned int Key3;
	} FileEncryptionHeader;

	typedef struct _CentralDirectoryHeader
	{
		//! central file header signature   4 bytes  (0x02014b50)
		unsigned int HeaderSignature;
		//! version made by                 2 bytes
		unsigned short VersionMadeBy;
		//! version needed to extract       2 bytes
		unsigned short VersionNeededToExtract;
		//! general purpose bit flag        2 bytes
		unsigned short BitFlag;
		//! compression method              2 bytes
		unsigned short CompressionMethod;
		//! last mod file time              2 bytes
		unsigned short LastModFileTime;
		//! last mod file date              2 bytes
		unsigned short LastModFileDate;
		//! crc-32                          4 bytes
		unsigned int CRC32;
		//! compressed size                 4 bytes
		unsigned int CompressedSize;
		//! uncompressed size               4 bytes
		unsigned int UncompressedSize;
		//! file name length                2 bytes
		unsigned short FilenameLength;
		//! extra field length              2 bytes
		unsigned short ExtraFieldLength;
		//! file comment length             2 bytes
		unsigned short CommentLength;
		//! disk number start               2 bytes
		unsigned short DiskNumberPart;
		//! internal file attributes        2 bytes
		unsigned short InternalFileAttr;
		//! external file attributes        4 bytes
		unsigned int ExternalFileAttr;
		//! relative offset of local header 4 bytes
		unsigned int RelativeOffsetLocalHeader;
		//! file name						(variable size)
		//! extra field						(variable size)
		//! file comment					(variable size)
	} CentralDirectoryHeader;

	//! digital signature for central directory encryption structure
	typedef struct _DigitalSignature
	{
		//! header signature                4 bytes  (0x05054b50)
		unsigned int HeaderSignature;
		//! size of data                    2 bytes
		unsigned short SignatureSize;
		//! signature data					(variable size)
	} DigitalSignature;

	//! archive extra data header structure
	typedef struct _ArchiveExtraData
	{
		//! archive extra data signature    4 bytes  (0x08064b50)
		unsigned int HeaderSignature;
		//! extra field length              4 bytes
		unsigned int ExtraFieldLength;
		//! extra field data                (variable size)
	} ArchiveExtraData;

	//! end of central directory record
	typedef struct _EndOfCentralDirectoryHeader
	{
		//! end of central dir signature														4 bytes  (0x06054b50)
		unsigned int HeaderSignature;
		//! number of this disk																	2 bytes
		unsigned short DiskNumber;
		//! number of the disk with the start of the central directory							2 bytes
		unsigned short DiskNumberWithStart;
		//! total number of entries in the central directory on this disk						2 bytes
		unsigned short TotalNumEntriesOnDisk;
		//! total number of entries in the central directory									2 bytes
		unsigned short TotalNumEntries;
		//! size of the central directory														4 bytes
		unsigned int CentralDirSize;
		//! offset of start of central directory with respect to the starting disk number		4 bytes
		unsigned int OffsetStart;
		//! .ZIP file comment length															2 bytes
		unsigned short ZipCommentLength;
		//! .ZIP file comment																	(variable size)
	} EndOfCentralDirectoryHeader;

	//! ZIP64 end of central directory record
	typedef struct _Zip64EndOfCentralDirectory
	{
		//! zip64 end of central dir signature													4 bytes  (0x06064b50)
		unsigned int HeaderSignature;
		//! size of zip64 end of central directory record										8 bytes
		unsigned long long Zip64RecordSize;
		//! version made by																		2 bytes
		unsigned short VersionMadeBy;
		//! version needed to extract															2 bytes
		unsigned short VersionNeededToExtract;
		//! number of this disk																	4 bytes
		unsigned int DiskNumber;
		//! number of the disk with the start of the central directory							4 bytes
		unsigned int DiskNumberWithStart;
		//! total number of entries in the central directory on this disk						8 bytes
		unsigned long long TotalNumEntriesOnDisk;
		//! total number of entries in the central directory									8 bytes
		unsigned long long TotalNumEntries;
		//! size of the central directory														8 bytes
		unsigned long long CentralDirSize;
		//! offset of start of central directory with respect to the starting disk number		8 bytes
		unsigned long long OffsetStart;
		//! zip64 extensible data sector														(variable size)
	} Zip64EndOfCentralDirectory;

	//! ZIP64 end of central directory record version 2
	typedef struct _Zip64EndOfCentralDirectory2
	{
		//! method used to compress theCentral Directory	2 bytes
		unsigned short CompressionMethod;
		//!	size of the compressed data						8 bytes
		unsigned long long CompressedDataSize;
		//!	original uncompressed size						8 bytes
		unsigned long long UncompressedDataSize;
		//!	encryption algorithm ID							2 bytes
		unsigned short EncryptionAlgorithmID;
		//!	encryption key length							2 bytes
		unsigned short EncryptionKeyLength;
		//!	encryption flags								2 bytes
		unsigned short EncryptionFlags;
		//!	hash algorithm identifier						2 bytes
		unsigned short HashAlgorithmID;
		//!	length of hash data								2 bytes
		unsigned short HashDataLength;
		//!	hash data										(variable size)
	} Zip64EndOfCentralDirectory2;

	//! ZIP64 end of central directory locator structure
	typedef struct _Zip64EndOfCentralDirectoryLocator
	{
		//! signature																				4 bytes  (0x07064b50)
		unsigned int HeaderSignature;
		//! number of the disk with the start of the zip64 end of central directory					4 bytes
		unsigned int DiskNumberWithStart;
		//! relative offset of the zip64 end of central directory record							8 bytes
		unsigned long long RelativeOffset;
		//! total number of disks																	4 bytes
		unsigned int DiskCount;
	} Zip64EndOfCentralDirectoryLocator;
	
	#pragma pack(pop)
	// restore the structure alignment

	//! Version made by enumeration
	enum eVersionMadeBy
	{
		VERSION_BY_MSDOS		=  0U,		//!< MS-DOS and OS/2 (FAT / VFAT / FAT32 file systems)
		VERSION_BY_AMIGA		=  1U,		//!< Amiga
		VERSION_BY_OPENVMS		=  2U,		//!< OpenVMS
		VERSION_BY_UNIX			=  3U,		//!< UNIX
		VERSION_BY_VM_CMS		=  4U,		//!< VM/CMS
		VERSION_BY_ATARI_ST		=  5U,		//!< Atari ST
		VERSION_BY_OS2_HPFS		=  6U,		//!< OS/2 H.P.F.S.
		VERSION_BY_MAC			=  7U,		//!< Macintosh
		VERSION_BY_ZSYSTEM		=  8U,		//!< Z-System
		VERSION_BY_CPM			=  9U,		//!< CP/M
		VERSION_BY_NTFS			= 10U,		//!< Windows NTFS
		VERSION_BY_MVS			= 11U,		//!< MVS (OS/390 - Z/OS)
		VERSION_BY_VSE			= 12U,		//!< VSE
		VERSION_BY_ACORN_RISC	= 13U,		//!< Acorn Risc
		VERSION_BY_VFAT			= 14U,		//!< VFAT
		VERSION_BY_ALT_MVS		= 15U,		//!< alternate MVS
		VERSION_BY_BEOS			= 16U,		//!< BeOS
		VERSION_BY_TANDEM		= 17U,		//!< Tandem
		VERSION_BY_OS400		= 18U,		//!< OS/400
		VERSION_BY_OSX			= 19U,		//!< OS X (Darwin)
	};

	//! Version needed enumeration
	enum eVersionNeeded
	{
		VERSION_NEEDED_DEFAULT						= 10U,	//!< 1.0 - Default value
		VERSION_NEEDED_VOL_LABEL					= 11U,	//!< 1.1 - File is a volume label
		VERSION_NEEDED_FOLDER						= 20U,	//!< 2.0 - File is a folder (directory)
		VERSION_NEEDED_DEFLATE						= 20U,	//!< 2.0 - File is compressed using Deflate compression
		VERSION_NEEDED_PKWARE_ENCRYPT				= 20U,	//!< 2.0 - File is encrypted using traditional PKWARE encryption
		VERSION_NEEDED_DEFLATE64					= 21U,	//!< 2.1 - File is compressed using Deflate64(tm)
		VERSION_NEEDED_PKWARE_IMPLODE				= 25U,	//!< 2.5 - File is compressed using PKWARE DCL Implode 
		VERSION_NEEDED_PATCH_DATA_SET				= 27U,	//!< 2.7 - File is a patch data set 
		VERSION_NEEDED_ZIP64						= 45U,	//!< 4.5 - File uses ZIP64 format extensions
		VERSION_NEEDED_BZIP2						= 46U,	//!< 4.6 - File is compressed using BZIP2 compression*
		VERSION_NEEDED_DES_ENCRYPT					= 50U,	//!< 5.0 - File is encrypted using DES
		VERSION_NEEDED_3DES_ENCRYPT					= 50U,	//!< 5.0 - File is encrypted using 3DES
		VERSION_NEEDED_ORIGINAL_RC2_ENCRYPT			= 50U,	//!< 5.0 - File is encrypted using original RC2 encryption
		VERSION_NEEDED_RC4_ENCRYPT					= 50U,	//!< 5.0 - File is encrypted using RC4 encryption
		VERSION_NEEDED_AES_ENCRYPT					= 51U,	//!< 5.1 - File is encrypted using AES encryption
		VERSION_NEEDED_CORRECT_RC2_ENCRYPT			= 51U,	//!< 5.1 - File is encrypted using corrected RC2 encryption**
		VERSION_NEEDED_CORRECT_RC2_64_ENCRYPT		= 52U,	//!< 5.2 - File is encrypted using corrected RC2-64 encryption**
		VERSION_NEEDED_NON_OAEP_ENCRYPT				= 61U,	//!< 6.1 - File is encrypted using non-OAEP key wrapping***
		VERSION_NEEDED_CENTRAL_DIRECTORY_ENCRYPT	= 62U,	//!< 6.2 - Central directory encryption
		VERSION_NEEDED_LZMA							= 63U,	//!< 6.3 - File is compressed using LZMA
		VERSION_NEEDED_PPMD							= 63U,	//!< 6.3 - File is compressed using PPMd+
		VERSION_NEEDED_BLOWFISH_ENCRYPT				= 63U,	//!< 6.3 - File is encrypted using Blowfish
		VERSION_NEEDED_TWOFISH_ENCRYPT				= 63U,	//!< 6.3 - File is encrypted using Twofish
	};

	//! general purpose bit flag enumeration
	enum eBitFlag
	{
		FLAG_NONE								= 0x0000U,
		FLAG_ENCRYPTED_FILE						= 0x0001U,	//!< Bit  0

		// Method 6 (Imploding) specific flags
		FLAG_IMPLODE_4K_8K_SLIDING_DIC			= 0x0002U,	//!< Bit  1
		FLAG_IMPLODE_2_3_SHANNON_FANO_TREES		= 0x0004U,	//!< Bit  2
		// Method 8-9 (Deflating) specific flags
		FLAG_DEFLATING_EN_EX_COMPRESSION		= 0x0002U,	//!< Bit  1
		FLAG_DEFLATING_EF_ES_COMPRESSION		= 0x0004U,	//!< Bit  2				Bits	1	2
		FLAG_DEFLATING_NORMAL					= FLAG_NONE,						//		0	0
		FLAG_DEFLATING_MAXIMUM					= FLAG_DEFLATING_EN_EX_COMPRESSION,	//		1	0
		FLAG_DEFLATING_FAST						= FLAG_DEFLATING_EF_ES_COMPRESSION, //		0	1
		FLAG_DEFLATING_SUPER_FAST				= FLAG_DEFLATING_EN_EX_COMPRESSION	//		1	1
												| FLAG_DEFLATING_EF_ES_COMPRESSION,
		// Method 14 (LZMA) specific flag
		FLAG_LZMA_USE_EOS_MARKER				= 0x0002U,	//!< Bit  1
		FLAG_USE_DATA_DESCRIPTORS				= 0x0008U,	//!< Bit  3
		// Method 8-9 (Deflating) specific flag
		FLAG_DEFLATING_ENHANCED					= 0x0010U,	//!< Bit  4
		FLAG_COMPRESSED_PATCHED_DATA			= 0x0020U,	//!< Bit  5
		FLAG_STRONG_ENCRYPTION					= 0x0040U,	//!< Bit  6
		FLAG_UNUSED_1							= 0x0080U,	//!< Bit  7
		FLAG_UNUSED_2							= 0x0100U,	//!< Bit  8
		FLAG_UNUSED_3							= 0x0200U,	//!< Bit  9
		FLAG_UNUSED_4							= 0x0400U,	//!< Bit 10
		FLAG_UTF8_ENCODING						= 0x0800U,	//!< Bit 11
		FLAG_RESERVED_1							= 0x1000U,	//!< Bit 12
		FLAG_ENCRYPTED_CENTRAL_DIRECTORY		= 0x2000U,	//!< Bit 13
		FLAG_RESERVED_2							= 0x4000U,	//!< Bit 14
		FLAG_RESERVED_3							= 0x8000U,	//!< Bit 15
	};

	//! compression method enumeration
	enum eCompressionMethod
	{
		COMPRESSION_STORE							=  0U,	//!< The file is stored (no compression)
		COMPRESSION_SHRUNK							=  1U,	//!< The file is Shrunk
		COMPRESSION_REDUCED_1						=  2U,	//!< The file is Reduced with compression factor 1
		COMPRESSION_REDUCED_2						=  3U,	//!< The file is Reduced with compression factor 2
		COMPRESSION_REDUCED_3						=  4U,	//!< The file is Reduced with compression factor 3
		COMPRESSION_REDUCED_4						=  5U,	//!< The file is Reduced with compression factor 4
		COMPRESSION_IMPLODED						=  6U,	//!< The file is Imploded
		COMPRESSION_RESERVED_TOKENIZE				=  7U,	//!< Reserved for Tokenizing compression algorithm
		COMPRESSION_DEFLATED						=  8U,	//!< The file is Deflated
		COMPRESSION_DEFLATED64						=  9U,	//!< Enhanced Deflating using Deflate64(tm)
		COMPRESSION_OLD_TERSE						= 10U,	//!< PKWARE Data Compression Library Imploding (old IBM TERSE)
		COMPRESSION_RESERVED_1						= 11U,	//!< Reserved by PKWARE
		COMPRESSION_BZIP2							= 12U,	//!< File is compressed using BZIP2 algorithm
		COMPRESSION_RESERVED_2						= 13U,	//!< Reserved by PKWARE
		COMPRESSION_LZMA							= 14U,	//!< LZMA (EFS)
		COMPRESSION_RESERVED_3						= 15U,	//!< Reserved by PKWARE
		COMPRESSION_RESERVED_4						= 16U,	//!< Reserved by PKWARE
		COMPRESSION_RESERVED_5						= 17U,	//!< Reserved by PKWARE
		COMPRESSION_NEW_TERSE						= 18U,	//!< File is compressed using IBM TERSE (new)
		COMPRESSION_LZ77							= 19U,	//!< IBM LZ77 z Architecture (PFS)
		COMPRESSION_WAVPACK							= 97U,	//!< WavPack compressed data
		COMPRESSION_PPMD_V1_REV1					= 98U,	//!< PPMd version I, Rev 1
	};

	//! internal file attributes enumeration
	enum eInternalFileAttributes
	{
		INTERNAL_ATTR_RESERVED_ASCII						= 0x0001U,
		INTERNAL_ATTR_RESERVED_RECORD_LENGTH_CONTROL_FIELD	= 0x0002U,
		INTERNAL_ATTR_UNUSED_1								= 0x0004U,
		INTERNAL_ATTR_UNUSED_2								= 0x0010U,
		INTERNAL_ATTR_UNUSED_3								= 0x0020U,
		INTERNAL_ATTR_UNUSED_4								= 0x0040U,
		INTERNAL_ATTR_UNUSED_5								= 0x0080U,
		INTERNAL_ATTR_UNUSED_6								= 0x0100U,
		INTERNAL_ATTR_UNUSED_7								= 0x0200U,
		INTERNAL_ATTR_UNUSED_8								= 0x0400U,
		INTERNAL_ATTR_UNUSED_9								= 0x0800U,
		INTERNAL_ATTR_UNUSED_10								= 0x1000U,
		INTERNAL_ATTR_UNUSED_11								= 0x2000U,
		INTERNAL_ATTR_UNUSED_12								= 0x4000U,
		INTERNAL_ATTR_UNUSED_13								= 0x8000U,
	};

	enum eExternalFileAttribules
	{
		EXTERNAL_ATTR_MSDOS_READ_ONLY						= 0x0001U, 	//!< MSDOS - Read Only file
		EXTERNAL_ATTR_MSDOS_HIDDEN							= 0x0002U, 	//!< MSDOS - Hidden file
		EXTERNAL_ATTR_MSDOS_SYSTEM							= 0x0004U, 	//!< MSDOS - System file
		EXTERNAL_ATTR_MSDOS_VOLUME_LABEL					= 0x0008U, 	//!< MSDOS - Volume Label
		EXTERNAL_ATTR_MSDOS_DIRECTORY						= 0x0010U, 	//!< MSDOS - Subdirectory
		EXTERNAL_ATTR_MSDOS_ARCHIVE							= 0x0020U, 	//!< MSDOS - Archive
		EXTERNAL_ATTR_MSDOS_RESERVED_1						= 0x0040U, 	//!< MSDOS - Reserved
		EXTERNAL_ATTR_MSDOS_RESERVED_2						= 0x0080U, 	//!< MSDOS - Reserved
	};
}

#endif//__ZIP_TYPES_H__
