/**************************************************************************
	created		:	2013-04-26
	filename	: 	zLibCompression.cpp
	author		:	Xebeth`
	copyright	:	North Edge (2013)
	purpose		:	
**************************************************************************/
#include "stdafx.h"
#include <zlib.h>

#include "IDataCompression.h"
#include "zLibCompression.h"

namespace ZipArchiveLib
{
	//! \brief zLibCompression default constructor
	zLibCompression::zLibCompression()
		: m_CompressionLevel(Z_DEFAULT_COMPRESSION) {}

	/*! \brief Sets the compression level
		\param[in] CompressionLevel_in : the new compression level
	*/
	void zLibCompression::SetCompressionLevel(int CompressionLevel_in)
	{ m_CompressionLevel = CompressionLevel_in; }

	/*! \brief Compresses the specified buffer
		\param[in] pUncompressedData_in : the data buffer to compress
		\param[in] UncompressedSize_in : the size of the uncompressed data
		\param[out] pCompressedData_out : buffer receiving the compressed data
		\param[out] CompressedSize_in_out : the size of the resulting compressed data
											may contain the size of the compressed data if known
		\return true if the data was compressed successfully; false otherwise
	*/
	bool zLibCompression::Compress(const unsigned char *pUncompressedData_in, unsigned long UncompressedSize_in,
								   unsigned char **pCompressedData_out, unsigned long &CompressedSize_in_out)
	{
		if (pUncompressedData_in != NULL && pCompressedData_out != NULL
		 && *pCompressedData_out == NULL && UncompressedSize_in > 0UL)
		{
			int deflateErr = Z_STREAM_ERROR;
			z_stream DeflateStream;

			memset(&DeflateStream, 0, sizeof(DeflateStream));
			deflateErr = deflateInit2(&DeflateStream, m_CompressionLevel, Z_DEFLATED, 
									  -MAX_WBITS, DEFLATE2_MEM_LEVEL, Z_DEFAULT_STRATEGY);

			if (deflateErr == Z_OK)
			{
				unsigned long AllocCount = 0UL, DataAvailable = 0UL;
				int FlushMethod = Z_NO_FLUSH;
				bool Result = false;

				// check if the operation can be done in one call (provided the compressed size is known)
				if (CompressedSize_in_out > 0UL && CompressedSize_in_out < ZLIB_CHUNK_SIZE)
				{
					DataAvailable = CompressedSize_in_out;
					FlushMethod = Z_FINISH;
				}
				else
					DataAvailable = ZLIB_CHUNK_SIZE;
				// update the alloc count
				AllocCount = DataAvailable / sizeof(unsigned char);
				// reset the count
				CompressedSize_in_out = 0UL;
				// setup the stream input buffer
				DeflateStream.next_in = pUncompressedData_in;
				DeflateStream.avail_in = UncompressedSize_in;

				// initial allocation
				if (Buffer::Realloc(pCompressedData_out, AllocCount, true))
				{
					do 
					{
						// setup the stream output buffer
						DeflateStream.next_out = (*pCompressedData_out) + CompressedSize_in_out;
						DeflateStream.avail_out = DataAvailable;
						// compress the buffer
						deflateErr = deflate(&DeflateStream, Z_NO_FLUSH);

						if (deflateErr == Z_OK)
						{
							// update the compressed size
							CompressedSize_in_out += (DataAvailable - DeflateStream.avail_out);
							// reallocate the buffer
							DataAvailable = DeflateStream.avail_out + ZLIB_CHUNK_SIZE;
							AllocCount = DataAvailable / sizeof(unsigned char);

							if (Buffer::Realloc(pCompressedData_out, AllocCount) == false)
								break;
						}
						else if (deflateErr == Z_STREAM_END)
						{
							// should never happen
							if (CompressedSize_in_out != DeflateStream.total_out)
								CompressedSize_in_out = DeflateStream.total_out;
							// finished
							Result = (deflateErr == Z_STREAM_END && DeflateStream.avail_in == 0);
						}
						else
							break;
					}
					while (Result == false);

					// cleanup if the compression was aborted prematurely
					if (Result == false)
					{
						// cleanup the buffer but keep the size to 
						// give an idea of the amount of data read
						free(*pCompressedData_out);
						*pCompressedData_out = NULL;
					}
				}

				deflateEnd(&DeflateStream);

				return Result;
			}
		}

		return false;
	}

