#ifndef __WINDOWER_SETTINGS_H__
#define __WINDOWER_SETTINGS_H__

namespace Windower
{
	class WindowerProfile
	{
	public:
		WindowerProfile() { m_ResX = m_ResY = 0L; m_VSync = m_Direct3D9Ex = FALSE; }
		WindowerProfile(const WindowerProfile *pSettings)
		{
			m_Name = pSettings->GetName();
			m_ResX = pSettings->GetResX();
			m_ResY = pSettings->GetResY();
			m_VSync = pSettings->GetVSync();
			m_PluginsDir = pSettings->GetPluginsDir();
			m_Direct3D9Ex = pSettings->GetDirect3DEx();
		}
		WindowerProfile(LONG ResX, LONG ResY, BOOL VSync, const TCHAR *pName, const TCHAR *pPluginsDir, BOOL Direct3D9Ex = FALSE)
			: m_ResX(ResX), m_ResY(ResY), m_VSync(VSync), m_Direct3D9Ex(Direct3D9Ex), m_Name(pName), m_PluginsDir(pPluginsDir) {}

		void SetVSync(BOOL vSync) { m_VSync = vSync; }
		void SetName(const TCHAR *pName) { if (pName != NULL) m_Name = pName; }
		void SetDirect3D9Ex(BOOL Direct3D9Ex) { m_Direct3D9Ex = Direct3D9Ex; }
		void SetResolution(LONG ResX, LONG ResY) { m_ResX = ResX; m_ResY = ResY; }
		void SetPluginsDir(const TCHAR *pPluginsDir) { if (pPluginsDir != NULL) m_PluginsDir = pPluginsDir; }

		LONG GetResX() const { return m_ResX; }
		LONG GetResY() const { return m_ResY; }
		BOOL GetVSync() const { return m_VSync; }
		BOOL GetDirect3DEx() const { return m_Direct3D9Ex; }
		const TCHAR* GetName() const { return m_Name.c_str(); }
		const TCHAR* GetPluginsDir() const { return m_PluginsDir.c_str(); }
		const string_t& GetPluginsAbsoluteDir()
		{
			TCHAR CurrentDir[MAX_PATH];

			GetCurrentDirectory(MAX_PATH, CurrentDir);
			format(m_PluginsAbsDir, _T("%s\\%s\\"), CurrentDir, m_PluginsDir.c_str());

			return m_PluginsAbsDir;
		}

	protected:
		LONG		m_ResX;
		LONG		m_ResY;
		BOOL		m_VSync;
		BOOL		m_Direct3D9Ex;
		string_t	m_Name;
		string_t	m_PluginsAbsDir;
		string_t	m_PluginsDir;
	};
}

#endif//__WINDOWER_SETTINGS_H__