/**************************************************************************
	created		:	2013-04-25
	filename	: 	IZipData.h
	author		:	Xebeth`
	copyright	:	North Edge (2013)
	purpose		:	ZIP data container interface
**************************************************************************/
#ifndef __I_ZIP_DATA_H__
#define __I_ZIP_DATA_H__

namespace ZipArchiveLib
{
	//! \brief ZIP data container interface
	class IZipData
	{
	public:
		/*! \brief IZipData constructor
			\param[in] DataSize_in : the size of the ZIP data in bytes
		*/
		explicit IZipData(unsigned long DataSize_in);

		//! \brief IZipData destructor
		virtual ~IZipData();
		
		/*! \brief Copies the data at the current position to the specified buffer
			\param[in] pDest_in : a pointer to the destination buffer
			\param[in] DestSize_in : the size of the destination buffer
			\return the number of bytes copied
		*/
		virtual unsigned long CopyData(void* pDest_in, unsigned long DestSize_in) =0;

		/*! \brief Reads the specified amount of data ahead of the current position
			\param[in] ChunkSize_in : the amount of data to read in bytes
			\return the number of bytes read
		*/
		virtual unsigned long ReadAhead(unsigned long ChunkSize_in) =0;

		/*! \brief Moves by the specified offset from the current position
			\param[in,out] Position_in_out : the position to move from
											 receives the new position if successful
			\param[in] Offset_in : the offset to move by
			\return the 4 bytes of raw data found at the new position
		*/
		virtual unsigned long MoveTo(unsigned long &Position_in_out, long Offset_in) =0;

		/*! \brief Resets the internal position to its initial value
			\return true if the position was reset; false otherwise
		*/
		virtual bool Reset() =0;

		/*! \brief Retrieves the size of the ZIP data
			\return the size of the ZIP data in bytes
		*/
		unsigned long GetSize() const;

		/*! \brief Retrieves the amount of ZIP data available in bytes
			\return the amount of ZIP data available in bytes
		*/
		unsigned long GetAvailableData() const;

		/*! \brief Retrieves the position int the ZIP data
			\return the position int the ZIP data
		*/
		unsigned long GetPosition() const;

	protected:
		//! the size of the ZIP data in bytes
		unsigned long m_DataSize;
		//! the amount of ZIP data available in bytes
		unsigned long m_AvailableData;
		//! the internal position in the ZIP data
		unsigned long m_Position;
	};
}

#endif//__IZIPDATA_H__
