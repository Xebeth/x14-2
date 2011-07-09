/**************************************************************************
	created		:	2011-06-18
	filename	: 	XMLMapper.h
	author		:	Xebeth`
	copyright	:	North Edge (2011)
	purpose		:	
**************************************************************************/
#ifndef __XML_MAPPER_H__
#define __XML_MAPPER_H__

namespace XMLConfig
{
	class XMLMapper
	{
	public:
		XMLMapper(const string_t &Name_in) : m_Root(Name_in, Name_in, 0, NULL)
			{ m_Root.m_DataType = DATA_TYPE_ROOT_ELEMENT;
			  m_Hierarchy.push(std::make_pair(&m_Root, -1)); }

		void DumpElement(const MapperElement* pElement_in);
		void Dump();

		const string_t& GetXML();

		const MapperElement& GetRoot() const { return m_Root; }

		// string
		void MapString(const string_t &Name_in, const string_t &StringValue_in);
		// float
		void MapFloat(const string_t &Name_in, const float &FloatValue_in);
		// long
		void MapLong(const string_t &Name_in, const long &LongValue_in);
		// bool
		void MapBool(const string_t &Name_in, const bool &BoolValue_in);
		// collection
		void MapCollection(size_t Size_in, const string_t &Name_in, const TCHAR *pID_in = NULL);

		void MapAttribute(const string_t &Element_in, const string_t &Name_in, string_t &StringValue_in);

		bool CheckHierarchy();

	protected:
		XMLMapper(const MapperElement &Root_in) : m_Root(Root_in) {}
		void InsertChild(MapperElement *pChild_in);
		const TCHAR* GenerateId(string_t &ID_out);
		void PopHierarchy();

		void GetCollectionXML(const MapperElement* pCollection_in);
		void GetElementXML(const MapperElement* pElement_in);

		typedef std::stack<std::pair<MapperElement*, size_t> > ElementStack;
		ElementStack		m_Hierarchy;
		string_t			m_XMLData;
		MapperElement		m_Root;
	};
}

#endif//__XML_MAPPER_H__