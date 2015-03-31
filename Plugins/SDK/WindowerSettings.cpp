/**************************************************************************
	created		:	2011-10-30
	filename	: 	WindowerSettings.cpp
	author		:	Xebeth`
	copyright	:	North Edge (2011)
	purpose		:	Windower settings
**************************************************************************/
#include "stdafx.h"

#include "WindowerSettings.h"

namespace Windower
{
	const TCHAR* WindowerProfile::m_sKeyDefault[INI_KEY_COUNT] =
	{
		_T(""),																// Plugins
		_T("0"),															// VSync
		_T("1"),															// Language
		_T("0"),															// AutoSubmit
		_T(""),																// Password
		_T(""),																// Username
		_T("0"),															// KeyHash
		_T("[HH:mm:ss] "),													// TimestampFormat
		_T("tell.wav"),														// TellSound
		_T("15"),															// BlacklistThreshold
		_T("3"),															// BlacklistCount
	};

	const TCHAR* WindowerProfile::m_sKeyComment[INI_KEY_COUNT] =
	{
		NULL,																// Plugins
		NULL,																// VSync
		_T(";	0: 日本語	1: English	2: Deutsch	3: Français"),			// Language
		NULL,																// AutoSubmit
		NULL,																// Password
		NULL,																// Username
		NULL,																// KeyHash
		NULL,																// TimestampFormat
		NULL,																// TellSound
		NULL,																// BlacklistThreshold
		NULL,																// BlacklistCount
	};

	const TCHAR* WindowerProfile::m_sKeyName[INI_KEY_COUNT] =
	{
		_T("Plugins"),														// Plugins
		_T("VSync"),														// VSync
		_T("Language"),														// Language
		_T("AutoSubmit"),													// AutoSubmit
		_T("Password"),														// Password
		_T("Username"),														// Username
		_T("KeyHash"),														// KeyHash
		_T("TimestampFormat"),												// TimestampFormat
		_T("TellSound"),													// TellSound
		_T("BlacklistThreshold"),											// m_BlacklistThreshold
		_T("BlacklistCount"),												// BlacklistCount
	};

	//! \brief WindowerProfile default constructor
	WindowerProfile::WindowerProfile() : 
		m_VSync(Default<LONG>(INI_KEY_VSYNC) == 1L),
		m_KeyHash(Default<ULONG>(INI_KEY_HASH)),
		m_Language(Default<LONG>(INI_KEY_LNG)),
		m_BlacklistThreshold(5L),
		m_BlackListCount(3L),
		m_pScoredWords(NULL)
	{
		for (int i = 0; i < INI_KEY_COUNT; ++i)
			m_KeyMapping[m_sKeyName[i]] = (eIniKeys)i;
	}

	/*! \brief Copies the specified settings
		\param[in] Settings_in : the settings to copy
	*/
	void WindowerProfile::Copy(const WindowerProfile &Settings_in)
	{
		// general settings
		m_Name = Settings_in.m_Name;
		m_VSync = Settings_in.m_VSync;
		m_Language = Settings_in.m_Language;

		// auto-login
		m_KeyHash = Settings_in.m_KeyHash;
		m_Password = Settings_in.m_Password;
		m_Username = Settings_in.m_Username;
		m_AutoSubmit = Settings_in.m_AutoSubmit;

		// timestamp / chat log
		m_Timestamp = Settings_in.m_Timestamp;

		// tell detect
		m_TellSound = Settings_in.m_TellSound;

		// auto blacklist
		m_BlacklistThreshold = Settings_in.m_BlacklistThreshold;
		m_BlackListCount = Settings_in.m_BlackListCount;

		m_ActivePlugins = Settings_in.m_ActivePlugins;
		m_pScoredWords = Settings_in.m_pScoredWords;
		m_TextLabels = Settings_in.m_TextLabels;
	}

	/*! \brief Adds or removes a plugin to the active plugins list given its name
		\param[in] PluginName_in : the name of the plugin
		\param[in] Activate_in : flag specifying if the plugin is added or removed
	*/
	void WindowerProfile::UpdatePluginList(const string_t& PluginName_in, bool Activate_in)
	{
		if (Activate_in == false)
		{
			ActivePlugins::const_iterator PluginIt = m_ActivePlugins.find(PluginName_in);

			if (PluginIt != m_ActivePlugins.cend())
				m_ActivePlugins.erase(PluginIt);
		}
		else
			m_ActivePlugins.insert(PluginName_in);
	}

