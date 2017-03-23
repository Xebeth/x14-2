/**************************************************************************
	created		:	2013-05-02
	filename	: 	EventHandler.cpp
	author		:	Xebeth`
	copyright	:	North Edge (2013)
	purpose		:	
**************************************************************************/
#include "stdafx.h"

#include "IEventInterface.h"
#include "EventHandler.h"

namespace Windower
{
	//!	\brief EventHandler constructor
	EventHandler::EventHandler()
		: m_hWnd(nullptr), m_pCurrentUIHandler(nullptr) {}

	//!	\brief EventHandler constructor
	EventHandler::EventHandler(HWND hWnd_in)
		: m_hWnd(hWnd_in), m_pCurrentUIHandler(nullptr) {}

	//!	\brief EventHandler destructor
	EventHandler::~EventHandler(void)
	{
		m_UserInputHandlerArray.clear();
	}

	/*! \brief The framework calls this member function when the user presses the left mouse button.
		\param[in] MouseFlags_in Indicates whether various virtual keys are down.
		\param[in] MousePos_in Specifies the x- and y-coordinate of the cursor. These coordinates are always relative to the upper-left corner of the window.
	 */
	LRESULT EventHandler::OnLButtonDown(WORD X_in, WORD Y_in, DWORD_PTR MouseFlags_in)
	{
		return DispatchMouseEvent(&IEventInterface::OnLButtonDown, X_in, Y_in, MouseFlags_in);
	}

	/*! \brief The framework calls this member function when the user releases the left mouse button.
		\param[in] MouseFlags_in Indicates whether various virtual keys are down.
		\param[in] MousePos_in Specifies the x- and y-coordinate of the cursor. These coordinates are always relative to the upper-left corner of the window.
	 */
	LRESULT EventHandler::OnLButtonUp(WORD X_in, WORD Y_in, DWORD_PTR MouseFlags_in)
	{
		return DispatchMouseEvent(&IEventInterface::OnLButtonUp, X_in, Y_in, MouseFlags_in);
	}

	/*! \brief The framework calls this member function when the user double-clicks the left mouse button.
		\param[in] MouseFlags_in Indicates whether various virtual keys are down.
		\param[in] MousePos_in Specifies the x- and y-coordinate of the cursor. These coordinates are always relative to the upper-left corner of the window.
	 */
	LRESULT EventHandler::OnLButtonDblClk(WORD X_in, WORD Y_in, DWORD_PTR MouseFlags_in)
	{
		return DispatchMouseEvent(&IEventInterface::OnLButtonDblClk, X_in, Y_in, MouseFlags_in);
	}

	/*! \brief The framework calls this member function when the user presses the right mouse button.
		\param[in] MouseFlags_in Indicates whether various virtual keys are down.
		\param[in] MousePos_in Specifies the x- and y-coordinate of the cursor. These coordinates are always relative to the upper-left corner of the window.
	 */
	LRESULT EventHandler::OnRButtonDown(WORD X_in, WORD Y_in, DWORD_PTR MouseFlags_in)
	{
		return DispatchMouseEvent(&IEventInterface::OnRButtonDown, X_in, Y_in, MouseFlags_in);
	}

	/*! \brief The framework calls this member function when the user releases the right mouse button.
		\param[in] MouseFlags_in Indicates whether various virtual keys are down.
		\param[in] MousePos_in Specifies the x- and y-coordinate of the cursor. These coordinates are always relative to the upper-left corner of the window.
	 */
	LRESULT EventHandler::OnRButtonUp(WORD X_in, WORD Y_in, DWORD_PTR MouseFlags_in)
	{
		return DispatchMouseEvent(&IEventInterface::OnRButtonUp, X_in, Y_in, MouseFlags_in);
	}

	/*! \brief The framework calls this member function when the mouse cursor moves.
		\param[in] MouseFlags_in Indicates whether various virtual keys are down.
		\param[in] MousePos_in Specifies the x- and y-coordinate of the cursor. These coordinates are always relative to the upper-left corner of the window.
	*/
	LRESULT EventHandler::OnMouseMove(WORD X_in, WORD Y_in, DWORD_PTR MouseFlags_in)
	{
		return DispatchMouseEvent(&IEventInterface::OnMouseMove, X_in, Y_in, MouseFlags_in);
	}

