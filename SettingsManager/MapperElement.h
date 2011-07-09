/**************************************************************************
	created		:	2011-06-20
	filename	: 	MapperElement.h
	author		:	Xebeth`
	copyright	:	North Edge (2011)
	purpose		:	
**************************************************************************/
#ifndef __MAPPER_ELEMENT_H__
#define __MAPPER_ELEMENT_H__

namespace XMLConfig
{
	enum XMLDataType
	{
		DATA_TYPE_UNKNOWN = -1,

		DATA_TYPE_ROOT_ELEMENT,
		DATA_TYPE_COLLECTION,
		DATA_TYPE_STRING,
		DATA_TYPE_FLOAT,
		DATA_TYPE_LONG,
		DATA_TYPE_BOOL,
		
		DATA_TYPE_COUNT
	};

	enum XMLElementStatus
	{
		ELEMENT_STATUS_UNSET = 0,
		ELEMENT_STATUS_SET,
		ELEMENT_STATUS_MODIFIED,
		ELEMENT_STATUS_COUNT
	};

	class MapperAttribute
	{
	public:
		typedef stdext::hash_map<string_t, MapperAttribute*>  MapperAttributes;

		string_t	m_Name;
		string_t	m_Value;
		string_t	m_Element;
	};

	typedef MapperAttribute::MapperAttributes MapperAttributes;

	class MapperElement
	{
	public:
		MapperElement(const string_t &Name_in, const string_t &Tag_in, int Level_in,
					  MapperElement *pParent_in, const TCHAR *pStringValue_in);
		MapperElement(const string_t &Name_in, const string_t &Tag_in, int Level_in,
					  MapperElement *pParent_in, const float *pFloatValue_in);
		MapperElement(const string_t &Name_in, const string_t &Tag_in, int Level_in,
					  MapperElement *pParent_in, const long *pLongValue_in);
		MapperElement(const string_t &Name_in, const string_t &Tag_in, int Level_in,
					  MapperElement *pParent_in, const bool *pBoolValue_in);
		MapperElement(const string_t &Name_in, const string_t &Tag_in, int Level_in,
					  MapperElement *pParent_in);
		MapperElement();
		~MapperElement();

		typedef stdext::hash_map<string_t, MapperElement*> MapperElements;

		string_t				m_Name;
		string_t				m_Tag;
		int						m_Level;
		MapperElements		   *m_pChildren;
		XMLDataType				m_DataType;
		MapperElement		   *m_pParent;
		void				   *m_pData;
		XMLElementStatus		m_Status;
		MapperAttributes	   *m_pAttributes;		
	};

	typedef MapperElement::MapperElements MapperElements;
}

#endif//__MAPPER_ELEMENT_H__