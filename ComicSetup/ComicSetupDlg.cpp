// ComicSetupDlg.cpp : implementation file
//

#include "stdafx.h"
#include "ComicSetup.h"
#include "ComicSetupDlg.h"
#include "ScheduleDialog.h"
#include <fstream>
#include "CommonComicHeader.h"
using namespace std;

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

////////////////////////////////////////////////////////////////////////////
// CComicSetupDlg dialog

CComicSetupDlg::CComicSetupDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CComicSetupDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CComicSetupDlg)
	m_HelpText = _T("");
	//}}AFX_DATA_INIT
	// Note that LoadIcon does not require a subsequent DestroyIcon in Win32
	m_hIcon = AfxGetApp()->LoadIcon(IDI_BULLSEYE);
}

void CComicSetupDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CComicSetupDlg)
	DDX_Control(pDX, IDC_SelectedComics, m_SelectedComics);
	DDX_Control(pDX, IDC_AllComics, m_AllComics);
	DDX_Text(pDX, IDC_TextBox, m_HelpText);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CComicSetupDlg, CDialog)
	//{{AFX_MSG_MAP(CComicSetupDlg)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_UnSelect, OnUnSelect)
	ON_BN_CLICKED(IDC_Select, OnSelect)
	ON_COMMAND(ID_OPTIONS_SELECTALL, OnOptionsSelectall)
	ON_COMMAND(ID_OPTIONS_UNSELECTALL, OnOptionsUnselectall)
	ON_WM_VKEYTOITEM()
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDC_Schedule, &CComicSetupDlg::OnBnClickedSchedule)
END_MESSAGE_MAP()

////////////////////////////////////////////////////////////////////////////
// CComicSetupDlg message handlers
/**************************************************************************/

BOOL CComicSetupDlg::OnInitDialog()
{
	m_HelpText = "Highlight one or more comics on the left and click the right arrow button to select them.  Once they are in the right pane, you can order them by dragging names in the list into the order you want to read the comics.  Note that if you have a comic highlighted in the right pane, then comics you add will go after the highlighted one.  Otherwise they will add to the end of the list.";

	CDialog::OnInitDialog();
	CoInitialize(NULL);

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon
	
	//Set ArraySize to the number of items in the array
	ArraySize = IniFile.ListSize;

	//First clear the boxes (just in case)
	m_SelectedComics.ResetContent();
	m_AllComics.ResetContent();

	//Fill in the list boxes with default values
	for (int i = 0; i < ArraySize; i++)
	{
		if (IniFile[i]->Subscribed)
		{
			m_SelectedComics.AddString(IniFile[i]->Name.c_str());
		}
		else
		{
			m_AllComics.AddString(IniFile[i]->Name.c_str());
		}
	}
	
	return TRUE;  // return TRUE  unless you set the focus to a control
}

/**************************************************************************/

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CComicSetupDlg::OnPaint() 
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

/**************************************************************************/

// The system calls this to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CComicSetupDlg::OnQueryDragIcon()
{
	return (HCURSOR) m_hIcon;
}

/**************************************************************************/

void CComicSetupDlg::OnUnSelect() 
{
//	TransferStrings(m_SelectedComics, m_AllComics);	

	int ptr = m_SelectedComics.GetCurSel();

	if (ptr == LB_ERR)
	{
		//nothing was selected
		return;
	}

	CString ComicName;
	
	m_SelectedComics.GetText(ptr,ComicName);
	m_AllComics.AddString(ComicName);
	m_SelectedComics.DeleteString(ptr);

	return;
	
}

/**************************************************************************/

