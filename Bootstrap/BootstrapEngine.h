/**************************************************************************
	created		:	2011-07-03
	filename	: 	BootstrapEngine.h
	author		:	Xebeth`
	copyright	:	North Edge (2011)
	purpose		:	Plugin engine used during the game starting process
					Handles the AutoLogin plugin during the login process
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

	//! a hash map of core modules
	typedef hash_map<string_t, Windower::ICoreModule*> CoreModules;

	//! \brief Plugin engine used during the game starting process
	class BootstrapEngine : public Windower::PluginEngine
	{
	public:
		BootstrapEngine(const string_t &WorkingDir_in, const TCHAR *pConfigFile_in);
		virtual ~BootstrapEngine();

		bool IsAutoLoginActive() const;
		bool InvokeAutoLogin();

		/*! \brief Retrieves a reference to the system core module
			\return a reference to the system core module
		*/
		SystemCore& System() const { return *m_pSystemCore; }

		bool UpdateCmdLineFromSettings(string_t &CmdLine_in_out) const;

	protected:
		bool Attach() override;
		bool Detach() override;

		//! the settings manager of the engine
		Windower::SettingsManager *m_pSettingsManager;
		//! the current settings profile
		Windower::WindowerProfile *m_pSettings;
		//! the hook engine
		HookEngine m_HookManager;
		//! the system core module
		SystemCore *m_pSystemCore;
		//! the module handle of the hooked process
		HMODULE m_hTarget;
	};
}

#endif//__BOOTSTRAP_ENGINE_H__