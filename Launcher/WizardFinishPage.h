/**************************************************************************
	created		:	2013-05-05
	filename	: 	WizardFinishPage.h
	author		:	Xebeth`
	copyright	:	North Edge (2013)
	purpose		:	
**************************************************************************/
#ifndef __WIZARD_FINISH_PAGE_H__
#define __WIZARD_FINISH_PAGE_H__

class WizardFinishPage : public BaseWizardPage
{
	enum { IDD = IDD_WIZARD_FINISH };
public:
	explicit WizardFinishPage(Windower::WindowerProfile *pSettings_in);

protected:
	DECLARE_MESSAGE_MAP()	
};

#endif//__WIZARD_FINISH_PAGE_H__
