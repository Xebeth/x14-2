/**************************************************************************
	created		:	2011-10-30
	filename	: 	WindowerSettings.h
	author		:	Xebeth`
	copyright	:	North Edge (2011)
	purpose		:	Windower settings
**************************************************************************/
#ifndef __WINDOWER_SETTINGS_H__
#define __WINDOWER_SETTINGS_H__

#ifndef _WINDOWER_PLUGIN_SDK_INCLUDE_
	#error Please include the global header file 'PluginSDK.h'
#endif

enum eIniKeys
{
	// general settings
	INI_KEY_PLUGINS,
	INI_KEY_VSYNC,	
	INI_KEY_LNG,
	// auto-login plugin
	INI_KEY_AUTO_SUBMIT,
	INI_KEY_PASSWORD,
	INI_KEY_USERNAME,	
	INI_KEY_HASH,
	// timestamp plugin
	INI_KEY_TIMESTAMP,
	// tell detect plugin
	INI_KEY_TELL_SOUND,
	// auto blacklist plugin
	INI_KEY_BLACKLIST_THRESHOLD,
	INI_KEY_BLACKLIST_COUNT,
	// number of keys
	INI_KEY_COUNT
};
								// X | Y | TextColor | FontName | FontSize | Bold | Italic | Collapsed
#define SERIALIZE_LABEL_FORMAT	_T("%ld|%ld|%08X|%s|%hu|%hu|%hu|%hu")
#define PROFILE_PREFIX			_T("Profile:")
#define LABELS_PREFIX			_T("Labels:")

namespace Windower
{
	class LabelSettings
	{
	public:
		LabelSettings() : TextColor(0xFF000000UL), X(0L), Y(0L),
			FontSize(12U), FontName(_T("Arial")), bBold(true), bItalic(false), bCollapsed(false) {}

		LabelSettings(long X_in, long Y_in, unsigned long TextColor_in = 0xFF000000UL,
					  const string_t &FontName_in = _T("Arial"), unsigned short FontSize_in = 12,
					  bool Bold_in = true, bool Italic_in = false, bool Collapsed_in = false)
			: X(X_in), Y(Y_in), TextColor(TextColor_in), FontSize(FontSize_in),
			  FontName(FontName_in), bBold(Bold_in), bItalic(Italic_in), bCollapsed(Collapsed_in) {}

		unsigned long TextColor;
		unsigned short FontSize;
		bool bBold, bItalic;
		string_t FontName;
		bool bCollapsed;
		long X, Y;
	};

	typedef hash_map<string_t, LabelSettings> TextLabels;
	typedef hash_map<std::string, long> ScoredWords;
	//! a set of active plugin names
	typedef std::set<string_t> ActivePlugins;

	//! \brief Windower settings
	class WindowerProfile : public PluginFramework::IUserSettings
	{
	public:
		//! \brief WindowerProfile default constructor
		WindowerProfile();

		/*! \brief WindowerProfile copy constructor
			\param[in] Settings_in : the settings to copy
		*/
		WindowerProfile(const WindowerProfile &Settings_in)
		{ Copy(Settings_in); }

		void Copy(const WindowerProfile &Settings_in);

		static const TCHAR* Key(eIniKeys Key_in)
		{ return m_sKeyName[Key_in]; }

		static const TCHAR* Comment(eIniKeys Key_in)
		{ return m_sKeyComment[Key_in]; }

		template<typename T> static T Default(eIniKeys Key_in);

		template<> static ULONG Default(eIniKeys Key_in)
		{
			TCHAR InvalidChars[64] = { '\0' };
			TCHAR *Chars = &InvalidChars[0];

			return _tcstoul(m_sKeyDefault[Key_in], &Chars, 10);
		}

		template<> static LONG Default(eIniKeys Key_in)
		{
			TCHAR InvalidChars[64] = { '\0' };
			TCHAR *Chars = &InvalidChars[0];

			return _tcstol(m_sKeyDefault[Key_in], &Chars, 10); 
		}

		template<> static const TCHAR* Default(eIniKeys Key_in)
		{
			return m_sKeyDefault[Key_in];
		}

		/*! \brief WindowerProfile constructor
			\param[in] VSync_in : flag specifying if vertical synchronization is in use
			\param[in] pName_in : the name of the profile
		*/
		WindowerProfile(const TCHAR *pName_in, long Lng_in, bool VSync_in)
			: m_VSync(VSync_in), m_Name(pName_in), m_Language(Lng_in) {}

		/*! \brief Sets the flag specifying if vertical synchronization is in use
			\param[in] VSync_in : the new value of the flag
		*/
		void SetVSync(bool VSync_in) { m_VSync = VSync_in; }

		/*! \brief Sets the language ID
			\param[in] Lng_in : the new language ID
		*/
		void SetLanguage(long Lng_in) { m_Language = Lng_in; }

		/*! \brief Sets the blacklist threshold
			\param[in] Lng_in : the new blacklist threshold
		*/
		void SetBlacklistThreshold(long Threshold_in) { m_BlacklistThreshold = Threshold_in; }

		void SetBlacklistCount(long Count_in) { m_BlackListCount = Count_in; }

		/*! \brief Sets the name of the profile
			\param[in] pName_in : the new name of the profile
		*/
		void SetName(const TCHAR *pName_in) { m_Name = pName_in; }

		/*! \brief Retrieves the flag specifying if vertical synchronization is in use
			\return the flag specifying if vertical synchronization is in use
		*/
		bool GetVSync() const { return m_VSync; }

