/**************************************************************************
	created		:	2011-06-20
	filename	: 	MapperIterator.h
	author		:	Xebeth`
	copyright	:	North Edge (2011)
	purpose		:	
**************************************************************************/
#ifndef __MAPPER_ITERATOR_H__
#define __MAPPER_ITERATOR_H__

namespace XMLConfig
{
	class MapperIterator
	{
	public:
		MapperIterator(const MapperElement *pRoot_in, int Limit_in = 0);
		const MapperElement* Next();

	protected:
		bool _Down(const MapperElement *pCurrent_in);

		class ParentLevel
		{
		public:
			ParentLevel(const MapperElements::iterator &Current_in, const MapperElements::iterator &End_in)
				: m_CurrentIter(Current_in), m_End(End_in) {}
			ParentLevel() : m_Set(false) {}

			bool IsValid() { return m_Set && m_CurrentIter != m_End; }
			void Set(const MapperElements::iterator &Current_in, const MapperElements::iterator &End_in)
				{ m_CurrentIter = Current_in; m_End = End_in; m_Set = true; }
			void Get(MapperElements::iterator &Current_out, MapperElements::iterator &End_out)
				{ Current_out = m_CurrentIter; End_out = m_End; }
			const MapperElements::iterator & GetCurrent() { return m_CurrentIter; }

		protected:
			MapperElements::iterator m_CurrentIter;
			MapperElements::iterator m_End;
			bool m_Set;
		};

		typedef std::stack<ParentLevel> ParentLevels;

		typedef std::pair<MapperElements::iterator,MapperElements::iterator> IteratorRange;

		MapperElements::iterator m_CurrentIter;
		MapperElements::iterator m_End;
		const MapperElement *m_pRoot;
		ParentLevels m_ParentLevels;
		int m_HierarchyLimit;
		int m_CurrentLevel;
		bool m_EndReached;
	};
}

#endif//__MAPPER_ITERATOR_H__