	LONG WindowerProfile::GetLong(const string_t &Key_in, LONG DefaultValue) const
	{
		KeyMapping::const_iterator KeyIt = m_KeyMapping.find(Key_in);

		if (KeyIt != m_KeyMapping.cend())
		{
			eIniKeys Key = KeyIt->second;

			switch(Key)
			{
				case INI_KEY_VSYNC:
					return GetVSync() ? 1L : 0L;
				case INI_KEY_LNG:
					return GetLanguage();
				case INI_KEY_AUTO_SUBMIT:
					return IsAutoSubmitted() ? 1L : 0L;
			}
		}

		return DefaultValue;
	}

	ULONG WindowerProfile::GetUnsignedLong(const string_t &Key_in, ULONG DefaultValue) const
	{
		KeyMapping::const_iterator KeyIt = m_KeyMapping.find(Key_in);

		if (KeyIt != m_KeyMapping.cend())
		{
			eIniKeys Key = KeyIt->second;

			switch(Key)
			{
				case INI_KEY_HASH:
					return GetKeyHash();
			}
		}

		return DefaultValue;
	}

	const TCHAR* WindowerProfile::GetString(const string_t &Key_in, const TCHAR* pDefaultValue) const
	{
		KeyMapping::const_iterator KeyIt = m_KeyMapping.find(Key_in);

		if (KeyIt != m_KeyMapping.cend())
		{
			eIniKeys Key = KeyIt->second;

			switch(Key)
			{
				case INI_KEY_TELL_SOUND:
					return GetTellSound();
				case INI_KEY_TIMESTAMP:
					return GetTimestampFormat();
				case INI_KEY_USERNAME:
					return GetUsername();
				case INI_KEY_PASSWORD:
					return GetCryptedPassword();
				case INI_KEY_PLUGINS:
					return GetPluginList();
			}
		}

		return pDefaultValue;
	}

	void WindowerProfile::SetUnsignedLong(const string_t &Key_in, ULONG NewValue_in, const TCHAR *pComment_in)
	{
		KeyMapping::const_iterator KeyIt = m_KeyMapping.find(Key_in);

		if (KeyIt != m_KeyMapping.cend())
		{
			eIniKeys Key = KeyIt->second;

			switch(Key)
			{
				case INI_KEY_HASH:
					SetKeyHash(NewValue_in);
				break;
			}
		}
	}

	void WindowerProfile::SetLong(const string_t &Key_in, LONG NewValue_in, const TCHAR *pComment_in)
	{
		KeyMapping::const_iterator KeyIt = m_KeyMapping.find(Key_in);

		if (KeyIt != m_KeyMapping.cend())
		{
			eIniKeys Key = KeyIt->second;

			switch(Key)
			{
				case INI_KEY_VSYNC:
					SetVSync(NewValue_in == 1L);
				break;
				case INI_KEY_LNG:
					SetLanguage(NewValue_in);
				break;
				case INI_KEY_AUTO_SUBMIT:
					SetAutoSubmit(NewValue_in == 1L);
				break;
			}
		}
	}

	void WindowerProfile::SetString(const string_t &Key_in, const string_t &NewValue_in, const TCHAR *pComment_in)
	{
		KeyMapping::const_iterator KeyIt = m_KeyMapping.find(Key_in);

		if (KeyIt != m_KeyMapping.cend())
		{
			eIniKeys Key = KeyIt->second;

			switch(Key)
			{
				case INI_KEY_TELL_SOUND:
					SetTellSound(NewValue_in);
				break;
				case INI_KEY_TIMESTAMP:
					SetTimestampFormat(NewValue_in);
				break;
				case INI_KEY_USERNAME:
					SetUsername(NewValue_in);
				break;
				case INI_KEY_PASSWORD:
					SetCryptedPassword(NewValue_in);
				break;
				case INI_KEY_PLUGINS:
					SetPluginList(NewValue_in);
				break;
			}
		}
	}

	void WindowerProfile::AddScoredWord(const TCHAR *pWord_in, long Score_in)
	{
		if (pWord_in != NULL && m_pScoredWords != NULL)
		{
			string_t wWord = pWord_in;
			std::string aWord;

			convert_ansi(wWord, aWord);
			std::transform(aWord.begin(), aWord.end(), aWord.begin(), ::tolower);

			(*m_pScoredWords)[aWord] = Score_in;
		}
	}

