/**************************************************************************
	created		:	2011-10-30
	filename	: 	WindowerSettings.h
	author		:	Xebeth`
	copyright	:	North Edge (2011)
	purpose		:	Windower settings
**************************************************************************/
#ifndef __WINDOWER_SETTINGS_H__
#define __WINDOWER_SETTINGS_H__

#define PROFILE_PREFIX_LENGTH 8
#define PROFILE_PREFIX _T("Profile:")

#include <set>

namespace Windower
{
	//! a set of active plugin names
	typedef std::set<string_t> ActivePlugins;

	//! \brief Windower settings
	class WindowerProfile
	{
	public:
		//! \brief WindowerProfile default constructor
		WindowerProfile() { m_ResX = m_ResY = 0L; m_VSync = FALSE; }
		/*! \brief WindowerProfile copy constructor
			\param[in] Settings_in : the settings to copy
		*/
		WindowerProfile(const WindowerProfile &Settings_in)
		{
			Copy(Settings_in);
		}

		/*! \brief Copies the specified settings
			\param[in] Settings_in : the settings to copy
		*/
		void Copy(const WindowerProfile &Settings_in)
		{
			m_Name = Settings_in.GetName();
			m_ResX = Settings_in.GetResX();
			m_ResY = Settings_in.GetResY();
			m_VSync = Settings_in.GetVSync();

			m_ActivePlugins = Settings_in.m_ActivePlugins;
		}

		/*! \brief WindowerProfile constructor
			\param[in] ResX_in : the width of the rendering surface
			\param[in] ResY_in : the height of the rendering surface
			\param[in] VSync_in : flag specifying if vertical synchronization is in use
			\param[in] pName_in : the name of the profile
		*/
		WindowerProfile(LONG ResX_in, LONG ResY_in, BOOL VSync_in, const TCHAR *pName_in)
			: m_ResX(ResX_in), m_ResY(ResY_in), m_VSync(VSync_in), m_Name(pName_in) {}

		/*! \brief Sets the flag specifying if vertical synchronization is in use
			\param[in] VSync_in : the new value of the flag
		*/
		void SetVSync(BOOL VSync_in) { m_VSync = VSync_in; }
		/*! \brief Sets the the name of the profile
			\param[in] pName_in : the new name of the profile
		*/
		void SetName(const TCHAR *pName_in) { m_Name = pName_in; }
		/*! \brief Sets the resolution of the rendering surface
			\param[in] ResX_in : the width of the rendering surface
			\param[in] ResY_in : the height of the rendering surface
		*/
		void SetResolution(LONG ResX_in, LONG ResY_in) { m_ResX = ResX_in; m_ResY = ResY_in; }

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
		/*! \brief Retrieves the name of the profile
			\return the name of the profile
		*/
		const TCHAR* GetName() const { return m_Name.c_str(); }

		bool IsPluginActive(const string_t &PluginName_in) const
		{
			ActivePlugins::const_iterator PluginIt = m_ActivePlugins.find(PluginName_in);

			return (PluginIt != m_ActivePlugins.end());
		}

		void ActivatePlugin(const string_t &PluginName_in, bool Activate_in = true)
		{
			if (Activate_in == false)
			{
				ActivePlugins::const_iterator PluginIt = m_ActivePlugins.find(PluginName_in);

				if (PluginIt != m_ActivePlugins.end())
					m_ActivePlugins.erase(PluginIt);
			}
			else
				m_ActivePlugins.insert(PluginName_in);
		}

		const ActivePlugins& GetActivePlugins() const { return m_ActivePlugins; }

	protected:
		//! the width of the rendering surface
		LONG m_ResX;
		//! the height of the rendering surface
		LONG m_ResY;
		//! flag specifying if vertical synchronization is in use
		BOOL m_VSync;
		//! the name of the profile
		string_t m_Name;
		//! set of active plugins
		ActivePlugins m_ActivePlugins;
	};
}

#endif//__WINDOWER_SETTINGS_H__