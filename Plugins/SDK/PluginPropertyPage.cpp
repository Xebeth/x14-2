/**************************************************************************
	created		:	2013-05-05
	filename	: 	PluginPropertyPage.cpp
	author		:	Xebeth`
	copyright	:	North Edge (2013)
	purpose		:	
**************************************************************************/
#include "stdafx.h"
#include <afxdlgs.h>

#include "WindowerSettings.h"
#include "PluginPropertyPage.h"

namespace Windower
{
	PluginPropertyPage::PluginPropertyPage(WindowerProfile *pSettings_in,
										   const string_t &PluginName_in,
										   UINT TemplateResID_in,
										   UINT IconResID_in)
		: CPropertyPage(TemplateResID_in), m_hIcon(AfxGetApp()->LoadIcon(IconResID_in)),
		  m_pSettings(pSettings_in), m_PluginName(PluginName_in),
		  m_TemplateResID(TemplateResID_in), m_PageFlags(0UL)
	{
		PluginFramework::IPlugin::LoadSettings(m_pSettings);
	}

	PluginPropertyPage::~PluginPropertyPage()
	{
		m_pSettings = nullptr;
	}

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
		UpdateWizardButtons();

		return __super::OnSetActive();
	}

	void PluginPropertyPage::UpdateWizardButtons()
	{
		CPropertySheet *pSheet = static_cast<CPropertySheet*>(GetParent());

		if (pSheet != nullptr && pSheet->IsWizard())
		{
			int ActiveIndex = pSheet->GetActiveIndex();
			int PageCount = pSheet->GetPageCount();
			UINT WizardButtons = PSWIZB_CANCEL;
			bool Valid = IsPageValid(nullptr);
			
			// handle the next button
			if (ActiveIndex >= PageCount - 1)
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

	void PluginPropertyPage::SetSettings(Windower::WindowerProfile *pSettings_in)
	{
		m_pSettings = pSettings_in;
	}
}