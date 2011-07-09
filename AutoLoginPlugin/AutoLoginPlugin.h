/**************************************************************************
	created		:	2011-06-04
	filename	: 	AutoLoginPlugin.h
	author		:	Xebeth`
	copyright	:	North Edge (2011)
	purpose		:	Auto login plugin
**************************************************************************/
#ifndef __AUTO_LOGIN_PLUGIN_H__
#define __AUTO_LOGIN_PLUGIN_H__

#define PLUGIN_REGKEY 0x18E5F530

namespace Windower
{
	class WindowerCommand;

	class AutoLoginPlugin : public PluginFramework::IPlugin
	{
	public:
		static void* Create();
		static void Destroy(void *pInstance_in);
		static void Query(PluginInfo& Info_out);

		static int AutoLoginThread(const WindowerCommand *pCommand_in);
	};
}

#endif//__AUTO_LOGIN_PLUGIN_H__