	const TCHAR* WindowerProfile::GetPluginList() const
	{
		ActivePlugins::const_iterator PluginBeginIt = m_ActivePlugins.cbegin();
		ActivePlugins::const_iterator PluginEndIt = m_ActivePlugins.cend();					
		ActivePlugins::const_iterator PluginIt = m_ActivePlugins.cbegin();

		m_PluginList.clear();

		for (; PluginIt != PluginEndIt; ++PluginIt)
		{
			if (PluginIt != PluginBeginIt)
				m_PluginList += '|';

			m_PluginList += *PluginIt;
		}

		return m_PluginList.c_str();
	}

	void WindowerProfile::SetPluginList(const string_t &PluginList_in)
	{
		std::list<string_t>::const_iterator PluginIt, EndIt;
		std::list<string_t> PluginList;

		tokenize<wchar_t>(PluginList_in, PluginList, _T("|"), _T("\0"));

		for (PluginIt = PluginList.cbegin(), EndIt = PluginList.cend(); PluginIt != EndIt; ++PluginIt)
			UpdatePluginList(*PluginIt);
	}

	void WindowerProfile::SerializeLabel(const string_t &Name_in, long X_in, long Y_in, unsigned long TextColor_in,
										 const string_t &FontName_in, unsigned short FontSize_in, bool Bold_in, bool Italic_in)
	{
		m_TextLabels[Name_in] = LabelSettings(X_in, Y_in, TextColor_in, FontName_in, FontSize_in, Bold_in, Italic_in);
	}

	bool WindowerProfile::DeserializeLabel(const string_t &Name_in, long &X_out, long &Y_out, unsigned long &TextColor_out,
										   string_t &FontName_out, unsigned short &FontSize_out, bool &Bold_out, bool &Italic_out)
	{
		TextLabels::const_iterator LabelIt = m_TextLabels.find(Name_in);

		if (LabelIt != m_TextLabels.cend())
		{
			TextColor_out = LabelIt->second.TextColor;
			FontName_out = LabelIt->second.FontName;
			FontSize_out = LabelIt->second.FontSize;
			Italic_out = LabelIt->second.bItalic;
			Bold_out = LabelIt->second.bBold;
			X_out = LabelIt->second.X;
			Y_out = LabelIt->second.Y;

			return true;
		}

		return false;
	}

	bool WindowerProfile::LoadLabel(const string_t &Name_in, const string_t &SerializedData_in)
	{
		std::list<string_t> LabelParams;

		if (tokenize<TCHAR>(SerializedData_in, LabelParams, _T("|"), _T("")))
		{
			LabelSettings &Settings = m_TextLabels[Name_in] = LabelSettings();

			// X
			Settings.X = _tcstol(LabelParams.front().c_str(), NULL, 10);
			LabelParams.pop_front();

			if (LabelParams.empty())
				return false;
			// Y
			Settings.Y = _tcstol(LabelParams.front().c_str(), NULL, 10);
			LabelParams.pop_front();

			if (LabelParams.empty())
				return false;
			// TextColor
			Settings.TextColor = _tcstoul(LabelParams.front().c_str(), NULL, 16);
			LabelParams.pop_front();

			if (LabelParams.empty())
				return false;
			// FontName
			Settings.FontName = LabelParams.front();
			LabelParams.pop_front();

			if (LabelParams.empty())
				return false;
			// FontSize
			Settings.FontSize = (unsigned short)_tcstol(LabelParams.front().c_str(), NULL, 10);
			LabelParams.pop_front();

			if (LabelParams.empty())
				return false;
			// Bold
			Settings.bBold = (unsigned short)_tcstol(LabelParams.front().c_str(), NULL, 10) ? true : false;
			LabelParams.pop_front();

			if (LabelParams.empty())
				return false;
			// Italic
			Settings.bItalic = (unsigned short)_tcstol(LabelParams.front().c_str(), NULL, 10) ? true : false;
			LabelParams.pop_front();

			return LabelParams.empty();
		}

		return false;
	}

	string_t& WindowerProfile::SaveLabel(const LabelSettings &Settings_in, string_t &SerializedData_out) const
	{
		return format(SerializedData_out, SERIALIZE_LABEL_FORMAT, Settings_in.X, Settings_in.Y,
																  Settings_in.TextColor,
																  Settings_in.FontName.c_str(),
																  Settings_in.FontSize,
																  Settings_in.bBold ? 1U : 0U,
																  Settings_in.bItalic ? 1U : 0U);
	}
}