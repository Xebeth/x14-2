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
	class PluginVersion
	{
	public:
		PluginVersion() : Major(0), Minor(0), Build(0) {}
		PluginVersion(short Major_in, short Minor_in, unsigned int Build_in)
			: Major(Major_in), Minor(Minor_in), Build(Build_in) {}
		PluginVersion(const TCHAR *pVersion_in) { FromString(pVersion_in); }

		bool operator == (const PluginVersion& OtherVersion) const
		{
			return (OtherVersion.Major == Major
				 && OtherVersion.Minor == Minor
				 && OtherVersion.Build == Build);
		}

		bool FromString(const TCHAR* pVersion_in) const
		{
			// ex : 1.0.25
			return (_stscanf_s(pVersion_in, _T("%d.%d.%d"),
					&Major, &Minor, &Build) == 3);
		}

		string_t ToString() const
		{
			return format(_T("%d.%d.%d"), Major, Minor, Build);
		}

	private:
		int Major;
		int Minor;
		int Build;
	};
}

#endif//__PLUGIN_VERSION_H__