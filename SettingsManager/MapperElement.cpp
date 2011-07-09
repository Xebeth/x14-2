/**************************************************************************
	created		:	2011-06-20
	filename	: 	MapperElement.cpp
	author		:	Xebeth`
	copyright	:	North Edge (2011)
	purpose		:	
**************************************************************************/
#include "stdafx.h"
#include <hash_map>
#include "MapperElement.h"

namespace XMLConfig
{
	MapperElement::MapperElement()
		: m_pData(NULL), m_pChildren(NULL), m_pParent(NULL),
		  m_Status(ELEMENT_STATUS_UNSET), m_DataType(DATA_TYPE_UNKNOWN) {}

	MapperElement::MapperElement(const string_t &Name_in, const string_t &Tag_in, int Level_in, MapperElement *pParent_in)
		: m_Name(Name_in), m_pData(NULL), m_pChildren(NULL), m_pParent(pParent_in), m_Tag(Tag_in), m_Level(Level_in),
		  m_pAttributes(NULL), m_Status(ELEMENT_STATUS_SET), m_DataType(DATA_TYPE_COLLECTION) {}

	MapperElement::MapperElement(const string_t &Name_in, const string_t &Tag_in, int Level_in, MapperElement *pParent_in, const TCHAR *pStringValue_in)
		: m_Name(Name_in), m_pData((void*)pStringValue_in), m_pChildren(NULL), m_pParent(pParent_in), m_Level(Level_in),
		  m_Tag(Tag_in), m_pAttributes(NULL), m_Status(ELEMENT_STATUS_SET), m_DataType(DATA_TYPE_STRING) {}

	MapperElement::MapperElement(const string_t &Name_in, const string_t &Tag_in, int Level_in, MapperElement *pParent_in, const float *pFloatValue_in)
		: m_Name(Name_in), m_pData((void*)pFloatValue_in), m_pChildren(NULL), m_pParent(pParent_in), m_Level(Level_in),
		  m_Tag(Tag_in), m_pAttributes(NULL), m_Status(ELEMENT_STATUS_SET), m_DataType(DATA_TYPE_FLOAT) {}

	MapperElement::MapperElement(const string_t &Name_in, const string_t &Tag_in, int Level_in, MapperElement *pParent_in, const long *pLongValue_in)
		: m_Name(Name_in), m_pData((void*)pLongValue_in), m_pChildren(NULL), m_pParent(pParent_in), m_Level(Level_in),
		  m_Tag(Tag_in), m_pAttributes(NULL), m_Status(ELEMENT_STATUS_SET), m_DataType(DATA_TYPE_LONG) {}

	MapperElement::MapperElement(const string_t &Name_in, const string_t &Tag_in, int Level_in, MapperElement *pParent_in, const bool *pBoolValue_in)
		: m_Name(Name_in), m_pData((void*)pBoolValue_in), m_pChildren(NULL), m_pParent(pParent_in), m_Level(Level_in),
		  m_Tag(Tag_in), m_pAttributes(NULL), m_Status(ELEMENT_STATUS_SET), m_DataType(DATA_TYPE_BOOL) {}

	MapperElement::~MapperElement()
	{
		m_pParent = NULL;

		if (m_pChildren != NULL)
		{
			MapperElements::iterator ElementIter;

			while (m_pChildren->empty() == false)
			{
				ElementIter = m_pChildren->begin();

				delete ElementIter->second;
				ElementIter->second = NULL;

				m_pChildren->erase(ElementIter);
			}

			delete m_pChildren;
			m_pChildren = NULL;
		}

		if (m_pAttributes != NULL)
		{
			MapperAttributes::iterator AttributeIter;

			while (m_pAttributes->empty() == false)
			{
				AttributeIter = m_pAttributes->begin();

				delete AttributeIter->second;
				AttributeIter->second = NULL;

				m_pAttributes->erase(AttributeIter);
			}

			delete m_pAttributes;
			m_pAttributes = NULL;
		}
	}
}