		/*! \brief Retrieves the language ID
			\return the language ID
		*/
		long GetLanguage() const { return m_Language; }

		/*! \brief Retrieves the blacklist threshold
			\return the blacklist threshold
		*/
		long GetBlacklistThreshold() const { return m_BlacklistThreshold; }

		long GetBlacklistCount() const { return m_BlackListCount; }

		/*! \brief Retrieves the name of the profile
			\return the name of the profile
		*/
		const TCHAR* GetName() const { return m_Name.c_str(); }

		bool IsPluginActive(const string_t& PluginName_in) const
		{
			ActivePlugins::const_iterator PluginIt = m_ActivePlugins.find(PluginName_in);

			return (PluginIt != m_ActivePlugins.cend());
		}

		void UpdatePluginList(const string_t& PluginName_in, bool Activate_in = true);

		const ActivePlugins& GetActivePlugins() const { return m_ActivePlugins; }

		const string_t& GetProfileName() const { return m_Name; }

		string_t& SaveLabel(const LabelSettings &Settings_in, string_t &SerializedData_out) const;
		bool LoadLabel(const string_t &Name_in, const string_t &SerializedData_in);
		
		bool DeserializeLabel(const string_t &Name_in, long &X_out, long &Y_out,
							  unsigned long &TextColor_out, string_t &FontName_out,
							  unsigned short &FontSize_out, bool &Bold_out,
							  bool &Italic_out, bool &Collapsed_out);
		void SerializeLabel(const string_t &Name_in, long X_in, long Y_in,
							unsigned long TextColor_in = 0xFF000000UL,
							const string_t &FontName_in = _T("Arial"),
							unsigned short FontSize_in = 12,
							bool Bold_in = true, bool Italic_in = false,
							bool Collapsed_in = false);
	
		void SetCryptedPassword(const string_t &Password_in) { m_Password = Password_in; }
		void SetTellSound(const string_t &TellSound_in) { m_TellSound = TellSound_in; }
		void SetTimestampFormat(const string_t &Format_in) { m_Timestamp = Format_in; }		
		void SetUsername(const string_t &Username_in) { m_Username = Username_in; }		
		void SetAutoSubmit(bool AutoSubmit_in) { m_AutoSubmit = AutoSubmit_in;}
		void SetKeyHash(ULONG Hash_in) { m_KeyHash = Hash_in; }

		const TCHAR* GetPluginList() const;
		
		ULONG GetKeyHash() const
		{ return m_KeyHash; }

		bool IsAutoSubmitted() const
		{ return m_AutoSubmit; }

		const TCHAR* GetUsername() const
		{ return m_Username.c_str(); }

		const TCHAR* GetCryptedPassword() const
		{ return m_Password.c_str(); }

		const TCHAR* GetTimestampFormat() const
		{ return m_Timestamp.c_str(); }

		const TCHAR* GetTellSound() const
		{ return m_TellSound.c_str(); }

		// implementation of IUserSettings
		virtual const TCHAR* GetString(const string_t &Key_in, const TCHAR* pDefaultValue = _T("")) const;
		virtual ULONG GetUnsignedLong(const string_t &Key_in, ULONG DefaultValue = 0UL) const;
		virtual LONG GetLong(const string_t &Key_in, LONG DefaultValue = 0L) const;

		virtual void SetString(const string_t &Key_in, const string_t &NewValue_in, const TCHAR *pComment_in = nullptr);
		virtual void SetUnsignedLong(const string_t &Key_in, ULONG NewValue_in, const TCHAR *pComment_in = nullptr);
		virtual void SetLong(const string_t &Key_in, LONG NewValue_in, const TCHAR *pComment_in = nullptr);
		void SetPluginList( const string_t &PluginList_in );

		void AddScoredWord(const TCHAR *pWord_in, long Score_in);

		void SetScoredWords(ScoredWords *pScoredWords_in)
		{ m_pScoredWords = pScoredWords_in; }
		ScoredWords* GetScoredWords() const
		{ return m_pScoredWords; }

		TextLabels& GetTextLabels()
		{ return m_TextLabels; }

	protected:
		void MoveNext(std::list<string_t> &List_in_out) const;

		static const TCHAR* m_sKeyDefault[INI_KEY_COUNT];
		static const TCHAR* m_sKeyComment[INI_KEY_COUNT];
		static const TCHAR* m_sKeyName[INI_KEY_COUNT];

		typedef hash_map<string_t, eIniKeys> KeyMapping;

		//! flag specifying if vertical synchronization is in use
		bool m_VSync;
		//! flag specifying if the login form is submitted automatically
		bool m_AutoSubmit;
		//! language ID
		long m_Language;
		//! the name of the profile
		string_t m_Name;
		//! hash of the encryption key
		ULONG m_KeyHash;
		//! the username used to log in
		string_t m_Username;
		//! the encrypted password used to log in
		string_t m_Password;
		//! the format of the timestamp
		string_t m_Timestamp;
		//! the path to the sound file played when a tell is detected
		string_t m_TellSound;
		//! set of active plugins
		ActivePlugins m_ActivePlugins;
		//! key mapping
		KeyMapping m_KeyMapping;
		//! string containing the list of plugins
		mutable string_t m_PluginList;
		//! serialized text labels
		TextLabels m_TextLabels;
		//! scored words for the AutoBlacklist plugin
		ScoredWords *m_pScoredWords;
		//! threshold for blacklisting
		long m_BlacklistThreshold;
		//! count before adding to blacklist
		long m_BlackListCount;
	};
}

#endif//__WINDOWER_SETTINGS_H__