	/*! \copydoc IEventInterface::OnMouseLeave(void)
	*/
	LRESULT EventHandler::OnMouseLeave(void)
	{
		LRESULT UIHReturnValue = IEventInterface::EVENT_IGNORED;
		UIHConstIterator UIHIt, UIHEnd = m_UserInputHandlerArray.end();

		// The size event can't be limited to one UIH. Ignore EVENT_CAPTURE
		for( UIHIt = m_UserInputHandlerArray.begin() ; UIHIt != UIHEnd ; ++UIHIt )
		{
			// Call the member function pointer on the current object pointer
			UIHReturnValue |= (*UIHIt)->OnMouseLeave();
		}

		return UIHReturnValue;
	}

	/*!	\brief returns the SHIFT and CONTROL key set using eKeyboardFlag
		\returns state of the Shift, Alt, Control key.
		\see IEventInterface::eKeyboardFlag
	 */
	UINT_PTR EventHandler::GetKeyFlags(void)
	{
		UINT_PTR KeyFlags = 0;

		if ((GetKeyState (VK_LSHIFT) < 0) || (GetKeyState (VK_RSHIFT) < 0))
			KeyFlags |= IEventInterface::SHIFT_KEY_FLAG;

		if ((GetKeyState (VK_LCONTROL) < 0) || (GetKeyState (VK_RCONTROL) < 0))
			KeyFlags |= IEventInterface::CONTROL_KEY_FLAG;

		if ((GetKeyState (VK_LMENU) < 0) || (GetKeyState (VK_RMENU) < 0))
			KeyFlags |= IEventInterface::ALT_KEY_FLAG;

		return KeyFlags;
	}

	/*!	\brief Handle the return value of the call to the UIH when an UIH locked during the last loop
		\param[in] FctReturnVal_in Return value of the function call
		\returns true if the loop should be skipped else false
	 */
	bool EventHandler::ProcessLockedUIHCall(LRESULT FctReturnVal_in)
	{
		switch(FctReturnVal_in & IEventInterface::EVENT_MASK)
		{
		case IEventInterface::EVENT_PROCESSED:		// Event consumed
			return true;
		case IEventInterface::EVENT_CAPTURE:	// Event consumed and need to be used for further events /!\ should not happened: already locked
			// Be sure that all messages will be redirected to the window until the user release the left button
			::SetCapture(m_hWnd);
			::SetFocus(m_hWnd);
			return true;
		case IEventInterface::EVENT_RELEASE:		// Event consumed and release the lock
			::ReleaseCapture();
			return false;
		case IEventInterface::EVENT_TERMINATE:
			TerminateHandler(m_pCurrentUIHandler);
			::ReleaseCapture();
			return true;
		case IEventInterface::EVENT_IGNORED:		// Not handled => Continue the loop
			return false;			
		}

		return false;
	}

	/*!	\brief Handle the return value of the call to the UIH in the main loop
		\param[in] FctReturnVal_in Return value of the function call
		\param[in,out] UIHIt_in_out reference on the current iterator that will be updated
		to the next value to handle delete the current UIH when necessary.
		It will also be used to set m_pCurrentUIHandler when necessary.
		\returns true if the loop should be stopped else false
		\see m_pCurrentUIHandler
	 */
	bool EventHandler::ProcessUIHCall(LRESULT FctReturnVal_in,
										EventHandler::UIHConstIterator & UIHIt_in_out)
	{
		// Call the member function pointer on the current object pointer
		switch(FctReturnVal_in & IEventInterface::EVENT_MASK)
		{
		case IEventInterface::EVENT_PROCESSED:		// Event consumed => stop the loop
			return true;
		case IEventInterface::EVENT_CAPTURE:	// Event consumed and need to be used for further events
			// Be sure that all messages will be redirected to the window until the user release the left button
			::SetCapture(m_hWnd);
			::SetFocus(m_hWnd);
			m_pCurrentUIHandler = *UIHIt_in_out;
			++UIHIt_in_out;
			return true;
		case IEventInterface::EVENT_RELEASE:		// Event consumed and release the lock
			m_pCurrentUIHandler = nullptr;
			::ReleaseCapture();
			++UIHIt_in_out;
			return true;
		case IEventInterface::EVENT_TERMINATE:
			UIHIt_in_out = TerminateHandler(*UIHIt_in_out);
			::ReleaseCapture();
			return true;
		case IEventInterface::EVENT_IGNORED:		// Not handled => Continue the loop
			++UIHIt_in_out;
			return false;
		default:
			++UIHIt_in_out;
			return false;
		}
	}

