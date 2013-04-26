/**************************************************************************
	created		:	2013-04-25
	filename	: 	IDataCompression.h
	author		:	Xebeth`
	copyright	:	North Edge (2013)
	purpose		:	Data compression interface
**************************************************************************/
#ifndef __I_DATA_COMPRESSION_H__
#define __I_DATA_COMPRESSION_H__

namespace ZipArchiveLib
{
	//! \brief Data compression interface
	class IDataCompression
	{
	public:
		//! \brief IDataCompression destructor
		virtual ~IDataCompression() {}

		/*! \brief Decompresses the specified buffer
			\param[in] pCompressedData_in : the data buffer to uncompress
			\param[in] CompressedSize_in : the size of the compressed data
			\param[out] pUncompressedData_out : buffer receiving the uncompressed data
			\param[out] UncompressedSize_out : the size of the resulting uncompressed data
			\return true if the data was uncompressed successfully; false otherwise
		*/
		virtual bool Uncompress(const unsigned char *pCompressedData_in, unsigned long CompressedSize_in,
								unsigned char **pUncompressedData_out, unsigned long &UncompressedSize_out) =0;

		/*! \brief Compresses the specified buffer
			\param[in] pUncompressedData_in : the data buffer to compress
			\param[in] UncompressedSize_in : the size of the uncompressed data
			\param[out] pCompressedData_out : buffer receiving the compressed data
			\param[out] CompressedSize_out : the size of the resulting compressed data
			\return true if the data was compressed successfully; false otherwise
		*/
		virtual bool Compress(const unsigned char *pUncompressedData_in, unsigned long UncompressedSize_in,
							  unsigned char **pCompressedData_out, unsigned long &CompressedSize_out) =0;
	};
}

#endif//__I_DATA_COMPRESSION_H__
