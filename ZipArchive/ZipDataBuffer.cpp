/**************************************************************************
	created		:	2013-04-25
	filename	: 	ZipDataBuffer.cpp
	author		:	Xebeth`
	copyright	:	North Edge (2013)
	purpose		:	ZIP data buffer (implements IZipData)
**************************************************************************/
#include "stdafx.h"

#include "IZipData.h"
#include "ZipDataBuffer.h"

namespace ZipArchiveLib
{
	/*! \brief ZipDataBuffer constructor
		\param[in] pBuffer_in : the buffer containing the ZIP data
		\param[in] BufferSize_in : the size of the buffer
	*/
	ZipDataBuffer::ZipDataBuffer(const unsigned char* pBuffer_in, unsigned long BufferSize_in)
		: IZipData(BufferSize_in), m_pBuffer(pBuffer_in) {}

	/*! \brief Copies the data at the current position to the specified buffer
		\param[in] pDest_in : a pointer to the destination buffer
		\param[in] DestSize_in : the size of the destination buffer
		\return the number of bytes copied
	*/
	unsigned long ZipDataBuffer::CopyData(void* pDest_in, unsigned long DestSize_in)
	{
		unsigned long CopySize = 0UL;

		if (m_pBuffer != NULL && DestSize_in > 0UL)
		{
			CopySize = min(m_AvailableData * sizeof(unsigned char), DestSize_in);

			if (memcpy_s(pDest_in, CopySize, m_pBuffer + m_Position, CopySize) != 0)
				return 0UL;
		}

		return CopySize;
	}

	/*! \brief Reads the specified amount of data ahead of the current position
		\param[in] ChunkSize_in : the amount of data to read in bytes
		\return the number of bytes read
	*/
	unsigned long ZipDataBuffer::ReadAhead(unsigned long ChunkSize_in)
	{
		if (m_pBuffer != NULL)
		{
			// return either the requested amount or the remaining size
			return min(m_AvailableData * sizeof(unsigned char), ChunkSize_in);
		}

		return 0UL;
	}

	/*! \brief Moves by the specified offset from the current position
		\param[in,out] Position_in_out : the position to move from
										 receives the new position if successful
		\param[in] Offset_in : the offset to move by
		\return the 4 bytes of raw data found at the new position
	*/
	unsigned long ZipDataBuffer::MoveTo(unsigned long &Position_in_out, long Offset_in)
	{
		unsigned long Result = 0UL;

		if (m_pBuffer != NULL)
		{
			m_Position = Position_in_out + Offset_in;

			if (m_Position > m_DataSize)
			{
				m_Position = m_DataSize; // next 4 bytes are beyond the buffer
				m_AvailableData = 0UL;
			}
			else
			{
				Result = *(unsigned long*)(m_pBuffer + m_Position);
				m_AvailableData = m_DataSize - m_Position;
			}

			Position_in_out = m_Position;
		}

		return Result;
	}

	/*! \brief Resets the internal position to its initial value
		\return true if the position was reset; false otherwise
	*/
	bool ZipDataBuffer::Reset()
	{
		m_AvailableData = m_DataSize;
		m_Position = 0UL;

		return true;
	}
}