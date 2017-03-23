/**************************************************************************
created		:	2011-06-04
filename	: 	Timetable.h
author		:	Xebeth`
copyright	:	North Edge (2011)
purpose		:	Gatherer plugin
**************************************************************************/
#ifndef __TIMETABLE_H__
#define __TIMETABLE_H__

namespace tinyxml2
{
	class XMLDocument;
	class XMLElement;
}

namespace Windower
{
	class TimetableEntry;

	typedef std::map<int, TimetableEntry*> GatheringData;

	class Timetable
	{
	public:
		Timetable(tinyxml2::XMLDocument *pDoc);
		~Timetable();

		GatheringData m_Data;
	};

	class TimetableEntry
	{
	private:
		class ItemData
		{
		public:
			ItemData(const std::string &zone, const std::string &position,
				const std::string &item, const std::string &slot,
				const std::string &difficulty);

			std::string m_Zone;
			std::string m_Position;
			std::string m_Item;
			std::string m_Slot;
			std::string m_Difficulty;
		};

		typedef std::vector<ItemData*> ItemEntries;

	public:
		TimetableEntry(tinyxml2::XMLElement *pElement_in);
		~TimetableEntry();

		int GetDuration() const { return m_Duration; }
		int GetTime() const { return m_Hour; }

	protected:
		int m_Hour, m_Duration;
		ItemEntries m_Entries;
	};
}

#endif