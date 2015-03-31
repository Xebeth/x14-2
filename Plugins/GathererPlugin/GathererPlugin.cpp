/**************************************************************************
	created		:	2011-06-04
	filename	: 	GathererPlugin.cpp
	author		:	Xebeth`
	copyright	:	North Edge (2011)
	purpose		:	Gatherer plugin
**************************************************************************/
#include "stdafx.h"
#include "version.h"
#include "resource.h"
#include "tinyxml2.h"

#include <ctime>
#include <chrono>

#include "Timetable.h"
#include "GathererPlugin.h"

namespace Windower
{
	bool LoadFileInResource(HMODULE handle, int name, LPWSTR type, DWORD& size, const char*& data)
	{
		if (handle != NULL)
		{
			LPWSTR nameStr = MAKEINTRESOURCE(name);
			HRSRC rc = ::FindResource(handle, nameStr, type);

			if (rc != NULL)
			{
				HGLOBAL rcData = ::LoadResource(handle, rc);

				if (rc != NULL)
				{
					size = ::SizeofResource(handle, rc);
					data = static_cast<const char*>(::LockResource(rcData));

					return true;
				}
			}
		}

		return false;
	}

	/*! \brief GathererPlugin constructor
		\param[in] pServices_in : a pointer to the plugin services
	*/
	GathererPlugin::GathererPlugin(PluginFramework::IPluginServices *pServices_in)
	  : WindowerPlugin(pServices_in), m_pTextLabel(NULL),
	    m_DisplayedTable(m_MiningTimetable) {}

	//! \brief GathererPlugin destructor
	GathererPlugin::~GathererPlugin()
	{
		CleanupTimetable(m_BotanyTimetable);
		CleanupTimetable(m_MiningTimetable);

		if (m_pTextLabel != NULL)
		{
			LabelServiceParam LabelParam(&m_pTextLabel);
			LabelParam.m_Destroy = true;
			PluginFramework::ServiceParam Param(_T("LabelServiceParam"), &LabelParam);

			if (InvokeService(_T(GRAPHICS_MODULE), _T(TEXT_LABEL_SERVICE), Param) == false)
				m_pTextLabel = NULL;
		}
	}

	/*! \brief Creates an instance of GathererPlugin
		\return a pointer to the new GathererPlugin instance
	*/
	PluginFramework::IPlugin* GathererPlugin::Create(PluginFramework::IPluginServices *pServices_in)
	{
		return new GathererPlugin(pServices_in);
	}

	/*! \brief Destroys an instance of GathererPlugin
		\param[in] pInstance_in : an instance of GathererPlugin
	*/
	void GathererPlugin::Destroy(IPlugin *pInstance_in)
	{
		if (pInstance_in != NULL)
		{
			delete pInstance_in;
			pInstance_in = NULL;
		}
	}

	/*! \brief Fills a VersionInfo structure with the plugin information
		\param[out] Info_out : a VersionInfo structure
	*/
	void GathererPlugin::Query(PluginFramework::PluginInfo& PluginInfo_out)
	{
		PluginInfo_out.SetDesc(_T("This plugin will provide information on unspoiled nodes."));
		PluginInfo_out.SetIdentifier(_T("61933EE0-3F0D-4EE4-B69E-C38B597293F0"));
		PluginInfo_out.SetCompatibilityFlags(PLUGIN_COMPATIBILITY_WINDOWER);
		PluginInfo_out.SetVersion(PLUGIN_VERSION);
		PluginInfo_out.SetAuthor(_T("Xebeth`"));
		PluginInfo_out.SetName(_T("Gatherer"));
	}

	bool GathererPlugin::Update()
	{
		if (m_pTextLabel == NULL)
			CreateLabel();

		if (m_pTextLabel != NULL)
		{
			string_t message;
			tm time = { 0 }, local = { 0 };
			auto timestamp = std::chrono::seconds(std::time(NULL));
			time_t localTime = timestamp.count(), eorzeanTime = (time_t)std::round(localTime * TIME_RATIO);

			if (gmtime_s(&time, &eorzeanTime) == 0 && localtime_s(&local, &localTime) == 0)
			{
				Timetable::const_iterator beginTimeIt = m_DisplayedTable.cbegin();
				Timetable::const_iterator endTimeIt = m_DisplayedTable.cend();
				Timetable::const_iterator timeIt, lastIt = beginTimeIt;
				string_t labelText, tempTitle, tempET;
				TCHAR eorzeanTimeStr[7] = { '\0' };
				TCHAR localTimeStr[9] = { '\0' };
				int first, hour = time.tm_hour;				
				std::string tempZone, tempItem;
				
				_tcsftime(eorzeanTimeStr, 7, _T("%H:%M"), &time);
				_tcsftime(localTimeStr, 9, _T("%H:%M:%S"), &local);
				
				for (timeIt = lastIt; timeIt != endTimeIt; ++timeIt)
				{
					first = timeIt->first;
					
					if (first > hour - 3 && first <= hour)
					{
						DisplayEntries(m_DisplayedTable[first], first, time, labelText);
						lastIt = timeIt;
					}
				}

				if (labelText.empty() == false)
					++lastIt;

				if (lastIt == endTimeIt)
					lastIt = beginTimeIt;

				DisplayEntries(lastIt->second, -1, time, labelText);				
				format(message, _T("%s%s %s\n\n%s"), pad<TCHAR>(_T("Gatherer"), tempTitle, 47).c_str(),
													 pad<TCHAR>(eorzeanTimeStr, tempET, 42).c_str(),
													 localTimeStr, labelText.c_str());
				m_pTextLabel->SetTitleText(message);
			}			
		}

		return false;
	}

