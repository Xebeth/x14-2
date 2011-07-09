/**************************************************************************
	created		:	2011-06-18
	filename	: 	XMLMapper.cpp
	author		:	Xebeth`
	copyright	:	North Edge (2011)
	purpose		:	
**************************************************************************/
#include "stdafx.h"
#include <hash_map>
#include <stack>

#include "MapperElement.h"
#include "XMLMapper.h"

namespace XMLConfig
{
	const string_t& XMLMapper::GetXML()
	{
		if (m_XMLData.empty())
		{
			m_XMLData = _T("<?xml version=\"1.0\" encoding=\"UTF-8\" ?>\n");
			GetCollectionXML(&m_Root);
		}

		return m_XMLData;
	}

	void XMLMapper::GetCollectionXML(const MapperElement* pCollection_in)
	{
		if (pCollection_in != NULL && pCollection_in->m_DataType <= DATA_TYPE_COLLECTION)
		{
			if (pCollection_in->m_pChildren != NULL && pCollection_in->m_pChildren->empty() == false)
			{
				MapperElements::const_iterator Iter;
	
				// opening tag
				for (int i = 0; i < pCollection_in->m_Level; i++)
					append_format(m_XMLData, _T("\t"));

				if (pCollection_in->m_Tag.compare(pCollection_in->m_Name) != 0 && pCollection_in->m_Name.empty() == false)
				{
					append_format(m_XMLData, _T("<%s ID=\"%s\">\n"),
								  pCollection_in->m_Tag.c_str(),
								  pCollection_in->m_Name.c_str());
				}
				else
					append_format(m_XMLData, _T("<%s>\n"), pCollection_in->m_Tag.c_str());
				
				for (Iter = pCollection_in->m_pChildren->begin(); Iter != pCollection_in->m_pChildren->end(); ++Iter)
				{
					if (Iter->second->m_DataType == DATA_TYPE_COLLECTION)
						GetCollectionXML(Iter->second);
					else
						GetElementXML(Iter->second);
				}
				// closing tag
				for (int i = 0; i < pCollection_in->m_Level; i++)
					append_format(m_XMLData, _T("\t"));

				append_format(m_XMLData, _T("</%s>\n"), pCollection_in->m_Tag.c_str());
			}
			else
			{
				// closing tag
				for (int i = 0; i < pCollection_in->m_Level; i++)
					append_format(m_XMLData, _T("\t"));

				if (pCollection_in->m_Tag.compare(pCollection_in->m_Name) != 0 && pCollection_in->m_Name.empty() == false)
				{
					append_format(m_XMLData, _T("<%s ID=\"%s\" />\n"),
								  pCollection_in->m_Tag.c_str(),
								  pCollection_in->m_Name.c_str());
				}
				else
					append_format(m_XMLData, _T("<%s />\n"), pCollection_in->m_Tag.c_str());
			}
		}
	}

	void XMLMapper::GetElementXML(const MapperElement* pElement_in)
	{
		if (pElement_in != NULL)
		{
			// closing tag
			for (int i = 0; i < pElement_in->m_Level; i++)
				append_format(m_XMLData, _T("\t"));

			switch(pElement_in->m_DataType)
			{
				case DATA_TYPE_STRING:
					append_format(m_XMLData, _T("<%s>%s</%s>\n"), pElement_in->m_Tag.c_str(),
								  (TCHAR*)pElement_in->m_pData, pElement_in->m_Tag.c_str());
				break;
				case DATA_TYPE_FLOAT:
					append_format(m_XMLData, _T("<%s>%g</%s>\n"), pElement_in->m_Tag.c_str(),
								  *((float*)pElement_in->m_pData) , pElement_in->m_Tag.c_str());
				break;
				case DATA_TYPE_LONG:
					append_format(m_XMLData, _T("<%s>%ld</%s>\n"), pElement_in->m_Tag.c_str(),
								  *((long*)pElement_in->m_pData) , pElement_in->m_Tag.c_str());
				break;
				case DATA_TYPE_BOOL:
					append_format(m_XMLData, _T("<%s>%s</%s>\n"), pElement_in->m_Tag.c_str(),
								  *((bool*)pElement_in->m_pData) ? _T("true") : _T("false"),
								  pElement_in->m_Tag.c_str());
				break;
			}
		}
	}

