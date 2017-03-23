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
	static void WINAPI OnGathererLabelClick(UIAL::CUiWindow<> *pLabel, WORD X_in, WORD Y_in, DWORD MouseFlags_in, UINT KeyFlags_in)
	{
		if (pLabel != NULL && (KeyFlags_in & 0x02U) == 0x02U) // CTRL
		{
			bool collapsed = !pLabel->IsCollapsed();

			pLabel->SetWidth(collapsed ? 100UL : 640UL);
			pLabel->SetCollapsed(collapsed);			
		}
	}

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
	  : WindowerPlugin(pServices_in), m_pMiningLabel(NULL), m_pBotanyLabel(NULL) {}

	//! \brief GathererPlugin destructor
	GathererPlugin::~GathererPlugin()
	{
		if (m_pBotanyTimetable != NULL)
		{
			delete m_pBotanyTimetable;
			m_pBotanyTimetable = NULL;
		}

		if (m_pMiningTimetable != NULL)
		{
			delete m_pMiningTimetable;
			m_pMiningTimetable = NULL;
		}

		if (m_pMiningLabel != NULL)
		{
			LabelServiceParam LabelParam(&m_pMiningLabel);
			LabelParam.m_Destroy = true;
			PluginFramework::ServiceParam Param(_T("LabelServiceParam"), &LabelParam);

			if (InvokeService(_T(GRAPHICS_MODULE), _T(TEXT_LABEL_SERVICE), Param) == false)
				m_pMiningLabel = NULL;
		}

		if (m_pBotanyLabel != NULL)
		{
			LabelServiceParam LabelParam(&m_pBotanyLabel);
			LabelParam.m_Destroy = true;
			PluginFramework::ServiceParam Param(_T("LabelServiceParam"), &LabelParam);

			if (InvokeService(_T(GRAPHICS_MODULE), _T(TEXT_LABEL_SERVICE), Param) == false)
				m_pBotanyLabel = NULL;
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

	Timetable* GathererPlugin::CreateTimetableFromRes(DWORD ResID)
	{
		const char* data = NULL;
		DWORD size = 0;

		if (LoadFileInResource(GetHandle(), ResID, XML, size, data) && data != NULL)
		{
			tinyxml2::XMLDocument doc;

			if (doc.Parse(data) == tinyxml2::XML_NO_ERROR)
				return new Timetable(&doc);
		}

		return NULL;
	}

	bool GathererPlugin::CreateLabels()
	{
		if (m_pMiningLabel == NULL)
		{
			LabelServiceParam LabelParam(&m_pMiningLabel, MINING_LABEL_NAME, 5L, 5L, 640UL, 0UL);
			PluginFramework::ServiceParam Param(_T("LabelServiceParam"), &LabelParam);

			LabelParam.m_pfnMouseClick = OnGathererLabelClick;

			if (InvokeService(_T(GRAPHICS_MODULE), _T(TEXT_LABEL_SERVICE), Param) == false)
				m_pMiningLabel = NULL;
		}

		CreateTimetableFromRes(IDR_MINING, m_pMiningTimetable);

		if (m_pBotanyLabel == NULL)
		{
			LabelServiceParam LabelParam(&m_pBotanyLabel, BOTANY_LABEL_NAME, 5L, 25L, 640UL, 0UL);
			PluginFramework::ServiceParam Param(_T("LabelServiceParam"), &LabelParam);

			LabelParam.m_pfnMouseClick = OnGathererLabelClick;

			if (InvokeService(_T(GRAPHICS_MODULE), _T(TEXT_LABEL_SERVICE), Param) == false)
				m_pBotanyLabel = NULL;
		}

		CreateTimetableFromRes(IDR_BOTANY, m_pBotanyTimetable);

		return (m_pMiningLabel != NULL && m_pBotanyLabel != NULL);
	}


	bool GathererPlugin::Update()
	{
		if ((m_pMiningLabel != NULL && m_pBotanyLabel != NULL) || CreateLabels())
		{
			tm ezTime = { 0 }, local = { 0 };
			auto timestamp = std::chrono::seconds(std::time(NULL));
			time_t localTime = timestamp.count(), eorzeanTime = (time_t)std::round(localTime * TIME_RATIO);

			if (gmtime_s(&ezTime, &eorzeanTime) == 0 && localtime_s(&local, &localTime) == 0)
			{
				if (m_pMiningTimetable.empty() == false)
					DisplayTimeTable(m_pMiningTimetable, m_pMiningLabel, _T("Mining"), ezTime, local);
				if (m_pBotanyTimetable.empty() == false)
					DisplayTimeTable(m_pBotanyTimetable, m_pBotanyLabel, _T("Botany"), ezTime, local);
			}
		}

		return false;
	}

	void GathererPlugin::DisplayTimeTable(const Timetable * pTimetable, UIAL::CUiWindow<> *pLabel,
										  const TCHAR *pTitle, const tm &ezTime, const tm &local)
	{
		if (pLabel != NULL)
		{
			string_t tempTitle, tempET, activeEntries, nextEntries;
			TCHAR eorzeanTimeStr[7] = { '\0' };
			TCHAR localTimeStr[9] = { '\0' };
			std::string tempZone, tempItem;
			string_t message;

			_tcsftime(eorzeanTimeStr, 7, _T("%H:%M"), &ezTime);
			_tcsftime(localTimeStr, 9, _T("%H:%M:%S"), &local);

			if (pLabel->IsCollapsed() == false)
			{
				GatheringData::const_iterator beginTimeIt = pTimetable->cbegin();
				GatheringData::const_iterator endTimeIt = pTimetable->cend();
				GatheringData::const_iterator timeIt, lastIt = endTimeIt;
				int current, hour = ezTime.tm_hour;

				format(message, _T("%s%s %s"), pad<TCHAR>(pTitle, tempTitle, 48).c_str(),
											   pad<TCHAR>(eorzeanTimeStr, tempET, 42).c_str(),
											   localTimeStr);
				// display active entries
				for (timeIt = beginTimeIt; timeIt != endTimeIt; ++timeIt)
				{
					current = timeIt->first;

					if (current > hour - 3 && current <= hour)
					{
						DisplayEntries(pTimetable[current], current, ezTime, activeEntries, true);
						lastIt = timeIt;
					}
				}

				if (activeEntries.empty())
				{
					for (timeIt = beginTimeIt; timeIt != endTimeIt; ++timeIt)
					{
						current = timeIt->first;

						if (current > hour)
						{
							lastIt = timeIt;
							break;
						}
					}
				}
				else
					++lastIt;
				// display the next time slot
				if (lastIt == endTimeIt)
					lastIt = beginTimeIt;

				DisplayEntries(lastIt->second, lastIt->first, ezTime, nextEntries, false);

				if (activeEntries.empty() == false)
					append_format(message, _T("\n\n%s"), activeEntries.c_str());

				if (nextEntries.empty() == false)
					append_format(message, _T("\n%s"), nextEntries.c_str());

				::replace<TCHAR>(message, _T("*"), _T("\u2605"));
			}
			else
				message = pTitle;

			pLabel->SetTitleText(message);
		}
	}

	void GathererPlugin::DisplayEntries(const ItemEntries &entries_in, int hours,
										const tm &time_in, string_t &message_out, bool active) const
	{
		ItemEntries::const_iterator entryIt, endIt = entries_in.cend();
		std::string tempZone, tempItem, tempTimer, tempPos;
		char remainingTimeStr[7] = "--:--";
		tm remaining = { 0 };

		if (active)
		{
			remaining.tm_hour = 3 - (time_in.tm_hour - hours) - 1;
			remaining.tm_min = 60 - time_in.tm_min - 1;
			strftime(remainingTimeStr, 7, "+%H:%M", &remaining);
		}
		else
		{
			remaining.tm_hour = ((24 + hours - time_in.tm_hour) % 24) - 1;
			remaining.tm_min = 60 - time_in.tm_min - 1;
			strftime(remainingTimeStr, 7, "-%H:%M", &remaining);
		}

		for (entryIt = entries_in.cbegin(); entryIt != endIt; ++entryIt)
		{
			const ItemData *pEntry = (*entryIt);

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