	/*! \brief Decompresses the specified buffer
		\param[in] pCompressedData_in : the data buffer to uncompress
		\param[in] CompressedSize_in : the size of the compressed data
		\param[out] pUncompressedData_out : buffer receiving the uncompressed data
		\param[out] UncompressedSize_in_out : the size of the resulting uncompressed data
											  may contain the size of the uncompressed data if known
		\return true if the data was uncompressed successfully; false otherwise
	*/
	bool zLibCompression::Uncompress(const unsigned char *pCompressedData_in, unsigned long CompressedSize_in,
									 unsigned char **pUncompressedData_out, unsigned long &UncompressedSize_in_out)
	{
		if (pCompressedData_in != NULL && pUncompressedData_out != NULL
		 && *pUncompressedData_out == NULL && CompressedSize_in > 0UL)
		{
			int deflateErr = Z_STREAM_ERROR;
			z_stream InflateStream;

			memset(&InflateStream, 0, sizeof(InflateStream));
			deflateErr = inflateInit2(&InflateStream, -MAX_WBITS);

			if (deflateErr == Z_OK)
			{
				unsigned long AllocCount = 0UL, DataAvailable = 0UL;
				int FlushMethod = Z_NO_FLUSH;
				bool Result = false;

				// check if the operation can be done in one call (provided the uncompressed size is known)
				if (UncompressedSize_in_out > 0UL && UncompressedSize_in_out < ZLIB_CHUNK_SIZE)
				{
					DataAvailable = UncompressedSize_in_out;
					FlushMethod = Z_FINISH;
				}
				else
					DataAvailable = ZLIB_CHUNK_SIZE;
				// update the alloc count
				AllocCount = DataAvailable / sizeof(unsigned char);
				// reset the count
				UncompressedSize_in_out = 0UL;
				// setup the stream input buffer
				InflateStream.next_in = pCompressedData_in;
				InflateStream.avail_in = CompressedSize_in;

				// initial allocation
				if (Buffer::Realloc(pUncompressedData_out, AllocCount, true))
				{
					do 
					{
						// setup the stream output buffer
						InflateStream.next_out = (*pUncompressedData_out) + UncompressedSize_in_out;
						InflateStream.avail_out = DataAvailable;
						// compress the buffer
						deflateErr = inflate(&InflateStream, FlushMethod);

						if (deflateErr == Z_OK)
						{
							// update the compressed size
							UncompressedSize_in_out += (DataAvailable - InflateStream.avail_out);
							// reallocate the buffer
							DataAvailable = InflateStream.avail_out + ZLIB_CHUNK_SIZE;
							AllocCount = DataAvailable / sizeof(unsigned char);

							if (Buffer::Realloc(pUncompressedData_out, AllocCount) == false)
								break;
						}
						else if (deflateErr == Z_STREAM_END)
						{
							// update the compressed size
							UncompressedSize_in_out += (DataAvailable - InflateStream.avail_out);
							// should never happen
							if (UncompressedSize_in_out != InflateStream.total_out)
								UncompressedSize_in_out = InflateStream.total_out;
							// finished
							Result = (deflateErr == Z_STREAM_END && InflateStream.avail_in == 0);
						}
						else
							break;
					}
					while (Result == false);

					// cleanup if the compression was aborted prematurely
					if (Result == false)
					{
						// cleanup the buffer but keep the size to 
						// give an idea of the amount of data read
						free(*pUncompressedData_out);
						*pUncompressedData_out = NULL;
					}
				}

				inflateEnd(&InflateStream);

				return Result;
			}
		}

		return false;
	}
}