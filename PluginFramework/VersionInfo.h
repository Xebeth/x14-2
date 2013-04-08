/**************************************************************************
	created		:	2011-05-29
	filename	: 	VersionInfo.h
	author		:	xebeth`
	copyright	:	North Edge (2011)
	purpose		:	Plugin version
**************************************************************************/
#ifndef __PLUGIN_VERSION_H__
#define __PLUGIN_VERSION_H__

#ifndef _PLUGIN_FRAMEWORK_SDK_INCLUDE_
	#error Please include the global header file 'PluginFramework.h'
#endif

namespace PluginFramework
{
	//! \brief Plugin version
	class VersionInfo
	{
	public:
		//! \brief VersionInfo default constructor
		VersionInfo() : Major(0), Minor(0), Build(0) {}
		/*! \brief VersionInfo constructor
			\param[in] Major_in : major version number (1.x.x)
			\param[in] Minor_in : minor version number (x.0.x)
			\param[in] Build_in : build version number (x.x.0)
		*/
		VersionInfo(short Major_in, short Minor_in, unsigned int Build_in)
			: Major(Major_in), Minor(Minor_in), Build(Build_in) {}
		/*! \brief VersionInfo constructor
			\param[in] pVersion_in : string representation of the version
		*/
		VersionInfo(const TCHAR *pVersion_in) { FromString(pVersion_in); }

		/*! \brief Version comparison operator
			\param[in] OtherVersion_in : the version to compare to
			\return true if the the versions are equal; false otherwise
		*/
		bool operator == (const VersionInfo& OtherVersion_in) const
		{
			return (OtherVersion_in.Major == Major
				 && OtherVersion_in.Minor == Minor
				 && OtherVersion_in.Build == Build);
		}

		/*! \brief Converts a string to a version object
			\param[in] pVersion_in : string representation of the version
			\return true if the conversion was successful; false otherwise
		*/
		bool FromString(const TCHAR* pVersion_in) const
		{
			// ex : 1.0.25
			return (_stscanf_s(pVersion_in, _T("%d.%d.%d"),
					&Major, &Minor, &Build) == 3);
		}

		/*! \brief Retrieves the string representation of the version
			\return the string representation of the version
		*/
		string_t ToString() const
		{
			return format(_T("%d.%d.%d"), Major, Minor, Build);
		}

	private:
		//! major version number (1.x.x)
		int Major;
		//! minor version number (x.0.x)
		int Minor;
		//! build version number (x.x.0)
		int Build;
	};
}

#endif//__PLUGIN_VERSION_H__