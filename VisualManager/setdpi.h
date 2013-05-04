/****************************************************

   Set dialog DPI helper class

   Use this class to make your dialog-editor based 
   dialogs look the same under any Windows DPI 
   settings (Large font mode).
   
   This is important if your dialogs have bitmaps on 
   them. You can specify current DPI if you want 
   bitmaps to get resized to current DPI (normally
   they don't get resized).

   Copyright (C)2003 by George Yohng 
   http://www.yohng.com

   LICENSE AGREEMENT:

	  You agree to go to http://www.yohng.com/music.html
	  and listen completely (without skips) to the very
	  first track on the list.

 ****************************************************/

/*  
	Source: http://www.codeproject.com/Articles/5451/Fight-the-dialog-units-DPI-and-Large-Fonts

	*************************************************
	* Warning: for proper sizing with large fonts,  *
	*          set dialog font explicitly to        *
	*                                               *
	*          MS Shell Dlg 2						*
	*************************************************

   // -------------------------------------------------------------------
   // MFC Example:
   // -------------------------------------------------------------------

   ...

   BOOL CMyDlg::OnInitDialog()
   {
	   CDialog::OnInitDialog();

	   dpi.Attach(AfxFindResourceHandle(IMAKEINTRESOURCE(IDD), RT_DIALOG),
				  m_hWnd,IDD,96.0); // 96 is the DPI

	   // The rest of your initialization code goes here

	   return TRUE;
   }

   // -------------------------------------------------------------------
   // ATL/WTL Example:
   // -------------------------------------------------------------------

   ...

   BOOL CMyDlg::OnInitDialog()
   {
	   CDialog::OnInitDialog();

	   dpi.Attach(_AtlBaseModule.GetResourceInstance(),m_hWnd,IDD,96.0);
	   //                                                         ^^^^ DPI

	   // The rest of your initialization code goes here

	   return TRUE;
   }

*/
#ifndef _VISUAL_MANAGER_SDK_INCLUDE_
	#error Please include the global header file 'VisualManager.h'
#endif

#ifndef SETDPI_INCLUDED
#define SETDPI_INCLUDED

class APPVIMGR_API CSetDPI
{
public:
	int IDD;
	HINSTANCE inst;
	HWND hwnd;
	HFONT font,oldfont;

	CSetDPI();

	void Attach(HINSTANCE inst,HWND dlg,int IDD,double dpi, bool relative);

	void Detach();

	~CSetDPI();
};

#endif
