/**************************************************************************
	created		:	2011-05-29
	filename	: 	PluginUUID.h
	author		:	xebeth`
	copyright	:	North Edge (2011)
	purpose		:	Plugin unique identifier
**************************************************************************/
#ifndef __PLUGIN_UUID_H__
#define __PLUGIN_UUID_H__

#include <tchar.h>

namespace PluginFramework
{
	typedef struct _PluginUUID
	{
		_PluginUUID() : m_Data1(0), m_Data2(0), m_Data3(0),
			m_Data4(0), m_Data5_1(0), m_Data5_2(0) {}
		_PluginUUID(TCHAR* pUID_in) { FromString(pUID_in); }

		_PluginUUID& FromString(TCHAR* pUID_in)
		{
			// ex : BC725A17-4E60-4EE2-9E48-EF33D7CBB7E9
			_stscanf_s(pUID_in, _T("%08X-%04X-%04X-%04X-%08X%04X"),
					   &m_Data1, &m_Data2, &m_Data3,
					   &m_Data4, &m_Data5_1, &m_Data5_2);

			return *this;
		}

		string_t ToString() const
		{
			return format(_T("%08X-%04X-%04X-%04X-%08X%04X"),
						  m_Data1, m_Data2, m_Data3, m_Data4, m_Data5_1, m_Data5_2);
		}

		bool operator == (const _PluginUUID &OtherUUID) const
		{
			return (m_Data1 == OtherUUID.m_Data1
				 && m_Data2 == OtherUUID.m_Data2
				 && m_Data3 == OtherUUID.m_Data3
				 && m_Data4 == OtherUUID.m_Data4
				 && m_Data5_1 == OtherUUID.m_Data5_1
				 && m_Data5_2 == OtherUUID.m_Data5_2);
		}
		// meaningless operator but required by std::set
		bool operator < (const _PluginUUID &OtherUUID) const
		{
			return (m_Data1 < OtherUUID.m_Data1);
		}


		DWORD	m_Data1;
		WORD	m_Data2;
		WORD	m_Data3;
		WORD	m_Data4;
		DWORD	m_Data5_1;
		WORD	m_Data5_2;
	} PluginUUID;
}

#endif//__PLUGIN_UUID_H__