void CComicSetupDlg::OnSelect() 
{

	int Num_Picked = m_AllComics.GetSelCount();

	int SelListPtr = m_SelectedComics.GetCurSel();
	
	CArray<int,int> ListBoxSel;
	ListBoxSel.SetSize(Num_Picked);
	m_AllComics.GetSelItems(Num_Picked, ListBoxSel.GetData()); 

	int ptr,i;
	CString ComicName;

	//Add the comics - no deletions, as that throws the pointer off.
	//I did this in reverse which worked, but always threw me off when files
	//got added in reverse order.  I like doing two loops better...slower,
	//but speed isn't an issue here.
	for (i = 0; i < Num_Picked; i++)
	{
		ptr = ListBoxSel[i];
		m_AllComics.GetText(ptr,ComicName);
		
		//Add it to the selected list
		if (SelListPtr == LB_ERR)
		{
			//add it to the end of the list
			m_SelectedComics.AddString(ComicName);
		}
		else
		{
			//Add it after the selected entry
			//Advance the selected entry by one so that if this is a multiple 
			//selection they all add in alpabetical order (otherwise they
			//add in reverse alphabetical order)
			m_SelectedComics.InsertString(++SelListPtr, ComicName);
		}
	}

	//Now delete them.  In reverse order, so the pointer
	//doesn't get messed up.
	for (i = Num_Picked - 1; i >= 0 ; i--)
	{
		ptr = ListBoxSel[i];
		//and remove it from the unselected list
		m_AllComics.DeleteString(ptr);
	}




	m_AllComics.SetSel(-1,false);
	m_SelectedComics.SetSel(-1,false);
}

/**************************************************************************/

void CComicSetupDlg::OnOK() 
{
	// TODO: Add extra validation here
	int ComicOrder = 0;
	int TopCount,lcv;
	CString ComicName;

	TopCount = m_SelectedComics.GetCount();

	for (lcv=0; lcv < TopCount; )
	{
		m_SelectedComics.GetText(lcv++,ComicName);
		IniFile.SetNumber(ComicOrder++,ComicName, true);
	}
	

	TopCount = m_AllComics.GetCount();

	for (lcv=0; lcv < TopCount; )
	{
		m_AllComics.GetText(lcv++,ComicName);
		IniFile.SetNumber(ComicOrder++,ComicName, false);
	}

	IniFile.WriteFileOnExit = true;

	WriteComicFiles();

	CDialog::OnOK();
}

/**************************************************************************/

void CComicSetupDlg::OnOptionsSelectall() 
{
	m_AllComics.SelItemRange(TRUE, 0, m_AllComics.GetCount());

	OnSelect();
	
	return;
}

/**************************************************************************/

void CComicSetupDlg::OnOptionsUnselectall() 
{
	while (m_SelectedComics.GetCount())
	{
		m_SelectedComics.SetCurSel(0);
		OnUnSelect();
	}

	return;	
}

/**************************************************************************/

//Create the comic_in.htm file
void CComicSetupDlg::WriteComicFiles(void)
{		
//Now create the comic_in.htm file
	ofstream HTMLWriter(ComicInFile);
	if (!HTMLWriter)
	{
		MessageBox("Can't open HTML output file","Error");
		return;
	}

	IniFile.SortEntries();

	//Write the headers
	HTMLWriter << "<html><head><title>Comics</title></head><body>" << endl << endl;
	HTMLWriter << "Comic program by <a href=\"mailto:evan@evan.org\">Evan Reynolds.</a>  Please refer to the <a href = \"ReadMe.txt\">ReadMe</a> file for directions on sending me comic requests." << endl << endl;

	
	for (int i = 0; i < ArraySize; i++)
	{
		if (IniFile[i]->Subscribed)
			HTMLWriter << IniFile[i]->URL.c_str() << endl;
		else
			break; //all the selected ones are at the top - no need to continue
	}
	

	//Write the end tags
	HTMLWriter << "</body></html>" << endl << endl;

	return;
}

/**************************************************************************/

int CComicSetupDlg::OnVKeyToItem(UINT nKey, CListBox* pListBox, UINT nIndex) 
{
	//Left arrow with SelectedComics having focus
	if (nKey == 37)
	{
		if (m_SelectedComics.GetFocus() == &m_SelectedComics)
		{
			OnUnSelect();
		}
	}
	//Right arrow with AllComics having focus
	else if (nKey == 39)
	{
		if (m_AllComics.GetFocus() == &m_AllComics)
		{
			OnSelect();
		}
	}

	return CDialog::OnVKeyToItem(nKey, pListBox, nIndex);
}

/**************************************************************************/

void CComicSetupDlg::OnBnClickedSchedule()
{
	ScheduleDialog SchedDlg;
	SchedDlg.DoModal();
}

/**************************************************************************/
