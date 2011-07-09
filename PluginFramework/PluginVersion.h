/**************************************************************************
	created		:	2011-05-29
	filename	: 	PluginVersion.h
	author		:	xebeth`
	copyright	:	North Edge (2011)
	purpose		:	Plugin version
**************************************************************************/
#ifndef __PLUGIN_VERSION_H__
#define __PLUGIN_VERSION_H__

#include <tchar.h>

namespace PluginFramework
{
	typedef struct _PluginVersion
	{
		_PluginVersion() : Major(0), Minor(0), Build(0) {}
		_PluginVersion(short Major_in, short Minor_in, unsigned int Build_in)
			: Major(Major_in), Minor(Minor_in), Build(Build_in) {}
		_PluginVersion(const TCHAR *pVersion_in) { FromString(pVersion_in); }

		bool operator == (const _PluginVersion& OtherVersion)
		{
			return (OtherVersion.Major == Major
				 && OtherVersion.Minor == Minor
				 && OtherVersion.Build == Build);
		}

		bool FromString(const TCHAR* pVersion_in)
		{
			// ex : 1.0.25
			return (_stscanf_s(pVersion_in, _T("%d.%d.%d"),
					&Major, &Minor, &Build) == 3);
		}

		string_t ToString()
		{
			return format(_T("%d.%d.%d"), Major, Minor, Build);
		}

		int Major;
		int Minor;
		int Build;
	} PluginVersion;
}

#endif//__PLUGIN_VERSION_H__