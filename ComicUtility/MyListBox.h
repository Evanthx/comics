/***********************************************************************/ 
/* Copyright (C) 2002 Definitive Solutions, Inc.  All Rights Reserved. */ 
/* THIS COMPUTER PROGRAM IS PROPRIETARY AND CONFIDENTIAL TO DEFINITIVE */ 
/* SOLUTIONS, INC. AND ITS LICENSORS AND CONTAINS TRADE SECRETS OF     */ 
/* DEFINITIVE SOLUTIONS, INC. THAT ARE PROVIDED PURSUANT TO A WRITTEN  */ 
/* AGREEMENT CONTAINING RESTRICTIONS ON USE AND DISCLOSURE.  ANY USE,  */ 
/* REPRODUCTION, OR TRANSFER EXCEPT AS PROVIDED IN SUCH AGREEMENT      */ 
/* IS STRICTLY PROHIBITED.                                             */ 
/***********************************************************************/ 

#if !defined(MYLISTBOXH_563580F0_8724_11d3_9865_00508B025F16)
#define MYLISTBOXH_563580F0_8724_11d3_9865_00508B025F16
#pragma once


/////////////////////////////////////////////////////////////////////////////
// MyListBox

// Declare the class depending on this file is in an EXE project or a DLL one.
class 
	#ifdef _WINDLL
	AFX_EXT_CLASS
	#endif
MyListBox : public CListBox
{
// Construction.
public:
	MyListBox();
	virtual ~MyListBox();

// Operations.
public:
	int	AddString(const CString& sItem, COLORREF cr);
	int	InsertString(int nIndex, const CString& sItem, COLORREF cr);
	void	SetItemColor(int nIndex, COLORREF cr);
	void	SetItemsColor(COLORREF cr);

	// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(MyListBox)
	public:
	virtual void DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct);
	virtual void MeasureItem(LPMEASUREITEMSTRUCT lpMeasureItemStruct);
	//}}AFX_VIRTUAL

// Overrides.
public:
	virtual int	CompareItem(LPCOMPAREITEMSTRUCT lpCompareItemStruct);


// Generated message map functions
protected:
	//{{AFX_MSG(MyListBox)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(MYLISTBOXH_563580F0_8724_11d3_9865_00508B025F16)
