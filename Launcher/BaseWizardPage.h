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
	WindowerProfile;
}

class BaseWizardPage : public Windower::PluginPropertyPage
{
public:
	BaseWizardPage(Windower::WindowerProfile *pSettings_in,
				   const string_t &PluginName_in,
				   UINT TemplateResID_in, UINT IconResID_in);

	virtual BOOL OnSetActive();

	bool IsPageValid(string_t *pFeedback_out) const;
	bool InitializePage();
	bool Commit();
	void Revert();
};

#endif//__BASE_WIZARD_PAGE_H__
