/**************************************************************************
	created		:	2013-05-02
	filename	: 	IEventInterface.h
	author		:	Xebeth`
	copyright	:	North Edge (2013)
	purpose		:	
**************************************************************************/
#ifndef __I_EVENT_INTERFACE_H__
#define __I_EVENT_INTERFACE_H__

namespace Windower
{
	//!	\brief Interface for module that handles user input in 3D view
	class IEventInterface : public NonCopyable
	{
	public:
		IEventInterface() : m_hWnd(NULL) {}
		//!	\brief empty destructor
		virtual ~IEventInterface(void) {}

		//! Keyboard modifier keys
		enum eKeyboardFlag
		{
			SHIFT_KEY_FLAG			= 0x01U,	//!< Shift pressed
			CONTROL_KEY_FLAG		= 0x02U,	//!< Control pressed
			ALT_KEY_FLAG			= 0x04U,	//!< Alt pressed
		};

		/*! \brief Describe the action done by the user input handler and what the manager of user input handler should do
			\see eEventAction, LRESULT
		 */
		enum eEventAction
		{
			EVENT_IGNORED				= 0x00U,	//!< The user input handler has done nothing
			EVENT_PROCESSED				= 0x01U,	//!< The user input handler has done something and the event should not be passed to other handlers
			EVENT_CAPTURE				= 0x02U,	//!< The user input handler needs to capture any further events exclusively (bypasse other handlers)
			EVENT_RELEASE				= 0x03U,	//!< The user input handler does not need to receive the further event (and bypass other handlers)
			EVENT_TERMINATE				= 0x04U,	//!< The user input handler has definitively finish and could be remove, destroy ...

			EVENT_MASK					= 0x07U		//!< Mask that contains all possible events value
		};

		/*!	\brief Function to call when the left mouse button is pressed
			\param[in] Flags_in gives the state of keyboard modifier (SHIFT, CTRL..)
			\param[in] MousePos_in Mouse position
			\returns A value that describe what the user input manager should do
			\see UserInputHandler::Interface::eKeyboardFlag
		 */
		virtual LRESULT OnLButtonDown(WORD X_in, WORD Y_in, DWORD MouseFlags_in, UINT KeyFlags_in) { return EVENT_IGNORED; }

		/*!	\brief Function to call when the left mouse button is released
			\param[in] Flags_in gives the state of keyboard modifier (SHIFT, CTRL..)
			\param[in] MousePos_in Mouse position
			\returns A value that describe what the user input manager should do
			\see UserInputHandler::Interface::eKeyboardFlag
		 */
		virtual LRESULT OnLButtonUp(WORD X_in, WORD Y_in, DWORD MouseFlags_in, UINT KeyFlags_in) { return EVENT_IGNORED; }

		/*!	\brief Function to call when the left mouse button is double clicked
			\param[in] Flags_in gives the state of keyboard modifier (SHIFT, CTRL..)
			\param[in] MousePos_in Mouse position
			\returns A value that describe what the user input manager should do
			\see UserInputHandler::Interface::eKeyboardFlag
		 */
		virtual LRESULT OnLButtonDblClk(WORD X_in, WORD Y_in, DWORD MouseFlags_in, UINT KeyFlags_in) { return EVENT_IGNORED; }

		/*!	\brief Function to call when the right mouse button is pressed
			\param[in] Flags_in gives the state of keyboard modifier (SHIFT, CTRL..)
			\param[in] MousePos_in Mouse position
			\returns A value that describe what the user input manager should do
			\see UserInputHandler::Interface::eKeyboardFlag
		 */
		virtual LRESULT OnRButtonDown(WORD X_in, WORD Y_in, DWORD MouseFlags_in, UINT KeyFlags_in) { return EVENT_IGNORED; }

		/*!	\brief Function to call when the right mouse button is released
			\param[in] Flags_in gives the state of keyboard modifier (SHIFT, CTRL..)
			\param[in] MousePos_in Mouse position
			\returns A value that describe what the user input manager should do
			\see UserInputHandler::Interface::eKeyboardFlag
		 */
		virtual LRESULT OnRButtonUp(WORD X_in, WORD Y_in, DWORD MouseFlags_in, UINT KeyFlags_in) { return EVENT_IGNORED; }

