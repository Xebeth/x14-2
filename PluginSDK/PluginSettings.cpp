/**************************************************************************
	created		:	2013-05-04
	filename	: 	PluginSettings.cpp
	author		:	Xebeth`
	copyright	:	North Edge (2013)
	purpose		:	
**************************************************************************/
#include "stdafx.h"

#include "PluginSettings.h"

namespace Windower
{
	/*! \brief SettingsIniFile constructor
		\param[in] SourceFile_in : the path of the INI file
	*/
	PluginSettings::PluginSettings(const string_t &SourceFile_in, const TCHAR *pProfileName_in)
		: Settings::SettingsIniFile(SourceFile_in)
	{
		if (pProfileName_in != NULL)
			m_CurrentSection.assign(pProfileName_in);
		// load the settings file
		Load();
	}

	/*! \brief Creates a new section in the INI file
		\param[in] SectionName_in : the name of the new section
		\param[in] SetCurrent_in : flag specifying if the new section should be made the current one
		\return true if successful; false otherwise
	*/
	bool PluginSettings::CreateSection(const string_t &SectionName_in, bool SetCurrent_in)
	{
		if (__super::CreateSection(SectionName_in))
		{
			if (SetCurrent_in)
				m_CurrentSection = SectionName_in;

			return true;
		}

		return false;
	}

	/*! \brief Retrieves the long value corresponding to the specified key in the current section
		\param[in] Key_in : the name of the key
		\param[in] DefaultValue : the default value if the key wasn't found
		\return the long value of the key if valid; DefaultValue otherwise
	*/
	LONG PluginSettings::GetLong(const string_t &Key_in, LONG DefaultValue) const
	{
		return __super::GetLong(m_CurrentSection.c_str(), Key_in.c_str(), DefaultValue);
	}

	/*! \brief Retrieves the DWORD value corresponding to the specified key in the current section
		\param[in] Key_in : the name of the key
		\param[in] DefaultValue : the default value if the key wasn't found
		\return the DWORD value of the key if valid; DefaultValue otherwise
	*/
	ULONG PluginSettings::GetUnsignedLong(const string_t &Key_in, ULONG DefaultValue) const
	{
		return __super::GetUnsignedLong(m_CurrentSection.c_str(), Key_in.c_str(), DefaultValue);
	}

	/*! \brief Sets the specified key value as a long in the current section
		\param[in] Key_in : the name of the key
		\param[in] NewValue_in : the new value for the key
	*/
	void PluginSettings::SetLong(const string_t &Key_in, LONG NewValue_in, const TCHAR *pComment_in)
	{
		__super::SetLong(m_CurrentSection.c_str(), Key_in.c_str(), NewValue_in, pComment_in);
	}

	/*! \brief Sets the specified key value as an hexadecimal representation of a long in the current section
		\param[in] Key_in : the name of the key
		\param[in] NewValue_in : the new value for the key
	*/
	void PluginSettings::SetHex(const string_t &Key_in, LONG NewValue_in, const TCHAR *pComment_in)
	{
		__super::SetHex(m_CurrentSection.c_str(), Key_in.c_str(), NewValue_in, pComment_in);
	}

	/*! \brief Retrieves the string value corresponding to the specified key in the current section
		\param[in] Key_in : the name of the key
		\param[in] pDefaultValue : the default value if the key wasn't found
		\return the string value of the key if valid; pDefaultValue otherwise
	*/
	const TCHAR* PluginSettings::GetString(const string_t &Key_in, const TCHAR* pDefaultValue) const
	{
		return __super::GetString(m_CurrentSection.c_str(), Key_in.c_str(), pDefaultValue);
	}

	/*! \brief Sets the specified key value as string in the current section
		\param[in] Key_in : the name of the key
		\param[in] NewValue_in : the new value for the key
	*/
	void PluginSettings::SetString(const string_t &Key_in, const string_t &NewValue_in, const TCHAR *pComment_in) 
	{
		__super::SetString(m_CurrentSection.c_str(), Key_in.c_str(), NewValue_in.c_str(), pComment_in);
	}


	/*! \brief Checks if the specified key exists within the current section
		\param[in] KeyName_in : key to check
		\return true if the key exists in the specified section; false otherwise
	*/
	bool PluginSettings::KeyExists(const string_t &KeyName_in) const
	{
		return __super::KeyExists(m_CurrentSection.c_str(), KeyName_in.c_str());
	}

	/*! \brief Deletes the specified key exists within the current section
		\param[in] KeyName_in : key to delete
		\return true if the key was deleted in the specified section; false otherwise
	*/
	bool PluginSettings::DeleteKey(const string_t &KeyName_in) const
	{
		return __super::DeleteKey(m_CurrentSection.c_str(), KeyName_in.c_str());
	}

	/*! \brief Sets the name of the current section
		\param[in] CurrentSection_in : the name of the new current section
	*/
	void PluginSettings::SetCurrentSection(const string_t &CurrentSection_in)
	{ m_CurrentSection = CurrentSection_in; }

	/*! \brief Retrieves the name of the current section
		\return the name of the current section
	*/
	const string_t& PluginSettings::GetCurrentSection() const
	{ return m_CurrentSection; }

	/*! \brief Retrieves the name of the profile
		\return the name of the profile if set; 'Default' otherwise
	*/
	const TCHAR* PluginSettings::GetProfileName() const
	{
		if (m_CurrentSection.empty() || m_CurrentSection.find(PROFILE_PREFIX) == string_t::npos)
			return _T("Default");
		else
			return (m_CurrentSection.c_str() + PROFILE_PREFIX_LENGTH);
	}
}