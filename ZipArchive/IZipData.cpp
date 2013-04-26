/**************************************************************************
	created		:	2013-04-25
	filename	: 	IZipData.cpp
	author		:	Xebeth`
	copyright	:	North Edge (2013)
	purpose		:	ZIP data container interface
**************************************************************************/
#include "stdafx.h"

#include "IZipData.h"

namespace ZipArchiveLib
{
	IZipData::IZipData(unsigned long DataSize_in)
		: m_DataSize(DataSize_in), 
		  m_AvailableData(0UL),
		  m_Position(0UL) {}

	IZipData::~IZipData() {}

	unsigned long IZipData::GetPosition() const
	{ return m_Position; }

	unsigned long IZipData::GetSize() const
	{ return m_DataSize; }

	unsigned long IZipData::GetAvailableData() const
	{ return m_AvailableData; }
}