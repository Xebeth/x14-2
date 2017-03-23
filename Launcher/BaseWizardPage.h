/**************************************************************************
	created		:	2013-05-05
	filename	: 	BaseWizardPage.h
	author		:	Xebeth`
	copyright	:	North Edge (2013)
	purpose		:	
**************************************************************************/
#ifndef __BASE_WIZARD_PAGE_H__
#define __BASE_WIZARD_PAGE_H__

class BaseWizardPage : public Windower::PluginPropertyPage
{
public:
	BaseWizardPage(Windower::WindowerProfile *pSettings_in,
				   const string_t &PluginName_in,
				   UINT TemplateResID_in, UINT IconResID_in);

	BOOL OnSetActive() override;

	bool IsPageValid(string_t *pFeedback_out) const override;
	bool InitializePage() override;
	bool Commit() override;
	void Revert() override;
};

#endif//__BASE_WIZARD_PAGE_H__
