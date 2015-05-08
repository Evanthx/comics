// WinComics.h : main header file for the WINCOMICS application
//

#if !defined(AFX_WINCOMICS_H__5D90477E_389A_4B2F_9EEE_70078691B542__INCLUDED_)
#define AFX_WINCOMICS_H__5D90477E_389A_4B2F_9EEE_70078691B542__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols

/////////////////////////////////////////////////////////////////////////////
// CWinComicsApp:
// See WinComics.cpp for the implementation of this class
//

class CWinComicsApp : public CWinApp
{
public:
	CWinComicsApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CWinComicsApp)
	public:
	virtual BOOL InitInstance();
	//}}AFX_VIRTUAL

// Implementation

	//{{AFX_MSG(CWinComicsApp)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_WINCOMICS_H__5D90477E_389A_4B2F_9EEE_70078691B542__INCLUDED_)
