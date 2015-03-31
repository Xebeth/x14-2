/**************************************************************************
created		:	2011-06-04
filename	: 	Timetable.h
author		:	Xebeth`
copyright	:	North Edge (2011)
purpose		:	Gatherer plugin
**************************************************************************/
#ifndef __TIMETABLE_H__
#define __TIMETABLE_H__

namespace Windower
{
	class TimetableEntry
	{
	public:
		TimetableEntry();
		TimetableEntry(int hour, const std::string &zone, const std::string &position,
					   const std::string &item, const std::string &slot,
					   const std::string &difficulty);

		int m_Hour;
		std::string m_Zone;
		std::string m_Position;
		std::string m_Item;
		std::string m_Slot;
		std::string m_Difficulty;
	};

	typedef std::vector<TimetableEntry*> TimetableEntries;
	typedef std::map<int, TimetableEntries> Timetable;
}

#endif