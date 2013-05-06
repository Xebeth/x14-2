/**************************************************************************
	created		:	2013-05-05
	filename	: 	PluginPropertyPage.cpp
	author		:	Xebeth`
	copyright	:	North Edge (2013)
	purpose		:	
**************************************************************************/
#include "stdafx.h"
#include <afxdlgs.h>

#include "PluginPropertyPage.h"
#include "PluginSettings.h"

namespace Windower
{
	PluginPropertyPage::PluginPropertyPage(PluginSettings *pSettings_in, UINT TemplateResID_in, UINT IconResID_in)
		: CPropertyPage(TemplateResID_in), m_hIcon(AfxGetApp()->LoadIcon(IconResID_in)),
		  m_pSettings(pSettings_in), m_TemplateResID(TemplateResID_in), m_PageFlags(0UL) {}

	BOOL PluginPropertyPage::OnInitDialog()
	{
		SetIcon(m_hIcon, TRUE);
		SetIcon(m_hIcon, FALSE);

		Revert();
		InitializePage();

		return CPropertyPage::OnInitDialog();
	}

	UINT PluginPropertyPage::GetTemplateID() const
	{ return m_TemplateResID; }

	DWORD PluginPropertyPage::GetFlags() const
	{ return m_PageFlags; }

	void PluginPropertyPage::SetFlags(DWORD Flags_in)
	{ m_PageFlags = Flags_in; }

	DWORD PluginPropertyPage::AddFlag(DWORD Flag_in)
	{ m_PageFlags |= Flag_in; return m_PageFlags; }

	DWORD PluginPropertyPage::RemoveFlag(DWORD Flag_in)
	{ m_PageFlags &= ~Flag_in; return m_PageFlags; }

	bool PluginPropertyPage::IsFlagSet(DWORD Flag_in) const
	{ return ((m_PageFlags & Flag_in) == Flag_in); }

	/*! \brief Retrieves the page validation feedback
		\return a const reference to the page validation feedback
	*/
	const string_t& PluginPropertyPage::GetFeedback() const
	{ return m_Feedback; }


	BOOL PluginPropertyPage::OnSetActive()
	{
		InitializePage();
		UpdateWizardButtons();		

		return __super::OnSetActive();
	}

	void PluginPropertyPage::UpdateWizardButtons()
	{
		CPropertySheet *pSheet = static_cast<CPropertySheet*>(GetParent());

		if (pSheet != NULL && pSheet->IsWizard())
		{
			int ActiveIndex = pSheet->GetActiveIndex();
			int PageCount = pSheet->GetPageCount();
			UINT WizardButtons = PSWIZB_CANCEL;
			bool Valid = IsPageValid(NULL);
			
			// handle the next button
			if (ActiveIndex >= PageCount - 2)
			{
				if (Valid == false)
					WizardButtons |= PSWIZB_DISABLEDFINISH;
				else
					WizardButtons |= PSWIZB_FINISH;
			}
			else if (Valid)
				WizardButtons |= PSWIZB_NEXT;
			// handle the previous button
			if (ActiveIndex > 0)
				WizardButtons |= PSWIZB_BACK;

			pSheet->SetWizardButtons(WizardButtons);
		}
	}

	/*! \brief Retrieves the long value corresponding to the specified key in the current section
		\param[in] Key_in : the name of the key
		\param[in] DefaultValue : the default value if the key wasn't found
		\return the long value of the key if valid; DefaultValue otherwise
	*/
	LONG PluginPropertyPage::GetLong(const string_t &Key_in, LONG DefaultValue) const
	{
		if (m_pSettings != NULL)
			return m_pSettings->GetLong(Key_in.c_str(), DefaultValue);
		else
			return DefaultValue;
	}

