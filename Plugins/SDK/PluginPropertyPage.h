/**************************************************************************
	created		:	2013-05-05
	filename	: 	PluginPropertyPage.h
	author		:	Xebeth`
	copyright	:	North Edge (2013)
	purpose		:	
**************************************************************************/
#ifndef __PLUGIN_PROPERTY_PAGE_H__
#define __PLUGIN_PROPERTY_PAGE_H__

#include <afxdlgs.h>

namespace Windower
{
	class SettingsManager;
	class WindowerProfile;

	class PluginPropertyPage : public CPropertyPage
	{
	public:
		PluginPropertyPage(WindowerProfile *pSettings_in,
						   const string_t &PluginName_in,
						   UINT TemplateResID_in,
						   UINT IconResID_in);
		virtual ~PluginPropertyPage();

		const string_t& GetFeedback() const;

		DWORD RemoveFlag(DWORD Flag_in);
		void SetFlags(DWORD Flags_in);
		DWORD AddFlag(DWORD Flag_in);

		bool IsFlagSet(DWORD Flag_in) const;
		UINT GetTemplateID() const;		
		DWORD GetFlags() const;

		/*! \brief Checks if the page configuration is valid
			\param[out] pFeedback_out : any error message explain why the page is invalid
			\return true if the page configuration is valid; false otherwise
		*/
		virtual bool IsPageValid(string_t *pFeedback_out) const =0;
		/*! \brief Commits the changes made to the settings
			\return true if the settings were saved; false otherwise
		*/
		virtual bool Commit() =0;
		//! \brief Reverts the settings
		virtual void Revert() =0;

		// Settings accessors
		LONG GetLong(const string_t &Key_in, LONG DefaultValue = 0L) const;
		ULONG GetUnsignedLong(const string_t &Key_in, ULONG DefaultValue = 0UL) const;
		const TCHAR* GetString(const string_t &Key_in, const TCHAR* pDefaultValue = _T("")) const;

		void SetHex(const string_t &Key_in, LONG NewValue_in, const TCHAR *pComment_in = nullptr);
		void SetLong(const string_t &Key_in, LONG NewValue_in, const TCHAR *pComment_in = nullptr);		
		void SetString(const string_t &Key_in, const string_t &NewValue_in, const TCHAR *pComment_in = nullptr);

		void SetSettings(WindowerProfile *pSettings_in);
		const string_t& GetPluginName() const
		{ return m_PluginName; }

	protected:

		virtual void UpdateWizardButtons();
		virtual BOOL OnSetActive();

		/*! \brief Initializes the controls of the page from the settings
			\return true if the initialization succeeded; false otherwise
		*/
		virtual bool InitializePage() =0;

		//! name of the plugin
		string_t m_PluginName;
		//! settings
		WindowerProfile *m_pSettings;
		//! general purpose flags
		DWORD m_PageFlags;
		//! Feedback from the page validation
		string_t m_Feedback;
		//! handle of the page icon
		HICON m_hIcon;

	private:
		BOOL OnInitDialog();

		//! resource ID of the dialog template
		UINT m_TemplateResID;
	};
}

#endif//__PLUGIN_PROPERTY_PAGE_H__
