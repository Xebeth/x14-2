/**************************************************************************
	created		:	2011-10-30
	filename	: 	WindowerSettings.h
	author		:	Xebeth`
	copyright	:	North Edge (2011)
	purpose		:	Windower settings
**************************************************************************/
#ifndef __WINDOWER_SETTINGS_H__
#define __WINDOWER_SETTINGS_H__

namespace Windower
{
	//! \brief Windower settings
	class WindowerProfile
	{
	public:
		//! \brief WindowerProfile default constructor
		WindowerProfile() { m_ResX = m_ResY = 0L; m_VSync = m_Direct3D9Ex = FALSE; }
		/*! \brief WindowerProfile copy constructor
			\param[in] Settings_in : the copied settings
		*/
		WindowerProfile(const WindowerProfile &Settings_in)
		{
			m_Name = Settings_in.GetName();
			m_ResX = Settings_in.GetResX();
			m_ResY = Settings_in.GetResY();
			m_VSync = Settings_in.GetVSync();
			m_PluginsDir = Settings_in.GetPluginsDir();
			m_Direct3D9Ex = Settings_in.GetDirect3DEx();
		}
		/*! \brief WindowerProfile constructor
			\param[in] ResX_in : the width of the rendering surface
			\param[in] ResY_in : the height of the rendering surface
			\param[in] VSync_in : flag specifying if vertical synchronization is in use
			\param[in] pName_in : the name of the profile
			\param[in] pPluginsDir_in : relative directory of the plugins
			\param[in] Direct3D9Ex_in : flag specifying if Direct3D9Ex should be used
		*/
		WindowerProfile(LONG ResX_in, LONG ResY_in, BOOL VSync_in, const TCHAR *pName_in, const TCHAR *pPluginsDir_in, BOOL Direct3D9Ex_in = FALSE)
			: m_ResX(ResX_in), m_ResY(ResY_in), m_VSync(VSync_in), m_Direct3D9Ex(Direct3D9Ex_in), m_Name(pName_in), m_PluginsDir(pPluginsDir_in) {}

		/*! \brief Sets the flag specifying if vertical synchronization is in use
			\param[in] VSync_in : the new value of the flag
		*/
		void SetVSync(BOOL VSync_in) { m_VSync = VSync_in; }
		/*! \brief Sets the the name of the profile
			\param[in] pName_in : the new name of the profile
		*/
		void SetName(const TCHAR *pName_in) { if (pName_in != NULL) m_Name = pName_in; }
		/*! \brief Sets the flag specifying if Direct3D9Ex should be used
			\param[in] Direct3D9Ex_in : the new value of the flag
		*/
		void SetDirect3D9Ex(BOOL Direct3D9Ex_in) { m_Direct3D9Ex = Direct3D9Ex_in; }
		/*! \brief Sets the resolution of the rendering surface
			\param[in] ResX_in : the width of the rendering surface
			\param[in] ResY_in : the height of the rendering surface
		*/
		void SetResolution(LONG ResX_in, LONG ResY_in) { m_ResX = ResX_in; m_ResY = ResY_in; }
		/*! \brief Sets the relative directory of the plugins
			\param[in] pPluginsDir_in : the relative directory of the plugins
		*/
		void SetPluginsDir(const TCHAR *pPluginsDir_in) { if (pPluginsDir_in != NULL) m_PluginsDir = pPluginsDir_in; }

		/*! \brief Retrieves he width of the rendering surface
			\return the width of the rendering surface
		*/
		LONG GetResX() const { return m_ResX; }
		/*! \brief Retrieves the height of the rendering surface
			\return the height of the rendering surface
		*/
		LONG GetResY() const { return m_ResY; }
		/*! \brief Retrieves the flag specifying if vertical synchronization is in use
			\return the flag specifying if vertical synchronization is in use
		*/
		BOOL GetVSync() const { return m_VSync; }
		/*! \brief Retrieves flag specifying if Direct3D9Ex should be used
			\return the flag specifying if Direct3D9Ex should be used
		*/
		BOOL GetDirect3DEx() const { return m_Direct3D9Ex; }
		/*! \brief Retrieves the name of the profile
			\return the name of the profile
		*/
		const TCHAR* GetName() const { return m_Name.c_str(); }
		/*! \brief Retrieves the relative directory of the plugins
			\return the relative directory of the plugins
		*/
		const TCHAR* GetPluginsDir() const { return m_PluginsDir.c_str(); }
		/*! \brief Retrieves the absolute directory of the plugins
			\return the absolute directory of the plugins
		*/
		const string_t& GetPluginsAbsoluteDir()
		{
			TCHAR CurrentDir[MAX_PATH];

			GetCurrentDirectory(MAX_PATH, CurrentDir);
			format(m_PluginsAbsDir, _T("%s\\%s\\"), CurrentDir, m_PluginsDir.c_str());

			return m_PluginsAbsDir;
		}

	protected:
		//! the width of the rendering surface
		LONG m_ResX;
		//! the height of the rendering surface
		LONG m_ResY;
		//! flag specifying if vertical synchronization is in use
		BOOL m_VSync;
		//! flag specifying if Direct3D9Ex should be used
		BOOL m_Direct3D9Ex;
		//! the name of the profile
		string_t m_Name;
		//! the absolute directory of the plugins
		string_t m_PluginsAbsDir;
		//! the relative directory of the plugins
		string_t m_PluginsDir;
	};
}

#endif//__WINDOWER_SETTINGS_H__