	/*! \brief The framework calls this member function as a user rotates the mouse wheel and encounters the wheel's next notch.
		\param[in] MouseFlags_in Indicates whether various virtual keys are down.
		\param[in] zDelta_in Indicates distance rotated. The zDelta value is expressed in multiples or divisions of WHEEL_DELTA, which is 120. A value less than zero indicates rotating back (toward the user) while a value greater than zero indicates rotating forward (away from the user). The user can reverse this response by changing the Wheel setting in the mouse software. See the Remarks for more information about this parameter.
		\param[in] MousePos_in Specifies the x- and y-coordinate of the cursor. These coordinates are always relative to the upper-left corner of the window.
	 */
	LRESULT EventHandler::OnMouseWheel(WORD X_in, WORD Y_in, short zDelta_in, DWORD_PTR MouseFlags_in)
	{
		LRESULT UIHReturnValue = IEventInterface::EVENT_IGNORED;
		UINT_PTR KeyFlags = GetKeyFlags();
		bool SkipLoop = false;
		
		// A handler could have asked to get all further events
		// Check if it has been saved
		if(m_pCurrentUIHandler)
		{
			// Call the member function on the saved handler first
			UIHReturnValue = m_pCurrentUIHandler->OnMouseWheel(X_in, Y_in, zDelta_in, MouseFlags_in, KeyFlags);
			SkipLoop = ProcessLockedUIHCall(UIHReturnValue);
		}

		if(SkipLoop == false)
		{
			UIHConstIterator UIHEnd = m_UserInputHandlerArray.end();
			UIHConstIterator UIHIt;
			bool StopLoop = false;

			for( UIHIt = m_UserInputHandlerArray.begin() ; UIHIt != UIHEnd && StopLoop == false;)
			{
				if ( m_pCurrentUIHandler == (*UIHIt) )
				{
					m_pCurrentUIHandler = nullptr;
					++UIHIt;	//Incr pointer as we don't proceed this entry
					continue;	//Do not proceed twice the same handler
				}

				// Call the member function pointer on the current object pointer
				UIHReturnValue = (*UIHIt)->OnMouseWheel(X_in, Y_in, zDelta_in, MouseFlags_in, KeyFlags);
				StopLoop = ProcessUIHCall(UIHReturnValue, UIHIt);
			}
		}

		return UIHReturnValue;
	}

	/*!	\brief Function to call when a key is pressed
		\param[in] nChar_in Specifies the virtual key code of the given key. For a list of of standard virtual key codes, see Winuser.h
		\param[in] nRepCnt_in Repeat count (the number of times the keystroke is repeated as a result of the user holding down the key).
		\param[in] MouseFlags_in Specifies the scan code, key-transition code, previous key state, and context code.
	 */
	LRESULT EventHandler::OnKeyDown(UINT_PTR nChar_in, UINT_PTR nRepCnt_in)
	{
		return DispatchKeyboardEvent(&IEventInterface::OnKeyDown, nChar_in, nRepCnt_in);
	}

	/*!	\brief Function to call when a key is released
		\param[in] nChar_in Specifies the virtual key code of the given key. For a list of of standard virtual key codes, see Winuser.h
		\param[in] nRepCnt_in Repeat count (the number of times the keystroke is repeated as a result of the user holding down the key).
		\param[in] MouseFlags_in Specifies the scan code, key-transition code, previous key state, and context code.
	 */
	LRESULT EventHandler::OnKeyUp(UINT_PTR nChar_in, UINT_PTR nRepCnt_in)
	{
		return DispatchKeyboardEvent(&IEventInterface::OnKeyUp, nChar_in, nRepCnt_in);
	}

