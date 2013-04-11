/**************************************************************************
	created		:	2013-03-05
	filename	: 	Point.h
	author		:	Xebeth`
	copyright	:	North Edge (2013)
	purpose		:	Point class
**************************************************************************/
#ifndef __POINT_3D_H__
#define __POINT_3D_H__

#ifndef _UIAL_SDK_INCLUDE_
	#error Please include the global header file 'UIAL.h'
#endif

namespace UIAL
{
	template <typename T, typename U = RHCS > class CPoint : private U
	{
	public:
		//! \brief CPoint default constructor
		CPoint() : m_X(0), m_Y(0), m_Z(0) {}

		/*! \brief CPoint constructor
			\param[in] X_in : the new x-coordinate of the point
			\param[in] Y_in : the new y-coordinate of the point
			\param[in] Z_in : the new y-coordinate of the point
		*/
		CPoint(T X_in, T Y_in, T Z_in = (T)0)
			: m_X(X_in), m_Y(Y_in), m_Z(Z_in) {}

		/*! \brief Equality operator
			\param[in] Rhs_in : the point to compare to this point
			\return true if the points have the same position; false otherwise
		*/
		bool operator == (const CPoint<T,U> &Rhs_in) const
		{
			return (m_X == Rhs_in.m_X && m_Y == Rhs_in.m_Y && m_Z == Rhs_in.m_Z);
		}

		/*! \brief Difference operator
			\param[in] Rhs_in : the point to compare to this point
			\return true if the points have the same position; false otherwise
		*/
		bool operator != (const CPoint<T,U> &Rhs_in) const
		{
			return (m_X != Rhs_in.m_X || m_Y != Rhs_in.m_Y || m_Z != Rhs_in.m_Z);
		}

		/*! \brief Addition operator
			\param[in] Rhs_in : the point to add to this point
			\return a reference to this point
		*/
		CPoint& operator + (const CPoint<T,U> &Rhs_in)
		{
			m_X += Rhs_in.m_X;
			m_Y += Rhs_in.m_Y;
			m_Z += Rhs_in.m_Z;

			return *this;
		}

		/*! \brief Subtraction operator
			\param[in] Rhs_in : the point to subtract from this point
			\return a reference to this pointB
		*/
		CPoint& operator - (const CPoint<T,U> &Rhs_in)
		{
			m_X -= Rhs_in.m_X;
			m_Y -= Rhs_in.m_Y;
			m_Z -= Rhs_in.m_Z;

			return *this;
		}

		/*! \brief Addition affectation operator
			\param[in] Rhs_in : the point to add to this point
		*/
		void operator += (const CPoint<T,U> &Rhs_in)
		{
			*this = *this + Rhs_in;
		}

		/*! \brief Subtraction affectation operator
			\param[in] Rhs_in : the point to subtract from this point
		*/
		void operator -= (const CPoint<T,U> &Rhs_in)
		{
			*this = *this - Rhs_in;
		}
	
		/*! \brief Retrieves the x-coordinate of the point
			\return the x-coordinate of the point
		*/
		T GetX() const
		{
			return m_X;
		}

		/*! \brief Retrieves the y-coordinate of the point
			\return the y-coordinate of the point
		*/
		T GetY() const
		{
			return m_Y;
		}

		/*! \brief Retrieves the z-coordinate of the point
			\return the z-coordinate of the point
		*/
		T GetZ() const
		{
			return m_Z;
		}

		/*! \brief Retrieves the position of the point
			\param[out] X_out : the x-coordinate of the point
			\param[out] Y_out : the y-coordinate of the point
			\param[out] Z_out : the z-coordinate of the point
		*/
		void GetPos(T &X_out, T &Y_out, T &Z_out) const
		{
			X_out = m_X;
			Y_out = m_Y;
			Z_out = m_Z;
		}

		/*! \brief Sets the position of the point
			\param[in] X_in : the new x-coordinate of the point
			\param[in] Y_in : the new y-coordinate of the point
			\return true if the coordinates changed; false otherwise
		*/
		bool SetPos(T X_in, T Y_in, T Z_in)
		{
			bool Result = (m_X != X_in || m_Y != Y_in || m_Z != Z_in);

			// update the position
			m_X = X_in;
			m_Y = Y_in;
			m_Z = Z_in;

			return Result;
		}

		/*! \brief Sets the x-coordinate of the point
			\param[in] X_in : the new x-coordinate of the point
			\return true if the coordinates changed; false otherwise
		*/
		bool SetX(T X_in)
		{
			bool Result = (m_X != X_in);

			// update the x-coordinate
			m_X = X_in;

			return Result;
		}

		/*! \brief Sets the y-coordinate of the point
			\param[in] Y_in : the new y-coordinate of the point
			\return true if the coordinates changed; false otherwise
		*/
		bool SetY(T Y_in)
		{
			bool Result = (m_Y != Y_in);

			// update the y-coordinate
			m_Y = Y_in;

			return Result;
		}

		/*! \brief Sets the z-coordinate of the point
			\param[in] Z_in : the new z-coordinate of the point
			\return true if the coordinates changed; false otherwise
		*/
		bool SetZ(T Z_in)
		{
			bool Result = (m_Z != Z_in);

			// update the z-coordinate
			m_Z = Z_in;

			return Result;
		}

		/*! \brief Moves the point by the specified offsets
			\param[in] XOffset_in : the offset in x-coordinate
			\param[in] YOffset_in : the offset in y-coordinate
			\param[in] ZOffset_in : the offset in z-coordinate
			\return true if the coordinates changed; false otherwise
		*/
		bool Move(T XOffset_in, T YOffset_in, T ZOffset_in)
		{
			if (XOffset_in || YOffset_in || ZOffset_in)
			{
				// update the coordinates of the point
				m_X += XOffset_in;
				m_Y += YOffset_in;
				m_Z += ZOffset_in;

				return true;
			}

			return false;
		}

		/*! \brief Exchanges the coordinates of the point with another
			\param[in,out] Source_in_out : the point to exchange with
			\return a reference to this point
		*/
		CPoint& Swap(CPoint &Point_in_out)
		{
			T X, Y, Z;

			// backup the coordinates
			X = m_X;
			Y = m_Y;
			Z = m_Z;
			// update this point
			m_X = Point_in_out.m_X;
			m_Y = Point_in_out.m_Y;
			m_Z = Point_in_out.m_Z;
			// update the other point
			Point_in_out.m_X = X;
			Point_in_out.m_Y = Y;
			Point_in_out.m_Z = Z;

			return *this;
		}

		bool IsLeft(const CPoint<T,U>& Point_in)
		{
			return is_left(m_X, Point_in.m_X);
		}

		bool IsRight(const CPoint<T,U>& Point_in)
		{
			return is_right(m_X, Point_in.m_X);
		}

		bool IsAbove(const CPoint<T,U>& Point_in)
		{
			return is_above(m_Y, Point_in.m_Y);
		}

		bool IsBelow(const CPoint<T,U>& Point_in)
		{
			return is_below(m_Y, Point_in.m_Y);
		}

		bool IsBack(const CPoint<T,U>& Point_in)
		{
			return is_back(m_Z, Point_in.m_Z);
		}

		bool IsFront(const CPoint<T,U>& Point_in)
		{
			return is_front(m_Z, Point_in.m_Z);
		}

		bool IsAlignedX(const CPoint<T,U>& Point_in)
		{
			return (m_X == Point_in.m_X);
		}

		bool IsAlignedY(const CPoint<T,U>& Point_in)
		{
			return (m_Y == Point_in.m_Y);
		}

		bool IsAlignedZ(const CPoint<T,U>& Point_in)
		{
			return (m_Z == Point_in.m_Z);
		}

	protected:
		//! coordinates of the point
		T m_X, m_Y, m_Z;
	};
}

#endif//__POINT_3D_H__
