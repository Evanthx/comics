// ComicSetupDlg.h : header file
//

#if !defined(AFX_COMICSETUPDLG_H__F319CED6_3229_4108_8735_D3978D23468F__INCLUDED_)
#define AFX_COMICSETUPDLG_H__F319CED6_3229_4108_8735_D3978D23468F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "SetupIniReader.h"

/////////////////////////////////////////////////////////////////////////////
// CComicSetupDlg dialog

class CComicSetupDlg : public CDialog
{
// Construction
public:
	CComicSetupDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	//{{AFX_DATA(CComicSetupDlg)
	enum { IDD = IDD_COMICSETUP_DIALOG };
	CDragListBox	m_SelectedComics;
	CListBox	m_AllComics;
	CString	m_HelpText;
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CComicSetupDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	//{{AFX_MSG(CComicSetupDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnUnSelect();
	afx_msg void OnSelect();
	virtual void OnOK();
	afx_msg void OnOptionsSelectall();
	afx_msg void OnOptionsUnselectall();
	afx_msg int OnVKeyToItem(UINT nKey, CListBox* pListBox, UINT nIndex);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

	SetupIniFile IniFile;	

	int ArraySize;
	void WriteComicFiles(void);


public:
	afx_msg void OnBnClickedSchedule();
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_COMICSETUPDLG_H__F319CED6_3229_4108_8735_D3978D23468F__INCLUDED_)