	/*!	\brief The framework calls this member function after the window's size has changed.
		\param[in] nType_in Specifies the type of resizing requested.
		\param[in] NewWidth_in Specifies the new width of the client area.
		\param[in] NewHeight_in Specifies the new height of the client area.
	 */
	LRESULT EventHandler::OnSize(int NewWidth_in, int NewHeight_in, UINT nFlags_in)
	{
		LRESULT UIHReturnValue = IEventInterface::EVENT_IGNORED;
		UIHConstIterator UIHIt, UIHEnd = m_UserInputHandlerArray.end();

		// The size event can't be limited to one UIH. Ignore EVENT_CAPTURE
		for( UIHIt = m_UserInputHandlerArray.begin() ; UIHIt != UIHEnd ; ++UIHIt )
		{
			// Call the member function pointer on the current object pointer
			UIHReturnValue |= (*UIHIt)->OnSize(NewWidth_in, NewHeight_in, nFlags_in);
		}

		return UIHReturnValue;
	}

	LRESULT EventHandler::OnPaint()
	{
		LRESULT UIHReturnValue = IEventInterface::EVENT_IGNORED;
		UIHConstIterator UIHIt, UIHEnd = m_UserInputHandlerArray.end();

		// The OnKillFocus event can't be limited to one UIH. So send it to all
		for( UIHIt = m_UserInputHandlerArray.begin() ; UIHIt != UIHEnd ; ++UIHIt )
		{
			// Call the member function pointer on the current object pointer
			UIHReturnValue |= (*UIHIt)->OnPaint();
		}

		return UIHReturnValue;
	}

	/*! \copydoc IEventInterface::OnKillFocus(void)
	*/
	LRESULT EventHandler::OnKillFocus(void)
	{
		LRESULT UIHReturnValue = IEventInterface::EVENT_IGNORED;
		UIHConstIterator UIHIt, UIHEnd = m_UserInputHandlerArray.end();

		// The OnKillFocus event can't be limited to one UIH. So send it to all
		for( UIHIt = m_UserInputHandlerArray.begin() ; UIHIt != UIHEnd ; ++UIHIt )
		{
			// Call the member function pointer on the current object pointer
			UIHReturnValue |= (*UIHIt)->OnKillFocus();
		}

		return UIHReturnValue;
	}

	/*! \copydoc IEventInterface::OnActive(bool)
	*/
	LRESULT EventHandler::OnActivate(bool bActive_in, bool bMinimized_in)
	{
		LRESULT UIHReturnValue = IEventInterface::EVENT_IGNORED;
		UIHConstIterator UIHIt, UIHEnd = m_UserInputHandlerArray.end();

		// The OnKillFocus event can't be limited to one UIH. So send it to all
		for( UIHIt = m_UserInputHandlerArray.begin() ; UIHIt != UIHEnd ; ++UIHIt )
		{
			// Call the member function pointer on the current object pointer
			UIHReturnValue |= (*UIHIt)->OnActivate(bActive_in, bMinimized_in);
		}

		return UIHReturnValue;
	}

	/*! \copydoc IEventInterface::OnSetFocus(void)
	*/
	LRESULT EventHandler::OnSetFocus(void)
	{
		LRESULT UIHReturnValue = IEventInterface::EVENT_IGNORED;
		UIHConstIterator UIHIt, UIHEnd = m_UserInputHandlerArray.end();

		// The OnSetFocus event can't be limited to one UIH. So send it to all
		for( UIHIt = m_UserInputHandlerArray.begin() ; UIHIt != UIHEnd ; ++UIHIt )
		{
			// Call the member function pointer on the current object pointer
			UIHReturnValue |= (*UIHIt)->OnSetFocus();
		}

		return UIHReturnValue;
	}

