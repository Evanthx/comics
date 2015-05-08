// WinComicsDlg.cpp : implementation file
//

#include "stdafx.h"
#include "WinComics.h"
#include "WinComicsDlg.h"
#include <string>
#include "CommonComicHeader.h"
#include "ComicMaker.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CWinComicsDlg dialog

CWinComicsDlg::CWinComicsDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CWinComicsDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CWinComicsDlg)
	//}}AFX_DATA_INIT
	// Note that LoadIcon does not require a subsequent DestroyIcon in Win32
	m_hIcon = AfxGetApp()->LoadIcon(IDI_PENCIL_ICON);
}

void CWinComicsDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CWinComicsDlg)
	DDX_Control(pDX, IDC_STATUSLIST, m_StatusList);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CWinComicsDlg, CDialog)
	//{{AFX_MSG_MAP(CWinComicsDlg)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CWinComicsDlg message handlers

//RunComics runs as a thread - it does all the work.
//The code it uses is in the file HTTPGet.cpp.

UINT RunComics(LPVOID pParam)
{
	CWinComicsDlg* theDialog = (CWinComicsDlg*)pParam;
	CListBox* listbox = &theDialog->m_StatusList;
	ComicMaker comics(listbox);
	bool UpdatesAvailable = comics.Runner();

	listbox->InsertString(0,"Done!  Lauching Web Browser.");
	//launch the web browser
	ShellExecute(GetDesktopWindow(), "open",
		ComicOutFile, NULL, NULL, SW_SHOW);
	
	if (UpdatesAvailable)
	{
		ShellExecute(GetDesktopWindow(), NULL,
			"ComicUpdater.exe", NULL, NULL, SW_SHOW);
	}

	
	theDialog->SendMessage(WM_CLOSE);
	return(0);
}


BOOL CWinComicsDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon
	
	// TODO: Add extra initialization here

	AfxBeginThread(RunComics,this);

	return TRUE;  // return TRUE  unless you set the focus to a control
}



// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CWinComicsDlg::OnPaint() 
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, (WPARAM) dc.GetSafeHdc(), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

// The system calls this to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CWinComicsDlg::OnQueryDragIcon()
{
	return (HCURSOR) m_hIcon;
}
