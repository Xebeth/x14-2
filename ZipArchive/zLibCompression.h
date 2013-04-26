/**************************************************************************
	created		:	2013-04-26
	filename	: 	zLibCompression.h
	author		:	Xebeth`
	copyright	:	North Edge (2013)
	purpose		:	zLib compression implementation
**************************************************************************/
#ifndef __ZLIB_COMPRESSION_H__
#define __ZLIB_COMPRESSION_H__

#define	DEFLATE2_MEM_LEVEL	8
#define ZLIB_CHUNK_SIZE		0x00100000UL // 1MiB

namespace ZipArchiveLib
{
	//! \brief zLib compression implementation
	class zLibCompression : public IDataCompression
	{
	public:
		zLibCompression();

		bool Uncompress(const unsigned char *pCompressedData_in, unsigned long CompressedSize_in,
						unsigned char **pUncompressedData_out, unsigned long &UncompressedSize_in_out);

		bool Compress(const unsigned char *pUncompressedData_in, unsigned long UncompressedSize_in,
					  unsigned char **pCompressedData_out, unsigned long &CompressedSize_in_out);

		void SetCompressionLevel(int CompressionLevel_in);

	private:
		//! the compression level
		int m_CompressionLevel;
	};
}

#endif//__ZLIB_COMPRESSION_H__
