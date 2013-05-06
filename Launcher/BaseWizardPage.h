/**************************************************************************
	created		:	2013-05-05
	filename	: 	BaseWizardPage.h
	author		:	Xebeth`
	copyright	:	North Edge (2013)
	purpose		:	
**************************************************************************/
#ifndef __BASE_WIZARD_PAGE_H__
#define __BASE_WIZARD_PAGE_H__

namespace Windower
{
	PluginSettings;
}

class BaseWizardPage : public Windower::PluginPropertyPage
{
public:
	BaseWizardPage(Windower::PluginSettings *pSettings_in, UINT TemplateResID_in, UINT IconResID_in);

	virtual BOOL OnSetActive();

	bool IsPageValid(string_t *pFeedback_out) const;
	bool InitializePage();
	bool Commit();
	void Revert();
};

#endif//__BASE_WIZARD_PAGE_H__
