// ComicUpdaterDlg.cpp : implementation file
//

#include "stdafx.h"
#include "ComicUpdater.h"
#include "ComicUpdaterDlg.h"
#include "CommonComicHeader.h"
#include "ComicMaker.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


/////////////////////////////////////////////////////////////////////////////
// CComicUpdaterDlg dialog

CComicUpdaterDlg::CComicUpdaterDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CComicUpdaterDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CComicUpdaterDlg)
	//}}AFX_DATA_INIT
	// Note that LoadIcon does not require a subsequent DestroyIcon in Win32
	m_hIcon = AfxGetApp()->LoadIcon(IDI_PHONE_ICON);
}

void CComicUpdaterDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CComicUpdaterDlg)
	DDX_Control(pDX, IDC_MESSAGES, m_Messages);
	DDX_Control(pDX, IDOK, m_ok);
	DDX_Control(pDX, IDCANCEL, m_cancel);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CComicUpdaterDlg, CDialog)
	//{{AFX_MSG_MAP(CComicUpdaterDlg)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CComicUpdaterDlg message handlers

BOOL CComicUpdaterDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon
	
	// TODO: Add extra initialization here
	m_Messages.SetWindowText("May I update the comics program?");
	UpdateData(FALSE);
	
	return TRUE;  // return TRUE  unless you set the focus to a control
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CComicUpdaterDlg::OnPaint() 
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
HCURSOR CComicUpdaterDlg::OnQueryDragIcon()
{
	return (HCURSOR) m_hIcon;
}

/************************************************************/

void GetFile(std::string FileName, HTTPGet* PageFetcher)
{
	std::string WebFilePath;
	WebFilePath = "/Software/files/";
	WebFilePath += FileName;
	
	PageFetcher->WriteWebPage("www.evan.org",WebFilePath.c_str(),"temp.file");
	remove(FileName.c_str());
	rename("temp.file",FileName.c_str());
	
	return;
}

/************************************************************/
	
void UpdateIni(CEdit* messagebox, HTTPGet* PageFetcher)
{
	messagebox->SetWindowText("Downloading comics.ini...");
	GetFile(IniFileName,PageFetcher);

	messagebox->SetWindowText("Downloading update.txt...");

	GetFile(UpdateFileName,PageFetcher);
	return;
}

/************************************************************/

void UpdateProg(CEdit* messagebox, HTTPGet* PageFetcher)
{
	messagebox->SetWindowText("Downloading ComicSetup.exe...");
	GetFile("ComicSetup.exe",PageFetcher);

	messagebox->SetWindowText("Downloading ComicReader.exe...");
	GetFile("ComicReader.exe",PageFetcher);

	messagebox->SetWindowText("Downloading readme.txt...");
	GetFile("readme.txt",PageFetcher);
	return;
}

/************************************************************/

UINT RunUpdate(LPVOID pParam)
{
	CComicUpdaterDlg* theDialog = (CComicUpdaterDlg*)pParam;
	CEdit* theMessageBox = &(theDialog->m_Messages);
	HTTPGet* PageFetcher = HTTPGet::getInstance();

	theMessageBox->SetWindowText("Checking to see if updates are available...");

	ComicMaker cmaker(NULL);

	bool ini, prog, upd;
	int progVer, iniVer, updateVer;
	if (cmaker.CheckForUpdates(ini, prog, upd, iniVer, progVer, updateVer))
	{
		if (ini)
		{
			UpdateIni(theMessageBox, PageFetcher);
		}
		if (prog)
		{
			UpdateProg(theMessageBox, PageFetcher);
		}
		
		//Write a new version file
		
		//Get the version number of the updater (me) since this program doesn't update that
		std::string PageToRead,Version;
		cmaker.ReadInFile(VersionFileName,PageToRead);
		if (!PageToRead.empty())
		{
			cmaker.ParseWebPage(PageToRead,"updater",Version);
			updateVer = atoi(Version.c_str());
		}
		
		cmaker.WriteVersionFile(progVer, iniVer, updateVer);

		//Launch the setup program to make any updates take effect
		ShellExecute(GetDesktopWindow(), NULL,
			"ComicSetup.exe", NULL, NULL, SW_SHOW);
	}

	theDialog->SendMessage(WM_CLOSE);
	
	return(0);
}

/***************************************/

void CComicUpdaterDlg::OnOK() 
{
	//User is willing to update the code.
	
	m_ok.EnableWindow(false);
	m_cancel.SetWindowText("Cancel");

	AfxBeginThread(RunUpdate,this);

	return;

	//don't call this as it'll close the window...
//	CDialog::OnOK();
}

/***************************************/

