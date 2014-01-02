/**************************************************************************
	created		:	2011-05-29
	filename	: 	UUID.h
	author		:	xebeth`
	copyright	:	North Edge (2011)
	purpose		:	Unique identifier
**************************************************************************/
#ifndef __UUID_H__
#define __UUID_H__

#ifndef _STRING_UTILS_SDK_INCLUDE_
	#error Please include the global header file 'StringUtils.h'
#endif

namespace StringUtils
{
	//! \brief unique identifier
	class UUID
	{
	public:
		//! \brief UUID default constructor
		UUID() : m_Data1(0), m_Data2(0), m_Data3(0),
				 m_Data4(0), m_Data5_1(0), m_Data5_2(0) {}
		/*! \brief UUID constructor
			\param[in] pUID_in : the string version of the UUID
		*/
		explicit UUID(const TCHAR* pUID_in) { FromString(pUID_in); }

		/*! \brief Implicit conversion to a string buffer
			\return the string representation of the UUID
		*/
		operator const TCHAR*() const { return ToString().c_str(); }

		/*! \brief Initializes the UUID from a string
			\param[in] pUID_in : the string representation of the UUID
			\return a reference to the UUID
		*/
		UUID& FromString(const TCHAR* pUID_in)
		{
			// ex : BC725A17-4E60-4EE2-9E48-EF33D7CBB7E9
			_stscanf_s(pUID_in, _T("%08X-%04X-%04X-%04X-%08X%04X"),
					   &m_Data1, &m_Data2, &m_Data3,
					   &m_Data4, &m_Data5_1, &m_Data5_2);

			return *this;
		}

		/*! \brief Retrieves the string representation of the UUID
			\return the string representation of the UUID
		*/
		string_t ToString() const
		{
			return format(_T("%08X-%04X-%04X-%04X-%08X%04X"),
						  m_Data1, m_Data2, m_Data3, m_Data4, m_Data5_1, m_Data5_2);
		}

		/*! \brief Compares two UUID
			\param[in] OtherUUID : the UUID to compare to
			\return true if the UUIDs are equal; false otherwise
		*/
		bool operator == (const UUID &OtherUUID) const
		{
			return (m_Data1 == OtherUUID.m_Data1
				 && m_Data2 == OtherUUID.m_Data2
				 && m_Data3 == OtherUUID.m_Data3
				 && m_Data4 == OtherUUID.m_Data4
				 && m_Data5_1 == OtherUUID.m_Data5_1
				 && m_Data5_2 == OtherUUID.m_Data5_2);
		}
		/*! \brief Meaningless operator comparison operator required by std::set
			\param[in] OtherUUID : the UUID to compare to
			\return bool
		*/
		bool operator < (const UUID &OtherUUID) const
		{
			return (OtherUUID.m_Data1 < m_Data1);
		}

	protected:
		//! first data chunk						(BC725A17-XXXX-XXXX-XXXX-XXXXXXXXXXXX)
		DWORD	m_Data1;
		//! second data chunk						(XXXXXXXX-4E60-XXXX-XXXX-XXXXXXXXXXXX)
		DWORD	m_Data2;
		//! third data chunk						(XXXXXXXX-XXXX-4EE2-XXXX-XXXXXXXXXXXX)
		DWORD	m_Data3;
		//! fourth data chunk						(XXXXXXXX-XXXX-XXXX-9E48-XXXXXXXXXXXX)
		DWORD	m_Data4;
		//! first part of the fifth data chunk		(XXXXXXXX-XXXX-XXXX-XXXX-EF33D7CBXXXX)
		DWORD	m_Data5_1;
		//! second part of the fifth data chunk		(XXXXXXXX-XXXX-XXXX-XXXX-XXXXXXXXB7E9)
		DWORD	m_Data5_2;
	};
}

#endif//__UUID_H__