/**************************************************************************
	created		:	2013-08-22
	filename	: 	IUserSettings.h
	author		:	Xebeth`
	copyright	:	North Edge (2013)
	purpose		:	
**************************************************************************/
#ifndef __IUSERSETTINGS_H__
#define __IUSERSETTINGS_H__

namespace PluginFramework
{
	class IUserSettings
	{
	public:
		// Settings accessors
		virtual const TCHAR* GetString(const string_t &Key_in, const TCHAR* pDefaultValue = _T("")) const =0;
		virtual ULONG GetUnsignedLong(const string_t &Key_in, ULONG DefaultValue = 0UL) const =0;
		virtual LONG GetLong(const string_t &Key_in, LONG DefaultValue = 0L) const =0;	

		virtual void SetString(const string_t &Key_in, const string_t &NewValue_in, const TCHAR *pComment_in = NULL) =0;
		virtual void SetUnsignedLong(const string_t &Key_in, ULONG NewValue_in, const TCHAR *pComment_in = NULL) =0;
		virtual void SetLong(const string_t &Key_in, LONG NewValue_in, const TCHAR *pComment_in = NULL) =0;		
	};
}

#endif//__IUSERSETTINGS_H__
