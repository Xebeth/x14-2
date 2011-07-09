/**************************************************************************
	created		:	2011-07-03
	filename	: 	BootstrapEngine.h
	author		:	Xebeth`
	copyright	:	North Edge (2011)
	purpose		:	
**************************************************************************/
#ifndef __BOOTSTRAP_ENGINE_H__
#define __BOOTSTRAP_ENGINE_H__

namespace Windower
{
	class CommandDispatcher;
	class SettingsManager;
	class WindowerProfile;
	class ICoreModule;
}

namespace Bootstrap
{
	class SystemCore;

	typedef stdext::hash_map<string_t, Windower::ICoreModule*> CoreModules;

	class BootstrapEngine : public Windower::PluginEngine
	{
	public:
		explicit BootstrapEngine(const TCHAR *pConfigFile_in);
		virtual ~BootstrapEngine();

		virtual bool Attach();
		virtual bool Detach();

		void InvokeAutoLogin(HWND hParentWnd_in);

		SystemCore& System() const { return *m_pSystemCore; }
		const Windower::CommandDispatcher& Dispatcher() const { return *m_pCommandDispatcher; }

	protected:
		Windower::CommandDispatcher	*m_pCommandDispatcher;
		Windower::SettingsManager	*m_pSettingsManager;
		Windower::WindowerProfile	*m_pSettings;
		HookEngine			*m_pHookManager;
		SystemCore			*m_pSystemCore;
		HMODULE				 m_hTarget;
	};
}

#endif//__BOOTSTRAP_ENGINE_H__