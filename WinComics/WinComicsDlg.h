// WinComicsDlg.h : header file
//

#if !defined(AFX_WINCOMICSDLG_H__466AABE8_B6DE_45C5_949F_50A4BE7FAD43__INCLUDED_)
#define AFX_WINCOMICSDLG_H__466AABE8_B6DE_45C5_949F_50A4BE7FAD43__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

/////////////////////////////////////////////////////////////////////////////
// CWinComicsDlg dialog

class CWinComicsDlg : public CDialog
{
// Construction
public:
	CWinComicsDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	//{{AFX_DATA(CWinComicsDlg)
	enum { IDD = IDD_WINCOMICS_DIALOG };
	CListBox	m_StatusList;
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CWinComicsDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	//{{AFX_MSG(CWinComicsDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_WINCOMICSDLG_H__466AABE8_B6DE_45C5_949F_50A4BE7FAD43__INCLUDED_)