	void GathererPlugin::InitTimeTable(DWORD ResID, Timetable &timetable)
	{
		const char* data = NULL;
		DWORD size = 0;

		if (LoadFileInResource(GetHandle(), IDR_MINING, XML, size, data) && data != NULL)
		{
			tinyxml2::XMLDocument doc;

			if (doc.Parse(data) == tinyxml2::XML_NO_ERROR)
			{
				tinyxml2::XMLElement *pElement = doc.FirstChildElement("document");

				if (pElement != NULL)
				{
					std::string item, position, zone, difficulty, slot;
					TimetableEntry *pEntry;
					int hours;

					for (tinyxml2::XMLElement* pTime = pElement->FirstChildElement(); pTime != NULL; pTime = pTime->NextSiblingElement())
					{
						hours = pTime->IntAttribute("hours");

						for (tinyxml2::XMLElement* pItem = pTime->FirstChildElement("item"); pItem != NULL; pItem = pItem->NextSiblingElement())
						{
							difficulty = pItem->Attribute("difficulty");
							position = pItem->Attribute("pos");
							zone = pItem->Attribute("zone");
							slot = pItem->Attribute("slot");
							item = pItem->GetText();

							

							pEntry = new TimetableEntry(hours, zone, position, item, slot, difficulty);
							timetable[hours].push_back(pEntry);
						}
					}
				}
			}
		}
	}

	void GathererPlugin::CleanupTimetable(Timetable &timetable)
	{
		Timetable::const_iterator vecIt, endVecIt = timetable.cend();
		TimetableEntries::const_iterator entryIt, endEntryIt;

		for (vecIt = timetable.cbegin(); vecIt != endVecIt; ++vecIt)
		{
			endEntryIt = vecIt->second.cend();

			for (entryIt = vecIt->second.cbegin(); entryIt != endEntryIt; ++entryIt)
				delete *entryIt;
		}

		timetable.clear();
	}

	bool GathererPlugin::CreateLabel()
	{
		if (m_pTextLabel == NULL)
		{
			LabelServiceParam LabelParam(&m_pTextLabel, GATHERER_LABEL_NAME, 5L, 5L, 640UL, 0UL);
			PluginFramework::ServiceParam Param(_T("LabelServiceParam"), &LabelParam);

			if (InvokeService(_T(GRAPHICS_MODULE), _T(TEXT_LABEL_SERVICE), Param) == false)
				m_pTextLabel = NULL;
		}

		InitTimeTable(IDR_MINING, m_MiningTimetable);

		return (m_pTextLabel != NULL);
	}

	void GathererPlugin::DisplayEntries(const TimetableEntries &entries_in, int hours,
										const tm &time_in, string_t &message_out) const
	{
		TimetableEntries::const_iterator entryIt, endIt = entries_in.cend();
		std::string tempZone, tempItem, tempTimer, tempPos;
		char remainingTimeStr[7] = "--:--";
		tm remaining = { 0 };

		if (hours != -1)
		{
			remaining.tm_hour = 3 - (time_in.tm_hour - hours) - 1;
			remaining.tm_min = 60 - time_in.tm_min - 1;
			strftime(remainingTimeStr, 7, "%H:%M", &remaining);
		}

		for (entryIt = entries_in.cbegin(); entryIt != endIt; ++entryIt)
		{
			const TimetableEntry *pEntry = (*entryIt);

			append_format(message_out, _T("%02i:00 (%S) %S %S %S Slot %S\t%S\n"),
									   pEntry->m_Hour, 
									   pad<char>(remainingTimeStr, tempTimer, 6).c_str(),
									   pad(pEntry->m_Zone, tempZone, 32).c_str(),
									   pad(pEntry->m_Position, tempPos, 10).c_str(),
									   pad(pEntry->m_Item, tempItem, 32).c_str(),
									   pEntry->m_Slot.c_str(),
									   pEntry->m_Difficulty.c_str());
		}
	}

}

using Windower::GathererPlugin;

/*! \brief Function exposed by the plugin DLL to initialize the plugin object
	\param[out] RegisterParams_out : Registration structure to be able to use the plugin
	\return true if the initialization succeeded; false otherwise
*/
extern "C" PLUGIN_API bool InitPlugin(PluginFramework::RegisterParams &RegisterParams_out)
{
	return PluginFramework::IPlugin::Initialize(RegisterParams_out, GathererPlugin::Create, 
												GathererPlugin::Destroy, GathererPlugin::Query, NULL);
}