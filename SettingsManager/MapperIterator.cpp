/**************************************************************************
	created		:	2011-06-20
	filename	: 	MapperIterator.cpp
	author		:	Xebeth`
	copyright	:	North Edge (2011)
	purpose		:	
**************************************************************************/
#include "stdafx.h"
#include <hash_map>
#include <stack>

#include "MapperElement.h"
#include "MapperIterator.h"

namespace XMLConfig
{
	MapperIterator::MapperIterator(const MapperElement *pRoot_in, int Limit_in)
		: m_pRoot(pRoot_in), m_HierarchyLimit(Limit_in),
		  m_CurrentLevel(0), m_EndReached(false) {}

	const MapperElement* MapperIterator::Next()
	{
		const MapperElement *pResult = NULL;

		while (m_EndReached == false && pResult == NULL)
		{
			if (m_pRoot != NULL)
			{
				pResult = m_pRoot;
				m_pRoot = NULL;

				if (pResult->m_pChildren != NULL && pResult->m_pChildren->empty() == false)
				{
					// initialize the iterators with the first child
					m_CurrentIter = pResult->m_pChildren->begin();
					m_End = pResult->m_pChildren->end();
				}
				// if the root is empty => we reached the end
				else
					m_EndReached = true;
			}
			else if (m_CurrentIter != m_End)
			{
				pResult = m_CurrentIter->second;

				if (_Down(pResult) == false)
					++m_CurrentIter;
			}
			else if (m_ParentLevels.empty() == false)
			{
				// resume traversing the previous level
				m_ParentLevels.top().Get(m_CurrentIter, m_End);
				m_ParentLevels.pop();
			}
			else
				m_EndReached = true;
		}

		return pResult;
	}

	bool MapperIterator::_Down(const MapperElement *pCurrent_in)
	{
		if (pCurrent_in != NULL)
		{
			if (pCurrent_in->m_pChildren != NULL && pCurrent_in->m_pChildren->empty() == false)
			{
				if (m_CurrentIter != m_End)
				{
					if (++m_CurrentIter != m_End && m_HierarchyLimit > 0 && ++m_CurrentLevel < m_HierarchyLimit)
					{
						// going down => push the current element if it still has siblings
						m_ParentLevels.push(ParentLevel(m_CurrentIter, m_End));
					}

					// initialize the iterators with the first child
					m_CurrentIter = pCurrent_in->m_pChildren->begin();
					m_End = pCurrent_in->m_pChildren->end();

					return true;
				}
			}
		}

		return false;
	}
}