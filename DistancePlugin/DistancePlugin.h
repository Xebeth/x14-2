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

		static PluginFramework::IPlugin* Create(PluginFramework::IPluginServices *pServices_in);
		static void Query(PluginFramework::PluginInfo& PluginInfo_out);
		static void Destroy(PluginFramework::IPlugin *pInstance_in);

		void OnPlayerPtrChange(const TargetData *pPlayerData_in);
		void OnTargetPtrChange(const TargetData *pTargetData_in);

		bool Update();

	private:
		const float *m_pPlayerX, *m_pTargetX;
		const float *m_pPlayerY, *m_pTargetY;
		const float *m_pPlayerZ, *m_pTargetZ;

		float m_Distance;

		char *m_pTargetName;
	};
}

#endif//__DISTANCE_PLUGIN_H__