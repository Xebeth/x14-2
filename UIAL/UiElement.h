/**************************************************************************
	created		:	2013-02-28
	filename	: 	UiElement.h
	author		:	Xebeth`
	copyright	:	North Edge (2013)
	purpose		:	UI Element base class
**************************************************************************/
#ifndef __UI_ELEMENT_H__
#define __UI_ELEMENT_H__

#ifndef _UIAL_SDK_INCLUDE_
	#error Please include the global header file 'UIAL.h'
#endif

namespace UIAL
{
	//! \brief UI Element base class
	template <typename T = long, typename U = FRHCS> class CUiElement
	{
	public:
		typedef CPoint<T,U> CUiPoint;
		typedef CRect<T,U> CUiRect;

		virtual ~CUiElement() {}
		/*! \brief UiElement constructor
			\param[in] ID_in : the ID of the element
		*/
		explicit CUiElement(unsigned long ID_in)
			: m_ID(ID_in), m_Dirty(true), m_Visible(true) {}

		/*! \brief UiElement constructor
			\param[in] ID_in : the ID of the element
			\param[in] Name_in : the name of the element
			\param[in] X_in : the x-coordinate of the element from the top-left corner of the screen
			\param[in] Y_in : the y-coordinate of the element from the top-left corner of the screen
			\param[in] W_in : the width of the element
			\param[in] H_in : the height of the element
			\param[in] Visible_in : flag specifying if the element is visible
		*/
		CUiElement(unsigned long ID_in, const string_t& Name_in, T X_in, T Y_in, T W_in, T H_in, bool Visible_in = true)
			: m_ID(ID_in), m_Rect(X_in, Y_in, X_in + W_in, Y_in + H_in), m_Name(Name_in), m_Dirty(true), m_Visible(true) {}

		/*! \brief UiElement constructor
			\param[in] ID_in : the ID of the element
			\param[in] Name_in : the name of the element
			\param[in] Pos_in : the position of the element
			\param[in] W_in : the width of the element
			\param[in] H_in : the height of the element
			\param[in] Visible_in : flag specifying if the element is visible
		*/
		CUiElement(unsigned long ID_in, const string_t& Name_in, const CUiPoint& Pos_in, T W_in, T H_in, bool Visible_in = true)
			: m_ID(ID_in), m_Rect(Pos_in, W_in, H_in), m_Name(Name_in), m_Dirty(true), m_Visible(Visible_in) {}

		/*! \brief UiElement constructor
			\param[in] ID_in : the ID of the element
			\param[in] Name_in : the name of the element
			\param[in] TopLeft_in : the position of the element
			\param[in] BottomRight_in : the position of the element
			\param[in] Visible_in : flag specifying if the element is visible
		*/
		CUiElement(unsigned long ID_in, const string_t& Name_in, const CUiPoint& TopLeft_in, const CUiPoint& BottomRight_in, bool Visible_in = true)
			: m_ID(ID_in), m_Rect(TopLeft_in, BottomRight_in), m_Name(Name_in), m_Dirty(true), m_Visible(Visible_in) {}

		/*! \brief UiElement constructor
			\param[in] ID_in : the ID of the element
			\param[in] Name_in : the name of the element
			\param[in] Rect_in : the area of the element
			\param[in] Visible_in : flag specifying if the element is visible
		*/
		CUiElement(unsigned long ID_in, const string_t& Name_in, const CUiRect& Rect_in, bool Visible_in = true)
			: m_ID(ID_in), m_Rect(Rect_in), m_Name(Name_in), m_Dirty(true), m_Visible(Visible_in) {}

		/*! \brief Retrieves the size of the element
			\param[out] Width_out : receives the width of the element
			\param[out] Height_out : receives the height of the element 
		*/
		virtual void GetSize(unsigned long &Width_out, unsigned long &Height_out) const
		{
			Height_out = m_Rect.GetHeight();
			Width_out = m_Rect.GetWidth();
		}

		/*! \brief Sets the size of the element
			\param[in] Width_in : the new width of the element
			\param[in] Height_in : the new height of the element
		*/
		virtual void SetSize(unsigned long Width_in, unsigned long Height_in)
		{
			// update the size
			SetDirty(m_Rect.SetSize(Width_in, Height_in));
		}

		/*! \brief Sets the height of the element
			\param[in] Height_in : the new height of the element
		*/
		virtual void SetHeight(unsigned long Height_in)
		{
			// update the height
			SetDirty(m_Rect.SetHeight(Height_in));
		}

		/*! \brief Sets the width of the element
			\param[in] Width_in : the new width of the element
		*/
		virtual void SetWidth(unsigned long Width_in)
		{
			// update the width
			SetDirty(m_Rect.SetWidth(Width_in));
		}

		/*! \brief Retrieves the height of the element
			\return the height of the element
		*/
		virtual unsigned long GetHeight() const { return m_Rect.GetHeight(); }

		/*! \brief Retrieves the width of the element
			\return the width of the element
		*/
		virtual unsigned long GetWidth() const { return m_Rect.GetWidth(); }

		/*! \brief Sets the name of the element
			\param[in] Name_in : the new name of the element
		*/
		virtual void SetName(const string_t &Name_in)
		{
			// update the name
			m_Name = Name_in;
		}

		/*! \brief Retrieves the name of the element
			\returns the name of the element
		*/
		virtual const string_t& GetName() const { return m_Name; }

		/*! \brief Retrieves the ID of the element
			\return the ID of the element
		*/
		virtual unsigned long GetID() const { return m_ID; }
	
		/*! \brief Checks if the element is dirty
			\returns true if the element is dirty; false otherwise
		*/
		virtual bool IsDirty() const { return m_Dirty; }

		/*! \brief Sets the state of the dirty flag
			\param[in] Dirty_in : the new state of the dirty flag
		*/
		virtual void SetDirty(bool Dirty_in) { m_Dirty = Dirty_in; }

		/*! \brief Changes the size of the element by the specified offsets
			\param[in] WidthOffset_in : offset in width
			\param[in] HeightOffset_in : offset in height
		*/
		virtual void Resize(T WidthOffset_in, T HeightOffset_in)
		{
			if (WidthOffset_in != 0L || HeightOffset_in != 0L)
			{
				SetDirty(m_Rect.Resize(WidthOffset_in, HeightOffset_in));
			}
		}

		/*! \brief Retrieves the position of the element
			\param[out] X_out : the x-coordinate from the top-left corner of the screen of the element
			\param[out] Y_out : the y-coordinate from the top-left corner of the screen of the element
		*/
		virtual void GetPos(T &X_out, T &Y_out) const
		{
			X_out = m_Rect.GetX();
			Y_out = m_Rect.GetY();
		}

		/*! \brief Sets the position of the element
			\param[in] X_in : the new x-coordinate of the element from the top-left corner of the screen
			\param[in] Y_in : the new y-coordinate of the element from the top-left corner of the screen
		*/
		virtual void SetPos(T X_in, T Y_in)
		{
			// update the position but keep the z-order
			bool Dirty = m_Rect.SetX(X_in);
			Dirty |= m_Rect.SetY(Y_in);
			SetDirty(Dirty);
		}

		/*! \brief Sets the x-coordinate of the element
			\param[in] X_in : the new x-coordinate of the element from the top-left corner of the screen
		*/
		virtual void SetX(T X_in)
		{
			// update the x-coordinate
			SetDirty(m_Rect.SetX(X_in));
		}

		/*! \brief Sets the y-coordinate of the element
			\param[in] Y_in : the new y-coordinate of the element from the top-left corner of the screen
		*/
		virtual void SetY(T Y_in)
		{
			// update the y-coordinate
			SetDirty(m_Rect.SetY(Y_in));		
		}

		/*! \brief Sets the z-order of the element
			\param[in] ZOrder_in : the new z-order of the element
		*/
		virtual void SetZOrder(T ZOrder_in)
		{
			// update the z-order
			SetDirty(m_Rect.SetZ(ZOrder_in));
		}

		/*! \brief Retrieves the x-coordinate of the element
			\return the x-coordinate from the top-left corner of the screen of the element
		*/
		virtual T GetX() const { return m_Rect.GetX(); }

		/*! \brief Retrieves the y-coordinate of the element
			\return the y-coordinate from the top-left corner of the screen of the element
		*/
		virtual T GetY() const { return m_Rect.GetY(); }

		/*! \brief Retrieves the z-order of the element
			\return the z-order of the element
		*/
		virtual T GetZOrder() const { return m_Rect.GetZ(); }

		/*! \brief Moves the element by specified offsets
			\param[in] XOffset_in : the offset in x-coordinate from the top-left corner of the screen
			\param[in] YOffset_in : the offset in y-coordinate from the top-left corner of the screen
		*/
		virtual void Move(T XOffset_in, T YOffset_in)
		{
			// move the element but keep the z-order
			SetDirty(m_Rect.Move(XOffset_in, YOffset_in));
		}

		/*! \brief Sets the flag specifying if the element is visible
			\param[in] Visible_in : the flag specifying if the element is visible
		*/
		virtual void SetVisibile(bool Visible_in)
		{
			m_Visible = Visible_in;
			SetDirty(Visible_in);
		}

		/*! \brief Toggles the flag specifying if the element is visible
			\return the new value of the flag
		*/
		virtual bool ToggleVisible()
		{
			m_Visible = !m_Visible;
			SetDirty(true);

			return m_Visible;
		}

		/*! \brief Checks if the element is visible
			\return true if the element is visible; false otherwise
		*/
		virtual bool IsVisible() const { return m_Visible; }

		/*! \brief Checks if the coordinates are inside the element rectangle
			\param[in] Rect_in : the rectangle to check
			\param[in] X_in : the x-coordinate to check
			\param[in] Y_in : the y-coordinate to check
			\return true if the coordinates are inside the element rectangle; false otherwise
		*/
		virtual bool IsInsideRect(const CUiRect &Rect_in, T X_in, T Y_in) const
		{
			CUiPoint Point(X_in, Y_in);

			return IsInsideRect(Rect_in, Point);
		}

		/*! \brief Checks if the specified point is inside the element rectangle
			\param[in] Rect_in : the rectangle to check
			\param[in] Point_in : the point to check
			\return true if the point is inside inside the element rectangle; false otherwise
		*/
		virtual bool IsInsideRect(const CUiRect &Rect_in, const CUiPoint &Point_in) const
		{
			return Rect_in.Contains(Point_in);
		}

	protected:
		/*! \brief Sets the ID of the element
			\param[in] ID_in : the new ID of the element
		*/
		void SetID(unsigned long ID_in) { m_ID = ID_in; }

		//! Area of the element
		CUiRect m_Rect;
		//! Name of the element
		string_t m_Name;
		//! Unique identifier
		unsigned long m_ID;
		//! Flag specifying if the element is dirty
		bool m_Dirty;
		//! Flag specifying if the element is visible
		bool m_Visible;
	};
}

#endif//__UI_ELEMENT_H__