		/*!	\brief Function to call when the mouse move in the 3D view
			\param[in] Flags_in gives the state of keyboard modifier (SHIFT, CTRL..)
			\param[in] MousePos_in Mouse position
			\returns A value that describe what the user input manager should do
			\see UserInputHandler::Interface::eKeyboardFlag
		 */
		virtual LRESULT OnMouseMove(WORD X_in, WORD Y_in, DWORD MouseFlags_in, UINT KeyFlags_in) { return EVENT_IGNORED; }
		/*!	\brief Function to call when the mouse wheel is moved in the 3D view
			\param[in] Flags_in gives the state of keyboard modifier (SHIFT, CTRL..)
			\param[in] Delta_in Value of the wheel movement
			\param[in] MousePos_in Mouse position
			\returns A value that describe what the user input manager should do
			\see UserInputHandler::Interface::eKeyboardFlag
		 */
		virtual LRESULT OnMouseWheel(WORD X_in, WORD Y_in, short zDelta_in, DWORD MouseFlags_in, UINT KeyFlags_in) { return EVENT_IGNORED; }

		/*!	\brief Function called when the mouse leave the view
		 */
		virtual LRESULT OnMouseLeave(void) { return EVENT_IGNORED; }

		/*!	\brief Function to call when a key is pressed
			\param[in] Flags_in gives the state of keyboard modifier (SHIFT, CTRL..)
			\param[in] PressedChar_in Keyboard key pressed
			\param[in] RepetitionCount_in Number of time the key pressed have been sent
			\returns A value that describe what the user input manager should do
			\see UserInputHandler::Interface::eKeyboardFlag
		 */
		virtual LRESULT OnKeyDown(UINT PressedChar_in, UINT RepetitionCount_in, UINT KeyFlags_in) { return EVENT_IGNORED; }

		/*!	\brief Function to call when a key is released
			\param[in] Flags_in gives the state of keyboard modifier (SHIFT, CTRL..)
			\param[in] PressedChar_in Keyboard key released
			\param[in] RepetitionCount_in Number of time the key pressed have been sent
			\returns A value that describe what the user input manager should do
			\see UserInputHandler::Interface::eKeyboardFlag
		 */
		virtual LRESULT OnKeyUp(UINT PressedChar_in, UINT RepetitionCount_in, UINT KeyFlags_in) { return EVENT_IGNORED; }

		/*!	\brief Function called when the user stretch the window
			\param[in] NewWidth_in New window width
			\param[in] NewHeight_in New window width
			\returns A value that describe what the user input manager should do
		 */
		virtual LRESULT OnSize(int NewWidth_in, int NewHeight_in, UINT nFlags_in) { return EVENT_IGNORED; }

		/*!	\brief Function called when the focus in the view is kill
		 */
		virtual LRESULT OnKillFocus(void) { return EVENT_IGNORED; }

		/*!	\brief Function called when the focus in set to the view
		 */
		virtual LRESULT OnSetFocus(void) { return EVENT_IGNORED; }

		/*!	\brief Function called when the focus in (de)activated
		 */
		virtual LRESULT OnActivate(bool bActive_in, bool bMinimized_in) { return EVENT_IGNORED; }

		//!	\brief Member function pointer for mouse events
		typedef LRESULT (IEventInterface::*MouseEventMemFn)(WORD X_in, WORD Y_in, DWORD MouseFlags_in, UINT KeyFlags_in);

		//!	\brief Member function pointer for mouse events
		typedef LRESULT (IEventInterface::*KeyEventMemFn)(UINT PressedChar_in, UINT RepetitionCount_in, UINT KeyFlags_in);

		void SetWnd(HWND hWnd_in) { m_hWnd = hWnd_in; }

	protected:
		HWND m_hWnd;
	};
}

#endif//__I_EVENT_INTERFACE_H__
