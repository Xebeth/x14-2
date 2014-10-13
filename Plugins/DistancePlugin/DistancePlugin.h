/**************************************************************************
	created		:	2011-06-04
	filename	: 	DistancePlugin.h
	author		:	Xebeth`
	copyright	:	North Edge (2011)
	purpose		:	Distance plugin
**************************************************************************/
#ifndef __DISTANCE_PLUGIN_H__
#define __DISTANCE_PLUGIN_H__

#define GRAPHICS_MODULE			"Graphics"
#define TEXT_LABEL_SERVICE		"TextLabelService"
#define DISTANCE_LABEL_NAME		_T("Distance##Label")

namespace Windower
{
	class DistancePlugin : public Windower::IPlayerDataPlugin
	{
	public:
		explicit DistancePlugin(PluginFramework::IPluginServices *pServices_in);
		~DistancePlugin();

		static PluginFramework::IPlugin* Create(PluginFramework::IPluginServices *pServices_in);
		static void Query(PluginFramework::PluginInfo& PluginInfo_out);
		static void Destroy(PluginFramework::IPlugin *pInstance_in);

		void OnTargetPtrChange(const TargetData **pPlayerData_in, const TargetData **pTargetData_in);

		bool Update();

	private:
		UIAL::CUiWindow<> *m_pTextLabel;
		const TargetData **m_pPlayerData;
		const TargetData **m_pPlayerTarget;
	};
}

#endif//__DISTANCE_PLUGIN_H__