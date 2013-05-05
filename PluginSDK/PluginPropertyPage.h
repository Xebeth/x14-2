/**************************************************************************
	created		:	2013-05-05
	filename	: 	PluginPropertyPage.h
	author		:	Xebeth`
	copyright	:	North Edge (2013)
	purpose		:	
**************************************************************************/
#ifndef __PLUGIN_PROPERTY_PAGE_H__
#define __PLUGIN_PROPERTY_PAGE_H__

namespace Windower
{
	class PluginSettings;

	class PluginPropertyPage : public CPropertyPage
	{
	public:
		PluginPropertyPage(PluginSettings *pSettings_in, UINT TemplateResID_in, UINT IconResID_in);

		virtual bool Save() =0;

	protected:
		PluginSettings *m_pSettings;
		HICON m_hIcon;

		virtual BOOL OnInitDialog();
	};
}

#endif//__PLUGIN_PROPERTY_PAGE_H__
