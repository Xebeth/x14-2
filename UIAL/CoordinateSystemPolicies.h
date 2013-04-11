/**************************************************************************
	created		:	2013-03-06
	filename	: 	CoordinateSystemPolicies.h
	author		:	Xebeth`
	copyright	:	North Edge (2013)
	purpose		:	Coordinate system policies
**************************************************************************/
#ifndef __COORDINATE_SYSTEM_POLICIES_H__
#define __COORDINATE_SYSTEM_POLICIES_H__

#ifndef _UIAL_SDK_INCLUDE_
	#error Please include the global header file 'UIAL.h'
#endif

namespace UIAL
{
	/*! \brief right-handed Cartesian system
				* the origin is the bottom-left corner of the screen
				* z-order is positive towards the user
	*/
	class RHCS
	{
	protected:
		//! \brief RHCS default constructor
		RHCS() : m_XOrientation(true), 
				 m_YOrientation(true),
				 m_ZOrientation(true) {}

		/*! \brief Checks if an x-coordinate is visually left of another
			\param[in] X1_in : the first coordinate
			\param[in] X2_in : the second coordinate
			\return true if the first coordinate is left of the second one
		*/
		template <typename T> bool is_left(T X1_in, T X2_in)
		{ return m_XOrientation ? (X1_in < X2_in) : (X1_in > X2_in); }
		/*! \brief Checks if an x-coordinate is visually right of another
			\param[in] X1_in : the first coordinate
			\param[in] X2_in : the second coordinate
			\return true if the first coordinate is right of the second one
		*/
		template <typename T> bool is_right(T X1_in, T X2_in)
		{ return m_XOrientation ? (X1_in > X2_in) : (X1_in < X2_in); }
		/*! \brief Checks if an y-coordinate is visually below another
			\param[in] Y1_in : the first coordinate
			\param[in] Y2_in : the second coordinate
			\return true if the first coordinate is below the second one
		*/
		template <typename T> bool is_below(T Y1_in, T Y2_in)
		{ return m_YOrientation ? (Y1_in < Y2_in) : (Y1_in > Y2_in); }
		/*! \brief Checks if an y-coordinate is visually above another
			\param[in] Y1_in : the first coordinate
			\param[in] Y2_in : the second coordinate
			\return true if the first coordinate is above the second one
		*/
		template <typename T> bool is_above(T Y1_in, T Y2_in)
		{ return m_YOrientation ? (Y1_in > Y2_in) : (Y1_in < Y2_in); }
		/*! \brief Checks if an z-coordinate is visually at the back of another
			\param[in] Z1_in : the first coordinate
			\param[in] Z2_in : the second coordinate
			\return true if the first coordinate is at the back of the second one
		*/
		template <typename T> bool is_back(T Z1_in, T Z2_in)
		{ return m_ZOrientation ? (Z1_in < Z2_in) : (Z1_in > Z2_in); }
		/*! \brief Checks if an z-coordinate is visually in front of another
			\param[in] Z1_in : the first coordinate
			\param[in] Z2_in : the second coordinate
			\return true if the first coordinate is in front of the second one
		*/
		template <typename T> bool is_front(T Z1_in, T Z2_in)
		{ return m_ZOrientation ? (Z1_in > Z2_in) : (Z1_in < Z2_in); }

		//! the orientation of the x-axis compared to a right-handed Cartesian system (true: same orientation, false: opposite)
		bool m_XOrientation;
		//! the orientation of the y-axis compared to a right-handed Cartesian system (true: same orientation, false: opposite)
		bool m_YOrientation;
		//! the orientation of the z-axis compared to a right-handed Cartesian system (true: same orientation, false: opposite)
		bool m_ZOrientation;
	};

	/*! \brief mirrored right-handed Cartesian system
				* the origin is the bottom-right corner of the screen
				* z-order is positive towards the user
	*/
	class MRHCS : protected RHCS
	{
	public:
		//! \brief MRHCS default constructor
		MRHCS() { m_XOrientation = false; m_YOrientation = true; m_ZOrientation = true; }
	};

	/*! \brief flipped right-handed Cartesian system
				* the origin is the top-left corner of the screen
				* z-order is positive towards the user
	*/
	class FRHCS : protected RHCS
	{
	public:
		//! \brief FRHCS default constructor
		FRHCS() { m_XOrientation = true; m_YOrientation = false; m_ZOrientation = true; }
	};

	/*! \brief flipped & mirrored right-handed Cartesian system
				* the origin is the top-right corner of the screen
				* z-order is positive towards the user
	*/
	class FMRHCS : protected RHCS
	{
	public:
		//! \brief FMRHCS default constructor
		FMRHCS() { m_XOrientation = false; m_YOrientation = false; m_ZOrientation = true; }
	};

	/*! \brief left-handed Cartesian system
				* the origin is the bottom-left corner of the screen
				* z-order is negative towards the user
	*/
	class LHCS : protected RHCS
	{
	public:
		//! \brief LHCS default constructor
		LHCS() { m_XOrientation = true; m_YOrientation = true; m_ZOrientation = false; }
	};

	/*! \brief mirrored left-handed Cartesian system
				* the origin is the bottom-right corner of the screen
				* z-order is negative towards the user
	*/
	class MLHCS : protected RHCS
	{
	public:
		//! \brief MLHCS default constructor
		MLHCS() { m_XOrientation = false; m_YOrientation = true; m_ZOrientation = false; }
	};

	/*! \brief flipped left-handed Cartesian system
				* the origin is the bottom-right corner of the screen
				* z-order is negative towards the user
	*/
	class FLHCS : protected RHCS
	{
	public:
		//! \brief FLHCS default constructor
		FLHCS() { m_XOrientation = true; m_YOrientation = false; m_ZOrientation = false; }
	};

	/*! \brief flipped & mirrored left-handed Cartesian system
				* the origin is the top-right corner of the screen
				* z-order is negative towards the user
	*/
	class FMLHCS : protected RHCS
	{
	public:
		//! \brief FMLHCS default constructor
		FMLHCS() { m_XOrientation = false; m_YOrientation = false; m_ZOrientation = false; }
	};
}

#endif//__COORDINATE_SYSTEM_POLICIES_H__
