/***********************************************************************/ 
/* Copyright (C) 2002 Definitive Solutions, Inc.  All Rights Reserved. */ 
/* THIS COMPUTER PROGRAM IS PROPRIETARY AND CONFIDENTIAL TO DEFINITIVE */ 
/* SOLUTIONS, INC. AND ITS LICENSORS AND CONTAINS TRADE SECRETS OF     */ 
/* DEFINITIVE SOLUTIONS, INC. THAT ARE PROVIDED PURSUANT TO A WRITTEN  */ 
/* AGREEMENT CONTAINING RESTRICTIONS ON USE AND DISCLOSURE.  ANY USE,  */ 
/* REPRODUCTION, OR TRANSFER EXCEPT AS PROVIDED IN SUCH AGREEMENT      */ 
/* IS STRICTLY PROHIBITED.                                             */ 
/***********************************************************************/ 

#include "stdafx.h"
#include "MyListBox.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


/////////////////////////////////////////////////////////////////////////////
// MyListBox

// Constructor.
MyListBox::MyListBox()
{
}

// Destructor.
/* virtual */ MyListBox::~MyListBox()
{
}

BEGIN_MESSAGE_MAP(MyListBox, CListBox)
	//{{AFX_MSG_MAP(MyListBox)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// MyListBox message handlers

//
void MyListBox::DrawItem(LPDRAWITEMSTRUCT lpDIS)
{
	ASSERT_POINTER(lpDIS, DRAWITEMSTRUCT);

	if (lpDIS  &&  0 <= lpDIS->itemID) {
		CDC* pDC = CDC::FromHandle(lpDIS->hDC);
		ASSERT_VALID(pDC);

		if (pDC) {

			// If item has been selected, draw the highlight rectangle using the
			// item's color.
			if ((lpDIS->itemState & ODS_SELECTED)  &&
			  (lpDIS->itemAction & (ODA_SELECT | ODA_DRAWENTIRE))) {

				CBrush br(lpDIS->itemData ? lpDIS->itemData :
				  ::GetSysColor(COLOR_HIGHLIGHT));
				pDC->FillRect(&lpDIS->rcItem, &br);
			}

			// If item has been deselected, draw the rectangle using the window 
			// color.
			if (! (lpDIS->itemState & ODS_SELECTED)  &&
			  (lpDIS->itemAction & ODA_SELECT)) {

				CBrush br(::GetSysColor(COLOR_WINDOW));
				pDC->FillRect(&lpDIS->rcItem, &br);
			}

			// If item has focus, draw the focus rect.
			if ((lpDIS->itemAction & ODA_FOCUS)  && 
			  (lpDIS->itemState & ODS_FOCUS)) {
				pDC->DrawFocusRect(&lpDIS->rcItem);
			}

			// If item does not have focus, redraw (erase) the focus rect.
			if ((lpDIS->itemAction & ODA_FOCUS)  &&
			  ! (lpDIS->itemState & ODS_FOCUS)) {

				pDC->DrawFocusRect(&lpDIS->rcItem);
			}

			// Set the background mode to TRANSPARENT to draw the text.
			int nBkMode(pDC->SetBkMode(TRANSPARENT));

			// If the item's color information is set, use the highlight color
			// gray text color, or normal color for the text.
			COLORREF crText(NULL);

			if (lpDIS->itemData) {

				if (lpDIS->itemState & ODS_SELECTED) {

					COLORREF crHilite(RGB(
					  255 - GetRValue(lpDIS->itemData),
					  255 - GetGValue(lpDIS->itemData),
					  255 - GetBValue(lpDIS->itemData)));

					crText = pDC->SetTextColor(crHilite);
				}
				else if (lpDIS->itemState & ODS_DISABLED) {
					crText = pDC->SetTextColor(::GetSysColor(COLOR_GRAYTEXT));
				}
				else {
					crText = pDC->SetTextColor(lpDIS->itemData);
				}
			}
			else {
				// Else the item's color information is not set, so use the 
				// system colors for the text.
				if (lpDIS->itemState & ODS_SELECTED) {
					crText = pDC->SetTextColor(::GetSysColor(COLOR_HIGHLIGHTTEXT));
				}
				else if (lpDIS->itemState & ODS_DISABLED) {
					crText = pDC->SetTextColor(::GetSysColor(COLOR_GRAYTEXT));
				}
				else {
					crText = pDC->SetTextColor(::GetSysColor(COLOR_WINDOWTEXT));
				}
			}

			// Get and display item text.
			CString sText;
			GetText(lpDIS->itemID, sText);
			CRect rc(lpDIS->rcItem);

			// Setup the text format.
			UINT uiFormat(DT_LEFT | DT_SINGLELINE | DT_VCENTER);

			if (GetStyle() & LBS_USETABSTOPS) {
				uiFormat |= DT_EXPANDTABS;
			}
			
			// Calculate the rectangle size before drawing the text.
			pDC->DrawText(sText, -1, &rc, uiFormat | DT_CALCRECT);
			pDC->DrawText(sText, -1, &rc, uiFormat);
			pDC->SetTextColor(crText); 
			pDC->SetBkMode(nBkMode);
		}
	}
}

//
void MyListBox::MeasureItem(LPMEASUREITEMSTRUCT lpMIS)
{
	lpMIS->itemHeight = ::GetSystemMetrics(SM_CYMENUCHECK);
}

//
int MyListBox::AddString(const CString& sItem, COLORREF cr)
{
	int nItem(CListBox::AddString(sItem));

	if (0 <= nItem) {
		SetItemData(nItem, cr);
	}

	return nItem;
}

//
int MyListBox::InsertString(int nItem, const CString& sItem, COLORREF cr)
{
	int nItemNew(CListBox::InsertString(nItem, sItem));

	if (0 <= nItem) {
		SetItemData(nItemNew, cr);
	}

	return nItemNew;
}

//
void MyListBox::SetItemColor(int nItem, COLORREF cr)
{
	SetItemData(nItem, cr);	
	RedrawWindow();
}

//
void MyListBox::SetItemsColor(COLORREF cr)
{
	for (int nItem = 0; nItem < GetCount(); ++nItem) {
		SetItemColor(nItem, cr);
	}
}

//
/* virtual */ int MyListBox::CompareItem(
										LPCOMPAREITEMSTRUCT lpCompareItemStruct)
{
	int nReturn(0);

	if (-1 < lpCompareItemStruct->itemID1  &&  -1 < lpCompareItemStruct->itemID2) {
		CString sItem1;
		GetText(lpCompareItemStruct->itemID1, sItem1);

		CString sItem2;
		GetText(lpCompareItemStruct->itemID2, sItem2);

		nReturn = sItem1.Compare(sItem2);
	}

	return nReturn;
}
