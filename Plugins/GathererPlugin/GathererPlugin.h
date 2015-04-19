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
#define MINING_LABEL_NAME		_T("Gatherer#Mining##Label")
#define BOTANY_LABEL_NAME		_T("Gatherer#Botany##Label")
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
		bool CreateLabels();
		void DisplayEntries(const TimetableEntries &entries_in, int hours,
							const tm &time_in, string_t &message_out, bool active) const;
		void DisplayTimeTable(Timetable &timetable, UIAL::CUiWindow<> *pLabel,
							  const TCHAR *pTitle, const tm &ezTime, const tm &local);
		UIAL::CUiWindow<> *m_pBotanyLabel;
		UIAL::CUiWindow<> *m_pMiningLabel;
		Timetable m_BotanyTimetable;
		Timetable m_MiningTimetable;
	};
}

#endif//__GATHERER_PLUGIN_H__