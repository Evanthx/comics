// ComicSetup.h : main header file for the COMICSETUP application
//

#if !defined(AFX_COMICSETUP_H__086C2BDF_E0C6_4133_8BE3_875E06ED2CFA__INCLUDED_)
#define AFX_COMICSETUP_H__086C2BDF_E0C6_4133_8BE3_875E06ED2CFA__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols

/////////////////////////////////////////////////////////////////////////////
// CComicSetupApp:
// See ComicSetup.cpp for the implementation of this class
//

class CComicSetupApp : public CWinApp
{
public:
	CComicSetupApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CComicSetupApp)
	public:
	virtual BOOL InitInstance();
	//}}AFX_VIRTUAL

// Implementation

	//{{AFX_MSG(CComicSetupApp)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_COMICSETUP_H__086C2BDF_E0C6_4133_8BE3_875E06ED2CFA__INCLUDED_)
