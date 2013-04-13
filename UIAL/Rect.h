/**************************************************************************
	created		:	2013-03-05
	filename	: 	Rect.h
	author		:	Xebeth`
	copyright	:	North Edge (2013)
	purpose		:	Rectangle class
**************************************************************************/
#ifndef __RECT_3D_H__
#define __RECT_3D_H__

#ifndef _UIAL_SDK_INCLUDE_
	#error Please include the global header file 'UIAL.h'
#endif

namespace UIAL
{
	template<typename T, typename U = RHCS> class CRect
	{
	public:
		CRect() {}
		/*! \brief CRect constructor
			\param[in] Left_in : the x-coordinate of the top-left corner
			\param[in] Top_in : the y-coordinate of the top-left corner
			\param[in] Right_in : the x-coordinate of the bottom-right corner
			\param[in] Bottom_in : the y-coordinate of the bottom-right corner			
		*/
		CRect(T Left_in, T Top_in, T Right_in, T Bottom_in)
			: m_Point2(Right_in, Bottom_in, (T)0), m_Point1(Left_in, Top_in, (T)0)
		{
			// normalize the points so that the first point has the lowest 
			// coordinates and the second one has the highest
			NormalizePoints(m_Point1, m_Point2);
		}

		/*! \brief CRect constructor
			\param[in] FirstPoint_in : the x-coordinate of the top-left corner
			\param[in] Width_in : the y-coordinate of the top-left corner
			\param[in] Height_in : the x-coordinate of the bottom-right corner
		*/
		CRect(const CPoint<T,U>& FirstPoint_in, T Width_in, T Height_in)
			: m_Point1(FirstPoint_in), m_Point2(FirstPoint_in)
		{
			m_Point2.Move(Width_in, Height_in, (T)0);
			// normalize the points so that the first point has the lowest 
			// coordinates and the second one has the highest
			NormalizePoints(m_Point1, m_Point2);
		}

		/*! \brief CRect constructor
			\param[in] TopLeft_in : top-left corner of the rectangle
			\param[in] BottomRight_in : bottom-right corner of the rectangle
		*/
		CRect(const CPoint<T,U>& TopLeft_in, const CPoint<T,U>& BottomRight_in)
			: m_Point2(BottomRight_in), m_Point1(TopLeft_in)
		{
			// normalize the points so that the first point has the lowest 
			// coordinates and the second one has the highest
			NormalizePoints(m_Point1, m_Point2);
		}


		/*! \brief Equality operator
			\param[in] Rect_in : the rectangle to compare with this rectangle
			\return true if the rectangles are equal; false otherwise
		*/
		bool operator == (const CRect<T,U>& Rect_in) const
		{
			return (m_Point2 == Rect_in.m_Point2 && m_Point1 == Rect_in.m_Point1);
		}

		/*! \brief Difference operator
			\param[in] Rect_in : the rectangle to compare with this rectangle
			\return true if the rectangles are different; false otherwise
		*/
		bool operator != (const CRect<T,U>& Rect_in) const
		{
			return (m_Point2 != Rect_in.m_Point2 || m_Point1 != Rect_in.m_Point1);
		}

		/*! \brief Expands the rectangle uniformly on each side
			\param[in] OffsetX_in : the offset to add to the width (half will be applied to each side)
			\param[in] OffsetY_in : the offset to add to the height (half will be applied to each side)
			\return true if the rectangle was modified; false otherwise
		*/
		bool Expand(T OffsetX_in, T OffsetY_in)
		{
			if (OffsetX_in || OffsetY_in)
			{
				m_Point2.Move(OffsetX_in / (T)2, OffsetY_in / (T)2, (T)0);
				m_Point1.Move(-OffsetX_in / (T)2, -OffsetY_in / (T)2, (T)0);

				return true;
			}

			return false;
		}

		/*! \brief Shrinks the rectangle uniformly on each side
			\param[in] OffsetX_in : the offset to remove from the width (half will be applied to each side)
			\param[in] OffsetY_in : the offset to remove from the height (half will be applied to each side)
			\return true if the rectangle was modified; false otherwise
		*/
		bool Shrink(T OffsetX_in, T OffsetY_in)
		{
			if (OffsetX_in || OffsetY_in)
			{
				m_Point2.Move(-OffsetX_in / (T)2, -OffsetY_in / (T)2, (T)0);
				m_Point1.Move(OffsetX_in / (T)2, OffsetY_in / (T)2, (T)0);

				return true;
			}

			return false;
		}

		/*! \brief Resizes the rectangle by moving one of its corner
			\param[in] OffsetX_in : the offset in width
			\param[in] OffsetY_in : the offset in height
			\param[in] Expand_in : flag specifying if the rectangle expands or shrinks (assuming positive offsets)
			\return true if the rectangle changed; false otherwise
		*/
		bool Resize(T OffsetX_in, T OffsetY_in, bool Expand_in = true)
		{
			if (OffsetX_in || OffsetY_in)
			{
				if (Expand_in)
					m_Point2.Move(OffsetX_in, OffsetY_in, (T)0);
				else
					m_Point1.Move(OffsetX_in, OffsetY_in, (T)0);
				// normalize the points so that the first point has the lowest 
				// coordinates and the second one has the highest
				NormalizePoints(m_Point1, m_Point2);

				return true;
			}

			return false;
		}

		/*! \brief Shifts the rectangle by the specified offsets
			\param[in] OffsetX_in : the offset in the x-coordinate
			\param[in] OffsetY_in : the offset in the y-coordinate
			\return true if the rectangle changed; false otherwises
		*/
		bool Move(T OffsetX_in, T OffsetY_in)
		{
			if (OffsetX_in || OffsetY_in)
			{
				m_Point2.Move(OffsetX_in, OffsetY_in, (T)0);
				m_Point1.Move(OffsetX_in, OffsetY_in, (T)0);

				return true;
			}

			return false;
		}

		/*! \brief Retrieves the width of the rectangle
			\return the width of the rectangle
		*/
		T GetWidth() const
		{
			return (m_Point2.GetX() - m_Point1.GetX());
		}

		/*! \brief Retrieves the height of the rectangle
			\return the height of the rectangle
		*/
		T GetHeight() const
		{
			return (m_Point2.GetY() - m_Point1.GetY());
		}

		/*! \brief Sets the width of the rectangle
			\param[in] Width_in : the new width of the rectangle
			\return true if the rectangle changed; false otherwise
		*/
		bool SetWidth(T Width_in)
		{
			if (Width_in != GetWidth())
			{
				m_Point2.SetX(Width_in + m_Point1.GetX());

				return true;
			}			

			return false;
		}

		/*! \brief Sets the height of the rectangle
			\param[in] Height_in : the new height of the rectangle
			\return true if the rectangle changed; false otherwise
		*/
		bool SetHeight(T Height_in)
		{
			if (Height_in != GetHeight())
			{
				m_Point2.SetY(Height_in + m_Point1.GetY());

				return true;
			}			

			return false;
		}

		/*! \brief Sets the size of the rectangle
			\param[in] Width_in : the new width of the rectangle
			\param[in] Height_in : the new height of the rectangle
			\return true if the rectangle changed; false otherwise
		*/
		bool SetSize(T Width_in, T Height_in)
		{
			return (SetWidth(Width_in) && SetHeight(Height_in));
		}

		/*! \brief Retrieves the size of the rectangle
			\param[out] Width_out : receives the width of the rectangle
			\param[out] Height_out : receives the height of the rectangle
		*/
		void GetSize(T& Width_out, T& Height_out)
		{
			Width_out = GetWidth();
			Height_out = GetHeight();
		}

		/*! \brief Checks if the rectangle has a null area
			\return true if the rectangle has a null area; false otherwise
		*/
		bool IsEmpty() { return (m_Point2 == m_Point1); }
		//! \brief Sets the rectangle to have a null area
		void SetEmpty() { m_Point2 = m_Point1; }		

		/*! \brief Checks if this rectangle intersects with the specified rectangle
			\param[in] Rect_in : the rectangle to check
			\return true if the rectangles intersect; false otherwise
		*/
		bool Intersects(const CRect<T,U>& Rect_in) const
		{
			return (m_Point2.GetY() <= Rect_in.m_Point1.GetY() && m_Point1.GetY() >= Rect_in.m_Point2.GetY()
				 && m_Point2.GetX() >= Rect_in.m_Point1.GetX() && m_Point1.GetX() <= Rect_in.m_Point2.GetX());
		}

		/*! \brief Checks if this rectangle contains the specified point
			\param[in] Point_in : the point to check
			\return true if the point is in the rectangle; false otherwise
		*/
		bool Contains(const CPoint<T,U>& Point_in) const
		{
			return (Point_in.GetX() <= m_Point2.GetX() && Point_in.GetY() <= m_Point2.GetY()
				 && Point_in.GetX() >= m_Point1.GetX() && Point_in.GetY() >= m_Point1.GetY());
		}

		/*! \brief Retrieves the center of the rectangle
			\return the center of the rectangle
		*/
		CPoint<T,U> GetCenter() const
		{
			return CPoint<T,U>((m_Point2.GetX() - m_Point1.GetX()) / (T)2,
							   (m_Point2.GetY() - m_Point1.GetY()) / (T)2, (T)0);
		}

		/*! \brief Retrieves the second point
			\return a const reference to the second point
		*/
		const CPoint<T,U>& GetPoint2() const { return m_Point2; }
		/*! \brief Retrieves the first point
			\return a const reference to the first point
		*/
		const CPoint<T,U>& GetPoint() const { return m_Point1; }

		/*! \brief Retrieves the position of the rectangle
			\param[out] X_out : the x-coordinate of the rectangle
			\param[out] Y_out : the y-coordinate of the rectangle
		*/
		void GetPos(T& X_out, T& Y_out)
		{
			X_out = m_Point1.GetX();
			Y_out = m_Point1.GetY();
		}

		/*! \brief Retrieves the x-coordinate of the first point
			\return the x-coordinate of the first point
		*/
		T GetX() const { return m_Point1.GetX(); }
		/*! \brief Retrieves the y-coordinate of the first point
			\return the y-coordinate of the first point
		*/
		T GetY() const { return m_Point1.GetY(); }
		/*! \brief Retrieves the z-coordinate of the first point
			\return the z-coordinate of the first point
		*/
		T GetZ() const { return m_Point1.GetZ(); }

		/*! \brief Retrieves the x-coordinate of the second point
			\return the x-coordinate of the second point
		*/
		T GetX2() const { return m_Point2.GetX(); }
		/*! \brief Retrieves the y-coordinate of the second point
			\return the y-coordinate of the second point
		*/
		T GetY2() const { return m_Point2.GetY(); }
		/*! \brief Retrieves the z-coordinate of the second point
			\return the z-coordinate of the second point
		*/
		T GetZ2() const { return m_Point2.GetZ(); }

		/*! \brief Sets the x-coordinate of the first point
			\param[in] X_in : the new x-coordinate
			\return true if the point changed; false otherwise
		*/
		bool SetX(T X_in)
		{
			T Width = GetWidth();

			return (m_Point1.SetX(X_in) && m_Point2.SetX(X_in + Width));
		}
		/*! \brief Sets the y-coordinate of the first point
			\param[in] X_in : the new y-coordinate
			\return true if the point changed; false otherwise
		*/
		bool SetY(T Y_in)
		{
			T Height = GetHeight();

			return (m_Point1.SetY(Y_in) && m_Point2.SetY(Y_in + Height));
		}
		/*! \brief Sets the z-coordinate of the first point
			\param[in] X_in : the new z-coordinate
			\return true if the point changed; false otherwise
		*/
		bool SetZ(T Z_in) { return m_Point1.SetZ(Z_in); }

		/*! \brief Sets the x-coordinate of the second point
			\param[in] X_in : the new x-coordinate
			\return true if the point changed; false otherwise
		*/
		bool SetX2(T X_in)
		{
			T Width = GetWidth();

			return (m_Point1.SetX(X_in - Width) && m_Point2.SetX(X_in));
		}
		/*! \brief Sets the y-coordinate of the second point
			\param[in] X_in : the new y-coordinate
			\return true if the point changed; false otherwise
		*/
		bool SetY2(T Y_in)
		{
			T Height = GetHeight();

			return (m_Point1.SetY(Y_in - Height) && m_Point2.SetY(Y_in));
		}
		/*! \brief Sets the z-coordinate of the second point
			\param[in] X_in : the new z-coordinate
			\return true if the point changed; false otherwise
		*/
		bool SetZ2(T Z_in) { return m_Point2.SetZ(Z_in); }

	protected:
		/*! \brief Ensures that P1.x < P2.x and P1.y < P2.y
			\param[in] First_in_out : the first point to normalize
			\param[in] Second_in_out : the second point to normalize 
			\return true if the points changed; false otherwise
		*/
		bool NormalizePoints(CPoint<T,U>& First_in_out, CPoint<T,U>& Second_in_out) const
		{
			T X2 = Second_in_out.GetX();
			T Y2 = Second_in_out.GetY();
			T Y1 = First_in_out.GetY();
			T X1 = First_in_out.GetX();
			
			bool Result = false;

			// normalize the x-coordinate
			if (X2 < X1)
			{
				Second_in_out.SetX(X1);
				First_in_out.SetX(X2);
				Result = true;
			}
			// normalize the y-coordinate
			if (Y2 < Y1)
			{
				Second_in_out.SetY(Y1);
				First_in_out.SetY(Y2);
				Result = true;
			}

			return Result;
		}

		//! first corner (lowest coordinates)
		CPoint<T,U> m_Point1;
		//! second corner (highest coordinates)
		CPoint<T,U> m_Point2;
	};
}

#endif//__RECT_3D_H__
