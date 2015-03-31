#include "stdafx.h"

#include "Timetable.h"

namespace Windower
{
	TimetableEntry::TimetableEntry() : m_Hour(0) {}

	TimetableEntry::TimetableEntry(int hour, const std::string &zone, const std::string &position,
								   const std::string &item, const std::string &slot, const std::string &difficulty)
		: m_Hour(hour), m_Zone(zone), m_Position(position), m_Item(item), m_Slot(slot), m_Difficulty(difficulty) {}
}