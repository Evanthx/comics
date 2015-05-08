// ScheduleDialog.cpp : implementation file
//

#include "stdafx.h"
#include "ComicSetup.h"
#include "ScheduleDialog.h"
#include <direct.h>
#include <string>
#include "CTask.h"

// ScheduleDialog dialog

IMPLEMENT_DYNAMIC(ScheduleDialog, CDialog)

ScheduleDialog::ScheduleDialog(CWnd* pParent /*=NULL*/)
	: CDialog(ScheduleDialog::IDD, pParent)
	, CUserName(_T(""))
	, CPassword(_T(""))
{

}

ScheduleDialog::~ScheduleDialog()
{
}

void ScheduleDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_COMICTIMEPICK, TimePicker);
	DDX_Text(pDX, IDC_USERNAME, CUserName);
	DDX_Text(pDX, IDC_PASSWORD, CPassword);
	DDX_Control(pDX, IDC_IntroText, IntroText);
}


BEGIN_MESSAGE_MAP(ScheduleDialog, CDialog)
	ON_BN_CLICKED(IDOK, &ScheduleDialog::OnBnClickedOk)
	ON_WM_CREATE()
END_MESSAGE_MAP()


// ScheduleDialog message handlers
/**************************************************************************/

void ScheduleDialog::OnBnClickedOk()
{
	//Get values from the dialog
	UpdateData(TRUE);

	if (ScheduleTheTask())
	{
		OnOK();
	}
	return;
}

/**************************************************************************/

bool ScheduleDialog::ScheduleTheTask(void)
{
	char tmpCurrDir[_MAX_PATH];
	_getcwd(tmpCurrDir,_MAX_PATH);
	std::string CurrDir = tmpCurrDir;


//	TimePicker

	time_t CurrTime;
	time(&CurrTime);

	CTask task; //constructing a CTask class object
	CTime time3(CurrTime); //date of creation
	CTime time2(2008, 6, 4, 9, 9, 0);

	CTime time;
	TimePicker.GetTime(time);

	//full path to the programme
    task.SetProgram ( "ComicReader.exe" );
    //execution parameters
    task.SetParameters ( "" );
    //starting directory
	task.SetStartingDir ( CurrDir.c_str() );
    //account name
    task.SetAccountName ( CUserName );
    //account password
    task.SetPassword ( CPassword );
	if (CPassword.IsEmpty())
	{
		MessageBox("The task scheduler won't accept blank passwords.  I hope to find a way around that but for now ... if you are at home and don't use a password, this won't work.");
		return(false);
	}
    //comment
    task.SetComment ( "Comic Reader Program" );

	task.SetStartDateTime ( time ); //start date

    //frequency adding the task to the scheduler

    task.SetFrequency ( CTask::freqDaily );
	
	HRESULT RetVal = task.SaveTask ( "Comic Reader", FALSE );

    if ( RetVal == S_OK )
	{
        MessageBox("The task has been added!", MB_OK);
		return(true);
     }
 
	char Msg[4097];
	FormatMessage (FORMAT_MESSAGE_FROM_SYSTEM, NULL, RetVal, 0, Msg, 4096, NULL);
	std::string ErrString = "Failed to create a task - depends on the error but I'd suggest checking the user name and password.  \nThe error returned by the task scheduler was ";
	ErrString += Msg;
	MessageBox(ErrString.c_str(), MB_OK);

	return(false);
}

/**************************************************************************/

int ScheduleDialog::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CDialog::OnCreate(lpCreateStruct) == -1)
		return -1;
	return 0;
}

/**************************************************************************/

BOOL ScheduleDialog::OnInitDialog()
{
	CDialog::OnInitDialog();
	TimePicker.SetFormat("hh:mm tt'");
	IntroText.SetWindowText("This will schedule the Comic Reader to run once a day at the time you specify.  This means it will be up and waiting when you arrive in the morning.  The real use is for days when you aren't at the computer, when you come back multiple days can be waiting for you.  The program just launches a browser with the comics page in it - so this will only be really useful if you have your browser set to open new pages in a new window or a new tab.  Otherwise you might be able to see previous pages by hitting the back button, but opening new pages in a new tab really makes this work best.");
	char* UserName = (char*)malloc((500) * sizeof(char));
	DWORD BufSize = 500;
	if (GetUserName(UserName, &BufSize) == ERROR_INSUFFICIENT_BUFFER)
	{
		free(UserName);
		UserName = (char*)malloc(BufSize * sizeof(char));
		GetUserName(UserName, &BufSize);
	}
	CUserName = UserName;
	free(UserName);
	//Write values to the dialog box
	UpdateData(FALSE);

	return(TRUE);
}

/**************************************************************************/
