#include "stdafx.h"

#include "Timetable.h"
#include "tinyxml2.h"

namespace Windower
{
	TimetableEntry::ItemData::ItemData(const std::string &zone, const std::string &position,
									   const std::string &item, const std::string &slot,
									   const std::string &difficulty)
		: m_Zone(zone), m_Position(position), m_Item(item),
		  m_Slot(slot), m_Difficulty(difficulty) {}

	Timetable::~Timetable()
	{
		GatheringData::const_iterator entryIt, endEntryIt = m_Data.cend();

		for (entryIt = m_Data.cbegin(); entryIt != endEntryIt; ++entryIt)
			delete entryIt->second;

		m_Data.clear();
	}

	Timetable::Timetable(tinyxml2::XMLDocument *pDoc_in)
	{
		if (pDoc_in != NULL)
		{
			tinyxml2::XMLElement *pElement = pDoc_in->FirstChildElement("document");

			if (pElement != NULL)
			{
				TimetableEntry *pEntry = NULL;

				for (tinyxml2::XMLElement* pTime = pElement->FirstChildElement(); pTime != NULL; pTime = pTime->NextSiblingElement())
				{
					pEntry = new TimetableEntry(pElement);

					if (pEntry != NULL)
						m_Data[pEntry->GetTime()] = pEntry;
				}
			}
		}
	}

	TimetableEntry::TimetableEntry(tinyxml2::XMLElement *pElement_in)
	{
		if (pElement_in != NULL)
		{
			std::string item, position, zone, difficulty, slot;
			ItemData *pEntry;

			m_Duration = pElement_in->IntAttribute("duration");
			m_Hour = pElement_in->IntAttribute("hour");

			for (tinyxml2::XMLElement* pItem = pElement_in->FirstChildElement("item"); pItem != NULL; pItem = pItem->NextSiblingElement())
			{
				difficulty = pItem->Attribute("difficulty");
				position = pItem->Attribute("pos");
				zone = pItem->Attribute("zone");
				slot = pItem->Attribute("slot");
				item = pItem->GetText();

				pEntry = new ItemData(zone, position, item, slot, difficulty);
				m_Entries.push_back(pEntry);
			}
		}
	}

	TimetableEntry::~TimetableEntry()
	{
		ItemEntries::const_iterator entryIt, endEntryIt = m_Entries.cend();

		for (entryIt = m_Entries.cbegin(); entryIt != endEntryIt; ++entryIt)
			delete *entryIt;

		m_Entries.clear();
	}
}