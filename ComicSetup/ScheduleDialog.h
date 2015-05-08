#pragma once
#include "afxdtctl.h"
#include "afxwin.h"


// ScheduleDialog dialog

class ScheduleDialog : public CDialog
{
	DECLARE_DYNAMIC(ScheduleDialog)

public:
	ScheduleDialog(CWnd* pParent = NULL);   // standard constructor
	virtual ~ScheduleDialog();

	bool ScheduleTheTask(void);

// Dialog Data
	enum { IDD = IDD_SCHEDULE_DIALOG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedOk();
	CDateTimeCtrl TimePicker;
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	BOOL OnInitDialog();
	CString CUserName;
	CString CPassword;
	CStatic IntroText;
};