	void XMLMapper::InsertChild(MapperElement *pChild_in)
	{
		// only insert children in collections or the root
		if (pChild_in != NULL && pChild_in->m_pParent != NULL
		 && (pChild_in->m_pParent->m_DataType <= DATA_TYPE_COLLECTION))
		{
			// allocate the children vector of the parent
			if (pChild_in->m_pParent->m_pChildren == NULL)
				pChild_in->m_pParent->m_pChildren = new MapperElements;
			// insert the child in its parent
			pChild_in->m_pParent->m_pChildren->insert(std::make_pair(pChild_in->m_Name, pChild_in));
		}
	}

	// string member
	void XMLMapper::MapString(const string_t &Name_in, const string_t &StringValue_in)
	{
		if (m_Hierarchy.empty() == false)
		{
			PopHierarchy();
			InsertChild(new MapperElement(Name_in, Name_in, m_Hierarchy.size(), m_Hierarchy.top().first, StringValue_in.c_str()));
		}
	}

	// float member
	void XMLMapper::MapFloat(const string_t &Name_in, const float &FloatValue_in)
	{
		if (m_Hierarchy.empty() == false)
		{
			PopHierarchy();
			InsertChild(new MapperElement(Name_in, Name_in, m_Hierarchy.size(), m_Hierarchy.top().first, &FloatValue_in));
		}
	}

	// long member
	void XMLMapper::MapLong(const string_t &Name_in, const long &LongValue_in)
	{
		if (m_Hierarchy.empty() == false)
		{
			PopHierarchy();
			InsertChild(new MapperElement(Name_in, Name_in, m_Hierarchy.size(), m_Hierarchy.top().first, &LongValue_in));
		}
	}

	// bool member
	void XMLMapper::MapBool(const string_t &Name_in, const bool &BoolValue_in)
	{
		if (m_Hierarchy.empty() == false)
		{
			PopHierarchy();
			InsertChild(new MapperElement(Name_in, Name_in, m_Hierarchy.size(), m_Hierarchy.top().first, &BoolValue_in));
		}
	}

	// collection member
	void XMLMapper::MapCollection(size_t Size_in, const string_t &Name_in, const TCHAR *pID_in)
	{
		if (m_Hierarchy.empty() == false)
		{
			PopHierarchy();
			string_t ID;

			if (pID_in == NULL)
				pID_in = GenerateId(ID);

			MapperElement *pCollection = new MapperElement(pID_in ? pID_in : Name_in, Name_in, m_Hierarchy.size(), m_Hierarchy.top().first);
			InsertChild(pCollection);
			// if the collection we just inserted is empty, don't add it to the stack
			if (Size_in > 0)
				m_Hierarchy.push(std::make_pair(pCollection, Size_in));
		}
	}

	void XMLMapper::PopHierarchy()
	{
		// pop all the hierarchy levels which have reached their maximum size
		while (m_Hierarchy.empty() == false
			&& m_Hierarchy.top().first->m_pChildren != NULL 
			&& m_Hierarchy.top().first->m_pChildren->size() >= m_Hierarchy.top().second)
		{
			m_Hierarchy.pop();
		}
	}

	const TCHAR* XMLMapper::GenerateId(string_t &ID_out)
	{
		const TCHAR *pResult = NULL;

		if (m_Hierarchy.empty() == false)
		{
			MapperElement *pParent = m_Hierarchy.top().first;
			// start counting children at 0
			size_t ChildCount = pParent->m_pChildren ? pParent->m_pChildren->size() : 0;
			// generate an id from the tag of the parent and the number of children
			format(ID_out, _T("%s-%ld"), m_Hierarchy.top().first->m_Tag.c_str(), ChildCount);
			pResult = ID_out.c_str();
		}

		return pResult;
	}

	bool XMLMapper::CheckHierarchy()
	{
		// pop the remaining levels
		PopHierarchy();
		// the last remaining item should be the root
		return (m_Hierarchy.size() == 1 && m_Hierarchy.top().first->m_DataType == DATA_TYPE_ROOT_ELEMENT);
	}
}
