#ifndef __WINDOWER_SETTINGS_H__
#define __WINDOWER_SETTINGS_H__

class WindowerSettings
{
public:
	WindowerSettings() { m_ResX = m_ResY = m_VSync = FALSE; m_Direct3D9Ex = FALSE; m_pName = NULL; }
	WindowerSettings(const WindowerSettings *pSettings)
	{
		const TCHAR *pName = pSettings->GetName();

		m_ResX = pSettings->GetResX();
		m_ResY = pSettings->GetResY();
		m_VSync = pSettings->GetVSync();
		m_Direct3D9Ex = pSettings->GetDirect3DEx();

		if (pName != NULL)
			m_pName = _tcsdup(pName);
		else
			m_pName = NULL;
	}
	WindowerSettings(LONG ResX, LONG ResY, BOOL VSync, const TCHAR *pName, BOOL Direct3D9Ex = FALSE) : 
					 m_ResX(ResX), m_ResY(ResY), m_VSync(VSync), m_Direct3D9Ex(Direct3D9Ex)
	{
		if (pName != NULL)
			m_pName = _tcsdup(pName);
	}
	~WindowerSettings()
	{
		if (m_pName != NULL)
		{
			delete m_pName;
			m_pName = NULL;
		}
	}

	void SetVSync(BOOL vSync) { m_VSync = vSync; }
	void SetDirect3D9Ex(BOOL Direct3D9Ex) { m_Direct3D9Ex = Direct3D9Ex; }
	void SetName(const TCHAR *pName)
	{
		if (pName != NULL)
		{
			if (m_pName != NULL)
				delete m_pName;

			m_pName = _tcsdup(pName);
		}
	}
	void SetResolution(LONG ResX, LONG ResY) { m_ResX = ResX; m_ResY = ResY; }

	LONG GetResX() const { return m_ResX; }
	LONG GetResY() const { return m_ResY; }
	BOOL GetVSync() const { return m_VSync; }
	BOOL GetDirect3DEx() const { return m_Direct3D9Ex; }
	const TCHAR* GetName() const { return m_pName; }

protected:
	LONG m_ResX;
	LONG m_ResY;
	BOOL m_VSync;
	BOOL m_Direct3D9Ex;
	TCHAR *m_pName;
};

#endif//__WINDOWER_SETTINGS_H__