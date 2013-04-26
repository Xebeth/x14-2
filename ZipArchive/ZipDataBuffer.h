/**************************************************************************
	created		:	2013-04-25
	filename	: 	ZipDataBuffer.h
	author		:	Xebeth`
	copyright	:	North Edge (2013)
	purpose		:	ZIP data buffer (implements IZipData)
**************************************************************************/
#ifndef __ZIP_DATA_BUFFER_H__
#define __ZIP_DATA_BUFFER_H__

namespace ZipArchiveLib
{
	class ZipDataBuffer : public IZipData
	{
	public:
		ZipDataBuffer(const unsigned char* pBuffer_in, unsigned long BufferSize_in);

		unsigned long MoveTo(unsigned long &Position_in_out, long Offset_in);
		unsigned long CopyData(void* pDest_in, unsigned long DestSize_in);		
		unsigned long ReadAhead(unsigned long ChunkSize_in);
		bool Reset();

	private:
		//! buffer containing the ZIP data
		const unsigned char *m_pBuffer;
	};
}

#endif//__ZIP_DATA_BUFFER_H__
