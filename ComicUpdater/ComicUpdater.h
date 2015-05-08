// ComicUpdater.h : main header file for the COMICUPDATER application
//

#if !defined(AFX_COMICUPDATER_H__20222460_60DD_42E0_B828_53D93B7BFACF__INCLUDED_)
#define AFX_COMICUPDATER_H__20222460_60DD_42E0_B828_53D93B7BFACF__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols

/////////////////////////////////////////////////////////////////////////////
// CComicUpdaterApp:
// See ComicUpdater.cpp for the implementation of this class
//

class CComicUpdaterApp : public CWinApp
{
public:
	CComicUpdaterApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CComicUpdaterApp)
	public:
	virtual BOOL InitInstance();
	//}}AFX_VIRTUAL

// Implementation

	//{{AFX_MSG(CComicUpdaterApp)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_COMICUPDATER_H__20222460_60DD_42E0_B828_53D93B7BFACF__INCLUDED_)
