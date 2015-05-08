// ComicUpdaterDlg.h : header file
//

#if !defined(AFX_COMICUPDATERDLG_H__64F295D1_8CCD_4EA3_8DB0_110929B05701__INCLUDED_)
#define AFX_COMICUPDATERDLG_H__64F295D1_8CCD_4EA3_8DB0_110929B05701__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "HTTPGet.h"

/////////////////////////////////////////////////////////////////////////////
// CComicUpdaterDlg dialog

class CComicUpdaterDlg : public CDialog
{
// Construction
public:
	CComicUpdaterDlg(CWnd* pParent = NULL);	// standard constructor
	
// Dialog Data
	//{{AFX_DATA(CComicUpdaterDlg)
	enum { IDD = IDD_COMICUPDATER_DIALOG };
	CEdit	m_Messages;
	CButton	m_ok;
	CButton	m_cancel;
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CComicUpdaterDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	HICON m_hIcon;

	void UpdateIni(void);
	void UpdateProg(void);
	void GetFile(std::string FileName);


	// Generated message map functions
	//{{AFX_MSG(CComicUpdaterDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_COMICUPDATERDLG_H__64F295D1_8CCD_4EA3_8DB0_110929B05701__INCLUDED_)
