/**************************************************************************
	created		:	2013-05-02
	filename	: 	EventHandler.h
	author		:	Xebeth`
	copyright	:	North Edge (2013)
	purpose		:	
**************************************************************************/
#ifndef __EVENTHANDLER_H__
#define __EVENTHANDLER_H__

namespace Windower
{
	class IEventInterface;

	class EventHandler
	{
	public:
		EventHandler();
		explicit EventHandler(HWND hWnd_in);		
		virtual ~EventHandler(void);

		/* User presses left mouse button. */
		virtual LRESULT OnLButtonDown(WORD X_in, WORD Y_in, DWORD MouseFlags_in);
		/* User releases left mouse button. */
		virtual LRESULT OnLButtonUp(WORD X_in, WORD Y_in, DWORD MouseFlags_in);
		/* User double-clicks left mouse button. */
		virtual LRESULT OnLButtonDblClk(WORD X_in, WORD Y_in, DWORD MouseFlags_in);
		/* User presses right mouse button. */
		virtual LRESULT OnRButtonDown(WORD X_in, WORD Y_in, DWORD MouseFlags_in);
		/* User releases right mouse button. */
		virtual LRESULT OnRButtonUp(WORD X_in, WORD Y_in, DWORD MouseFlags_in);
		/* Mouse moves. */
		virtual LRESULT OnMouseMove(WORD X_in, WORD Y_in, DWORD MouseFlags_in);
		/* Mouse wheels. */
		virtual LRESULT OnMouseWheel(WORD X_in, WORD Y_in, short zDelta_in, DWORD MouseFlags_in);
		/* Mouse leaves. */
		virtual LRESULT OnMouseLeave(void);
		/*	Function called when a key is pressed. */
		virtual LRESULT OnKeyDown(UINT PressedChar_in, UINT RepetitionCount_in);
		/*	Function called when a key is released. */
		virtual LRESULT OnKeyUp(UINT PressedChar_in, UINT RepetitionCount_in);
		/* Window resizes. */
		virtual LRESULT OnSize(int NewWidth_in, int NewHeight_in, UINT nFlags_in);
		/* Window focus get killed. */
		virtual LRESULT OnKillFocus(void);
		/* Window gets focus. */
		virtual LRESULT OnSetFocus(void);
		/* Window is (de)activate */
		virtual LRESULT OnActivate(bool bActive_in, bool bMinimized_in);

		/*!	\brief Add the specified UIH at the end of the UIH array
			\param[in] pNewUIH_in_out Handler to add into inside handler array.
			\remark The pointer will be deleted in the destructor of the controller
			\see m_UserInputHandlerArray
		 */
		void AddUIH(IEventInterface *pNewUIH_in_out)
		{
			if (pNewUIH_in_out != NULL)
			{
				m_UserInputHandlerArray.push_back(pNewUIH_in_out);
				pNewUIH_in_out->SetWnd(m_hWnd);
			}
		}

		void SetWnd(HWND hWnd_in);

		//! Array of user input handler.
		typedef std::vector<IEventInterface *> UIHVector;
		//! Iterator on user input handler arrays.
		typedef std::vector<IEventInterface *>::iterator UIHIterator;
		//! Const iterator on user input handler arrays.
		typedef std::vector<IEventInterface *>::const_iterator UIHConstIterator;
	protected:

		UINT GetKeyFlags(void);
		bool ProcessLockedUIHCall(LRESULT FctReturnVal_in);
		bool ProcessUIHCall(LRESULT FctReturnVal_in, EventHandler::UIHConstIterator & UIHIt_in_out);

		virtual LRESULT DispatchMouseEvent(IEventInterface::MouseEventMemFn pFn_in, WORD X_in, WORD Y_in, DWORD MouseFlags_in);
		virtual LRESULT DispatchKeyboardEvent(IEventInterface::KeyEventMemFn pFn_in, UINT Key_in, UINT RepCnt_in);

		virtual UIHIterator TerminateHandler(IEventInterface *pFinishedUIHandler);
		
		/*!	\brief Array of user input handlers used to handle the operation in the 3D view
			The order in the array determines the priority of the handlers. The event will be processed by the first one, then by the second one, ...
		 */
		UIHVector m_UserInputHandlerArray;
		IEventInterface *m_pCurrentUIHandler;	//!< Pointer on the User input handler that has required to keep all the messages
		HWND m_hWnd;
	};
}

#endif//__EVENTHANDLER_H__