	/*!	\brief Process the mouse events through the user input handlers
		\param[in] pFn_in Pointer on mouse event member function of IEventInterface
		\param[in] MousePos_in Mouse position
		\see IEventInterface
	 */
	LRESULT EventHandler::DispatchMouseEvent(IEventInterface::MouseEventMemFn pFn_in, WORD X_in, WORD Y_in, DWORD_PTR MouseFlags_in)
	{
		LRESULT UIHReturnValue = IEventInterface::EVENT_IGNORED;
		UINT_PTR KeyFlags = GetKeyFlags();
		bool SkipLoop = false;		

		// A handler could have asked to get all further events
		// Check if it has been saved
		if(m_pCurrentUIHandler)
		{
			// Call the member function on the saved handler first
			UIHReturnValue = (m_pCurrentUIHandler->*(pFn_in))(X_in, Y_in, MouseFlags_in, KeyFlags);
			SkipLoop = ProcessLockedUIHCall(UIHReturnValue);
		}

		if(SkipLoop == false)
		{
			UIHConstIterator UIHEnd = m_UserInputHandlerArray.end();
			UIHConstIterator UIHIt;
			bool StopLoop = false;

			for( UIHIt = m_UserInputHandlerArray.begin() ; UIHIt != UIHEnd && StopLoop == false ; )
			{
				if ( m_pCurrentUIHandler == (*UIHIt) )
				{
					m_pCurrentUIHandler = nullptr;
					++UIHIt;	//Incr pointer as we don't proceed this entry
					continue;	//Do not proceed twice the same handler
				}

				// Call the member function pointer on the current object pointer
				UIHReturnValue = ((*UIHIt)->*(pFn_in))(X_in, Y_in, MouseFlags_in, KeyFlags);
				StopLoop = ProcessUIHCall( UIHReturnValue, UIHIt );
			}
		}

		return UIHReturnValue;
	}

	/*!	\brief Process the keyboard events through the user input handlers
		\param[in] pFn_in Pointer on keyboard event member function of IEventInterface
		\param[in] Key_in Pressed or released key
		\param[in] RepCnt_in Number of time the key pressed have been sent
		\see IEventInterface
	 */
	LRESULT EventHandler::DispatchKeyboardEvent(IEventInterface::KeyEventMemFn pFn_in, UINT_PTR Key_in, UINT_PTR RepCnt_in)
	{
		LRESULT UIHReturnValue = IEventInterface::EVENT_IGNORED;
		UINT_PTR KeyFlags = GetKeyFlags();
		bool SkipLoop = false;

		// A handler could have asked to get all further events
		// Check if it has been saved
		if(m_pCurrentUIHandler)
		{
			// Call the member function on the saved handler first
			UIHReturnValue = (m_pCurrentUIHandler->*(pFn_in))(Key_in, RepCnt_in, KeyFlags);
			SkipLoop = ProcessLockedUIHCall(UIHReturnValue);
		}

		if(SkipLoop == false)
		{
			UIHConstIterator UIHIt;
			bool StopLoop = false;

			// UIHIt must be check against m_UserInputHandlerArray.end() every time as an item could be deleted
			// and it modify the value of m_UserInputHandlerArray.end()
			for( UIHIt = m_UserInputHandlerArray.begin(); UIHIt != m_UserInputHandlerArray.end() && StopLoop == false;)
			{
				if ( m_pCurrentUIHandler == (*UIHIt) )
				{
					m_pCurrentUIHandler = nullptr;
					++UIHIt;	//Incr pointer as we don't proceed this entry
					continue;	//Do not proceed twice the same handler
				}

				// Call the member function pointer on the current object pointer
				UIHReturnValue = ((*UIHIt)->*(pFn_in))(Key_in, RepCnt_in, KeyFlags);
				StopLoop = ProcessUIHCall(UIHReturnValue, UIHIt);
			}
		}

		return UIHReturnValue;
	}

	/*!	\brief Function call when a Handler send EVENT_TERMINATE
		\param[in] pFinishedUIHandler_in_out Pointer on finished UIHandler
		\returns An iterator on the next item or m_UserInputHandlerArray.end()
	 */
	EventHandler::UIHIterator EventHandler::TerminateHandler(IEventInterface *pFinishedUIHandler_in_out)
	{
		if(m_pCurrentUIHandler == pFinishedUIHandler_in_out)
			m_pCurrentUIHandler = nullptr;

		delete pFinishedUIHandler_in_out;

		// Remove the UIH pointer in the array
		UIHIterator ItToDelete = std::find( m_UserInputHandlerArray.begin(), m_UserInputHandlerArray.end(), pFinishedUIHandler_in_out );

		if( ItToDelete != m_UserInputHandlerArray.end() )
			return m_UserInputHandlerArray.erase(ItToDelete); // Should always be the case

		return m_UserInputHandlerArray.end();
	}

	void EventHandler::SetWnd(HWND hWnd_in)
	{
		 m_hWnd = hWnd_in;
	}
}