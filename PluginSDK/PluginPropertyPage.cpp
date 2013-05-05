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

namespace Windower
{
	PluginPropertyPage::PluginPropertyPage(PluginSettings *pSettings_in, UINT TemplateResID_in, UINT IconResID_in)
		: CPropertyPage(TemplateResID_in), m_pSettings(pSettings_in),
		  m_hIcon(AfxGetApp()->LoadIcon(IconResID_in)) {}

	BOOL PluginPropertyPage::OnInitDialog()
	{
		SetIcon(m_hIcon, TRUE);
		SetIcon(m_hIcon, FALSE);

		return CPropertyPage::OnInitDialog();
	}
}