	/*! \brief Retrieves the DWORD value corresponding to the specified key in the current section
		\param[in] Key_in : the name of the key
		\param[in] DefaultValue : the default value if the key wasn't found
		\return the DWORD value of the key if valid; DefaultValue otherwise
	*/
	ULONG PluginPropertyPage::GetUnsignedLong(const string_t &Key_in, ULONG DefaultValue) const
	{
		if (m_pSettings != NULL)
			return m_pSettings->GetUnsignedLong(Key_in.c_str(), DefaultValue);
		else
			return DefaultValue;
	}

	/*! \brief Sets the specified key value as a long in the current section
		\param[in] Key_in : the name of the key
		\param[in] NewValue_in : the new value for the key
	*/
	void PluginPropertyPage::SetLong(const string_t &Key_in, LONG NewValue_in, const TCHAR *pComment_in)
	{
		if (m_pSettings != NULL)
			m_pSettings->SetLong(Key_in.c_str(), NewValue_in, pComment_in);
	}

	/*! \brief Sets the specified key value as an hexadecimal representation of a long in the current section
		\param[in] Key_in : the name of the key
		\param[in] NewValue_in : the new value for the key
	*/
	void PluginPropertyPage::SetHex(const string_t &Key_in, LONG NewValue_in, const TCHAR *pComment_in)
	{
		if (m_pSettings != NULL)
			m_pSettings->SetHex(Key_in.c_str(), NewValue_in, pComment_in);
	}

	/*! \brief Retrieves the string value corresponding to the specified key in the current section
		\param[in] Key_in : the name of the key
		\param[in] pDefaultValue : the default value if the key wasn't found
		\return the string value of the key if valid; pDefaultValue otherwise
	*/
	const TCHAR* PluginPropertyPage::GetString(const string_t &Key_in, const TCHAR* pDefaultValue) const
	{
		if (m_pSettings != NULL)
			return m_pSettings->GetString(Key_in.c_str(), pDefaultValue);
		else
			return pDefaultValue;
	}

	/*! \brief Sets the specified key value as string in the current section
		\param[in] Key_in : the name of the key
		\param[in] NewValue_in : the new value for the key
	*/
	void PluginPropertyPage::SetString(const string_t &Key_in, const string_t &NewValue_in, const TCHAR *pComment_in) 
	{
		if (m_pSettings != NULL)
			m_pSettings->SetString(Key_in.c_str(), NewValue_in.c_str(), pComment_in);
	}


	/*! \brief Checks if the specified key exists within the current section
		\param[in] KeyName_in : key to check
		\return true if the key exists in the specified section; false otherwise
	*/
	bool PluginPropertyPage::KeyExists(const string_t &KeyName_in) const
	{
		return (m_pSettings != NULL && m_pSettings->KeyExists(KeyName_in.c_str()));
	}

	/*! \brief Deletes the specified key exists within the current section
		\param[in] KeyName_in : key to delete
		\return true if the key was deleted in the specified section; false otherwise
	*/
	bool PluginPropertyPage::DeleteKey(const string_t &KeyName_in) const
	{
		return (m_pSettings != NULL && m_pSettings->DeleteKey(KeyName_in.c_str()));
	}

	/*! \brief Retrieves the drive of the settings file
		\return the drive of the settings file
	*/
	string_t PluginPropertyPage::GetSettingsDrive() const
	{
		if (m_pSettings != NULL)
			return m_pSettings->GetSettingsDrive();
		else
			return _T("C:\\");		
	}
	/*! \brief Sets the name of the current section
		\param[in] CurrentSection_in : the name of the new current section
	*/
	bool PluginPropertyPage::SetCurrentSection(const string_t &CurrentSection_in)
	{
		if (m_pSettings != NULL && m_pSettings->GetCurrentSection().compare(CurrentSection_in) != 0)
		{
			m_pSettings->SetCurrentSection(CurrentSection_in);

			return true;
		}

		return false;
	}

	/*! \brief Retrieves the name of the profile
		\return the name of the profile if set; 'Default' otherwise
	*/
	const TCHAR* PluginPropertyPage::GetProfileName() const
	{
		if (m_pSettings != NULL)
			return m_pSettings->GetProfileName();
		else
			return _T("Default");
	}
}