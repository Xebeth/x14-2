/**************************************************************************
	created		:	2011-06-04
	filename	: 	DistancePlugin.h
	author		:	Xebeth`
	copyright	:	North Edge (2011)
	purpose		:	Distance plugin
**************************************************************************/
#ifndef __DISTANCE_PLUGIN_H__
#define __DISTANCE_PLUGIN_H__

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

		void OnPlayerPtrChange(const TargetPos &PlayerData_in);
		void OnTargetPtrChange(const TargetPos &TargetData_in);

		bool Update();

	private:
		UIAL::CUiWindow<> *m_pTextLabel;
		TargetPos m_PlayerPos;
		TargetPos m_TargetPos;
		string_t m_TargetName;
		string_t m_LabelText;
		float m_Distance;
	};
}

#endif//__DISTANCE_PLUGIN_H__