/**************************************************************************
	created		:	2011-06-03
	filename	: 	SettingsInterface.h
	author		:	Xebeth`
	copyright	:	North Edge (2011)
	purpose		:	Settings interface
**************************************************************************/
#ifndef __SETTINGS_INTERFACE_H__
#define __SETTINGS_INTERFACE_H__

namespace Settings
{
	class SettingsInterface
	{
	public:
		// Data source functions
		/*! \brief Loads the settings from the settings
			\return true if successful; false otherwise
		*/
		virtual bool Load() =0;
		/*! \brief Saves the settings to the settings
			\return true if successful; false otherwise
		*/
		virtual bool Save() =0;

		// Sections functions
		/*! \brief Creates a new section in the settings
			\param[in] SectionName_in : the name of the new section
			\return true if successful; false otherwise
		*/
		virtual bool CreateSection(const string_t &SectionName_in) =0;
		/*! \brief Deletes a section from the settings
			\param[in] SectionName_in : the name of the section to delete
			\return true if successful; false otherwise
		*/
		virtual bool DeleteSection(const string_t &SectionName_in) =0;

		// Settings accessors
		/*! \brief Retrieves the long value corresponding to the specified key and section
			\param[in] SectionName_in : the section containing the key
			\param[in] Key_in : the name of the key
			\return the long value of the key if valid; 0 otherwise
		*/
		virtual LONG GetLong(const string_t &SectionName_in, const string_t &Key_in) const =0;
		/*! \brief Sets the specified key value as a long in the specified section
			\param[in] SectionName_in : the section containing the key
			\param[in] Key_in : the name of the key
			\param[in] NewValue_in : the new value for the key
		*/
		virtual void SetLong(const string_t &SectionName_in, const string_t &Key_in, LONG NewValue_in) =0;
		/*! \brief Retrieves the string value corresponding to the specified key and section
			\param[in] SectionName_in : the section containing the key
			\param[in] Key_in : the name of the key
			\return the string value of the key if valid; 0 otherwise
		*/
		virtual const TCHAR* GetString(const string_t &SectionName_in, const string_t &Key_in) const =0;
		/*! \brief Sets the specified key value as string in the specified section
			\param[in] SectionName_in : the section containing the key
			\param[in] Key_in : the name of the key
			\param[in] NewValue_in : the new value for the key
		*/
		virtual void SetString(const string_t &SectionName_in, const string_t &Key_in, const string_t &NewValue_in) =0;
	};
}

#endif//__SETTINGS_INTERFACE_H__