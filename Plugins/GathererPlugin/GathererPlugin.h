/**************************************************************************
	created		:	2011-06-04
	filename	: 	GathererPlugin.h
	author		:	Xebeth`
	copyright	:	North Edge (2011)
	purpose		:	Gatherer plugin
**************************************************************************/
#ifndef __GATHERER_PLUGIN_H__
#define __GATHERER_PLUGIN_H__

#define GRAPHICS_MODULE			"Graphics"
#define TEXT_LABEL_SERVICE		"TextLabelService"
#define GATHERER_LABEL_NAME		_T("Gatherer##Label")
#define XML _T("XML")

#define TIME_RATIO 3600. / 175.

namespace Windower
{
	class GathererPlugin : public WindowerPlugin
	{
	public:
		explicit GathererPlugin(PluginFramework::IPluginServices *pServices_in);
		~GathererPlugin();

		static PluginFramework::IPlugin* Create(PluginFramework::IPluginServices *pServices_in);
		static void Destroy(PluginFramework::IPlugin *pInstance_in);
		static void Query(PluginFramework::PluginInfo& PluginInfo_out);

		virtual bool Update();

	protected:
		void InitTimeTable(DWORD ResID, Timetable &timetable);
		void CleanupTimetable(Timetable &timetable);		
		bool CreateLabel();
		void DisplayEntries(const TimetableEntries &entries_in, int hours,
						    const tm &time_in, string_t &message_out) const;
		UIAL::CUiWindow<> *m_pTextLabel;
		Timetable m_BotanyTimetable;
		Timetable m_MiningTimetable;
		Timetable &m_DisplayedTable;
	};
}

#endif//__GATHERER_PLUGIN_H__