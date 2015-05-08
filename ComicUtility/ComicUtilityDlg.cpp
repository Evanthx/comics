
// ComicUtilityDlg.cpp : implementation file
//

#include "stdafx.h"
#include "ComicUtility.h"
#include "ComicUtilityDlg.h"
#include "ComicMaker.h"
#include <fstream>
#include "CommonComicHeader.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

using namespace std;

bool Done;
bool ThreadDone;

// CComicUtilityDlg dialog




CComicUtilityDlg::CComicUtilityDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CComicUtilityDlg::IDD, pParent)
{
	InitializeCriticalSection(&VecLock);
	PageFetcher = HTTPGet::getInstance();
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

CComicUtilityDlg::~CComicUtilityDlg(void)
{
	//TODO delete each item in the ComicURLVector 
	DeleteCriticalSection(&VecLock);
}

void CComicUtilityDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_DateTokenList, DateTokenList);
	DDX_Control(pDX, IDC_URLTokenList, URLTokenList);
	DDX_Control(pDX, IDC_STATUS, StatusText);
	DDX_Control(pDX, IDC_COMICLIST, ComicList);
	DDX_Control(pDX, IDC_TheComicImage, TheComicImage);
	DDX_Control(pDX, IDC_URLToFix, URLToEdit);
}

BEGIN_MESSAGE_MAP(CComicUtilityDlg, CDialog)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDOK, &CComicUtilityDlg::OnBnClickedOk)
	ON_BN_CLICKED(IDC_BUTTON1, &CComicUtilityDlg::OnBnClickedButton1)
	ON_BN_CLICKED(IDC_RestartChecking, &CComicUtilityDlg::OnBnClickedRestartchecking)
	ON_LBN_SELCHANGE(IDC_COMICLIST, &CComicUtilityDlg::OnLbnSelchangeComiclist)
	ON_LBN_DBLCLK(IDC_COMICLIST, &CComicUtilityDlg::OnLbnDblclkComiclist)
	ON_BN_CLICKED(IDC_RemoveWorking, &CComicUtilityDlg::OnBnClickedRemoveworking)
	ON_BN_CLICKED(IDC_BUTTON2, &CComicUtilityDlg::OnBnClickedButton2)
END_MESSAGE_MAP()


// CComicUtilityDlg message handlers
/*****************************************************************/
std::string GetHomePage(const std::string& strURL)
{
	std::string Temp;
	DWORD StartLoc, EndLoc;
	StartLoc = strURL.find("blank href=\"");  //find the start of the image link
	if (StartLoc == std::string::npos) return(Temp);
	StartLoc = strURL.find("\"", StartLoc);  //find the opening quote mark
	if (StartLoc == std::string::npos) return(Temp);
	++StartLoc;
	EndLoc = strURL.find("\"", StartLoc+1);  //find the ending quote mark
	if (EndLoc == std::string::npos) return(Temp);
	Temp = strURL.substr(StartLoc, EndLoc-StartLoc);	
	return(Temp);
}

bool ReduceIniURL(std::string& strURL)
{
	DWORD StartLoc, EndLoc;
	StartLoc = strURL.find("img src");  //find the start of the image link
	if (StartLoc == std::string::npos) return(false);
	StartLoc = strURL.find("\"", StartLoc);  //find the opening quote mark
	if (StartLoc == std::string::npos) return(false);
	++StartLoc;
	EndLoc = strURL.find("\"", StartLoc+1);  //find the ending quote mark
	if (EndLoc == std::string::npos) return(false);

	strURL = strURL.substr(StartLoc, EndLoc-StartLoc);
	return(true);
}

UINT FillInTokenListBox(LPVOID pParam)
{
	CComicUtilityDlg* theDialog = (CComicUtilityDlg*)pParam;
	
	string SearchPage;

	SetupIniFile OriginalSIF(ComicUtilityFile);
	if (!ComicMaker::ReadInFile(ComicUtilityFile, SearchPage))
	{
		MessageBox(NULL,"Can't find the input file.  Make sure you ran the comic setup program to choose your strips before running this program to download them.  This program won't work correctly without it.","Error",MB_OK);
	}

	//What's today's date?
	time_t theTime;
	_tzset();  //set for local time zone
	time(&theTime);

	theTime -= 18000;  //put into US Eastern Time - strips don't pop up at midnight UTC
	struct tm today;
	gmtime_s(&today, &theTime);

	bool ErrorFlag, NotUsedFlag;

	char temp[TempArraySize];
	ComicMaker::DATE_TOKEN currentDateToken;
	ComicMaker::URL_TOKEN_LIST currentURLTokenList;
	ifstream Reader(IniFileName);

	string TokenString, NewStatusTextString;

	ComicMaker CM(NULL);

	//Run through the date based codes.  Easy and fast, so do them first.
	//If the token starts with SUNDAY, only replace it on Sundays - otherwise
	//remove it.
	while (CM.GetDateToken(Reader,currentDateToken,temp))
	{
		TokenString = currentDateToken.Name;
		CM.ProcessADateBasedToken(TokenString, currentDateToken, today);
		NotUsedFlag = (SearchPage.find(currentDateToken.Name) == std::string::npos)?true:false;

		theDialog->NewDateListItem(currentDateToken.Name, TokenString, false, NotUsedFlag);
		CM.ProcessADateBasedToken(SearchPage, currentDateToken, today);
	}

	//Now for the hard ones.  These can't be computed.  We
	//actually have to fetch a web page and parse out each token.
	while (!Done && CM.GetURLTokenList(Reader,currentURLTokenList,temp))
	{
		NewStatusTextString = "Reading ";
		NewStatusTextString += currentURLTokenList.First.Name.c_str();
		theDialog->NewStatusText(NewStatusTextString);

		TokenString = currentURLTokenList.First.Name;
		CM.ProcessAURLBasedToken(TokenString, currentURLTokenList);

		ErrorFlag = (TokenString == TokenNotFound)?true:false;
		NotUsedFlag = (SearchPage.find(currentURLTokenList.First.Name) == std::string::npos)?true:false;
		theDialog->NewURLListItem(currentURLTokenList.First.Name, TokenString, ErrorFlag, NotUsedFlag);

		CM.ProcessAURLBasedToken(SearchPage, currentURLTokenList);
	};

	//Now we have all the tokens, and have filled out the URL list for the comics page.  
	//Check each comic listing.
	CM.WriteOutFile(SearchPage, SecondComicUtilityFile);
	SetupIniFile SIF(SecondComicUtilityFile);

	//Set ArraySize to the number of items in the comic array
	int ArraySize = SIF.ListSize;
	UTILITY_INFO* myInfo;
	std::string Path;
	for (int i = 0; i < ArraySize; i++)
	{
		if (Done)
		{
			//The box is closing out, so don't try to update it.
			break;
		}

		NewStatusTextString = "Checking ";
		NewStatusTextString += SIF[i]->Name.c_str();
		myInfo = new UTILITY_INFO;
		theDialog->NewStatusText(NewStatusTextString);
		myInfo->Working = true;
		myInfo->URL = SIF[i]->URL;
		if (myInfo->URL.empty()) continue;
		myInfo->FullURL = OriginalSIF[i]->URL;
		myInfo->HomePage = GetHomePage(myInfo->URL);
	
		CM.ParseTheURL(myInfo->HomePage, myInfo->HomePageServer, myInfo->HomePagePath);

		if (!ReduceIniURL(myInfo->URL)) continue;

		if (!CM.ParseTheURL(myInfo->URL, myInfo->Server, myInfo->Path))
		{
			//Couldn't parse the URL ... not good.
			return(true);  //not a 404, but still.
		}
		ErrorFlag = CM.IsThisA404(myInfo->Server, myInfo->Path);
		myInfo->Name = SIF[i]->Name;
		if (ErrorFlag)
		{
			myInfo->Working = false;  //flag this as false
		}
		theDialog->NewComicListItem(myInfo, ErrorFlag, false);
	}

	if (!Done)
	{
		theDialog->NewStatusText("Finished reading comics.");
	}
	ThreadDone = true;
	return(0);
}

/*****************************************************************/

BOOL CComicUtilityDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon
	LoadEvanImage();
	CopyFile(SetupIniFileName, ComicUtilityFile, false);

	KickEverythingOff();

	return TRUE;  // return TRUE  unless you set the focus to a control
}

/*****************************************************************/

void CComicUtilityDlg::LoadEvanImage()
{
	if (!ComicMaker::DoesFileExist("evan.jpg"))
	{
		PageFetcher->WriteWebPage("www.evan.org", "magic/Pictures/TinyLogo.jpg", "evan.jpg");
	}
	DisplayImage("evan.jpg");
	return;
}

/*****************************************************************/

void CComicUtilityDlg::KickEverythingOff(void)
{
	Done = false;
	ThreadDone = false;

	//Empty out the list boxes
	DateTokenList.ResetContent();
	URLTokenList.ResetContent();
	ComicList.ResetContent();
	ComicURLVector.clear();

	NewURLListItem("", " Token Not Creatable", true, false);
	NewURLListItem("", " Token Not Used", false, true);
	NewURLListItem("", " Token Not Creatable Or Used", true, true);
	NewDateListItem("", " Token Not Creatable", true, false);
	NewDateListItem("", " Token Not Used", false, true);
	NewDateListItem("", " Token Not Creatable Or Used", true, true);

	AfxBeginThread(FillInTokenListBox,this);
	return;
}
/*****************************************************************/

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CComicUtilityDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

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

/*****************************************************************/

// The system calls this function to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CComicUtilityDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

/*****************************************************************/

void CComicUtilityDlg::NewStatusText(const std::string& NewText)
{
	if (Done) return;
	StatusText.SetWindowText(NewText.c_str());
	return;
}

/*****************************************************************/

void CComicUtilityDlg::NewURLListItem(const std::string& Name, const std::string& Value, bool ErrorFlag, bool UnusedFlag)
{
	if (Done) return;
	std::string ListBoxString;

	if (!Name.empty())
	{
		ComicTokens NewToken;
		NewToken.Name = Name;
		NewToken.Value = Value;

		//First, if it's already in the list with "NotFound" as the value, remove it so that this will replace it
		set<ComicTokens>::iterator TokenIterator;
		TokenIterator = ComicTokenVector.find(NewToken);
		if (TokenIterator != ComicTokenVector.end() && TokenIterator->Value == TokenNotFound)
		{
			ComicTokenVector.erase(TokenIterator);
		}

		ComicTokenVector.insert(NewToken);
		ListBoxString = Name;
		ListBoxString += " - ";
	}

	ListBoxString += Value;
	URLTokenList.AddString(ListBoxString.c_str(), RGB(ErrorFlag?255:0, 0, UnusedFlag?255:0));
	return;
}

/*****************************************************************/

void CComicUtilityDlg::NewDateListItem(const std::string& Name, const std::string& Value, bool ErrorFlag, bool UnusedFlag)
{
	if (Done) return;
	std::string ListBoxString;

	if (!Name.empty())
	{
		ComicTokens NewToken;
		NewToken.Name = Name;
		NewToken.Value = Value;
		ComicTokenVector.insert(NewToken);
		ListBoxString = Name;
		ListBoxString += " - ";
	}

	ListBoxString += Value;
	DateTokenList.AddString(ListBoxString.c_str(), RGB(ErrorFlag?255:0, 0, UnusedFlag?255:0));
	return;
}

/*****************************************************************/

void CComicUtilityDlg::NewComicListItem(UTILITY_INFO* NewComic, bool ErrorFlag, bool UnusedFlag)
{
	ComicList.AddString(NewComic->Name.c_str(), RGB(ErrorFlag?255:0, 0, UnusedFlag?255:0));
	AddComicToVector(NewComic);
	return;
}

/*****************************************************************/

void CComicUtilityDlg::OnBnClickedOk()
{
	Done = true;
	int i = 0;
	while (!ThreadDone)
	{
		if (i++ > 10)
		{

			//Don't wait TOO long for the thread to stop
			break;
		}
		Sleep(500);
	}

	OnOK();
}

/*****************************************************************/

void CComicUtilityDlg::DisplayImage(const std::string& FileName)
{
	bool FileOK = true;
	if (!ComicMaker::DoesFileExist(FileName)) 
	{
		FileOK = false;
	}

	HRESULT HRet;

	if (FileOK)
	{
		m_image.Destroy();
		HRet = m_image.Load(FileName.c_str());
		if (HRet != S_OK)
			FileOK = false;
	}

	if (!FileOK)
	{
		//default to my logo to give some feedback
		m_image.Destroy();
		if (m_image.Load("evan.jpg") != S_OK)
		{
			//Bah.  Quit trying.
			return;
		}
	}

	CDC dc;
	dc.CreateCompatibleDC(NULL);

	SetStretchBltMode(dc, HALFTONE);

	int h=m_image.GetHeight();
	int w=m_image.GetWidth();
	int NewHeight=200;
	int NewWidth=200*w/h;
//	m_image.StretchBlt(dc,0,0,NewWidth,NewHeight,SRCCOPY);
//	m_image.StretchBlt(dc,0,0,NewWidth,NewHeight,0,0,w,h,SRCCOPY);

	CBitmap *pb = new CBitmap;
	pb->CreateCompatibleBitmap(&dc, NewWidth, NewHeight);
	CBitmap *pob = dc.SelectObject(pb);
	m_image.StretchBlt(dc,0,0,NewWidth,NewHeight,0,0,w,h,SRCCOPY);
	dc.SelectObject(pob);

	new_image.Destroy();
	new_image.Attach((HBITMAP)(*pb));

	TheComicImage.SetBitmap((HBITMAP)new_image);  //load the image
	TheComicImage.Invalidate();  //make the control redraw itself	
	return;
}

/*****************************************************************/

void CComicUtilityDlg::OnBnClickedButton1()
{
	//Stop the threads
	Done = true;
	return;
}

/*****************************************************************/

void CComicUtilityDlg::OnBnClickedRestartchecking()
{
	//First, make sure the thread is stopped.
	NewStatusText("Restarting thread");
	Done = true;
	int i = 0;
	while (!ThreadDone)
	{
		if (i++ > 10)
		{
			//Don't wait TOO long for the thread to stop
			break;
		}
		Sleep(500);
	}

	KickEverythingOff();
	return;
}

/*****************************************************************/

void CComicUtilityDlg::OnLbnSelchangeComiclist()
{
	int i = ComicList.GetCurSel();
	UTILITY_INFO* theComic = GetComicByID(i);
	if (theComic == NULL) 
	{
		return;
	}

	NewStatusText(theComic->URL.c_str());
	_unlink("comic.jpg");
	PageFetcher->WriteWebPage(theComic->Server.c_str(), theComic->Path.c_str(), "comic.jpg");
	DisplayImage("comic.jpg");

	return;
}

/*****************************************************************/

void CComicUtilityDlg::AddComicToVector(UTILITY_INFO* NewComic)
{
	EnterCriticalSection(&VecLock);
	ComicURLVector.push_back(NewComic);
	LeaveCriticalSection(&VecLock);
	return;
}

/*****************************************************************/

UTILITY_INFO* CComicUtilityDlg::GetComicByID(int i)
{
	UTILITY_INFO* RetInfo;
	EnterCriticalSection(&VecLock);

	if (ComicURLVector.size() < i)
	{
		//double check ...
		return(NULL);
	}
	RetInfo = ComicURLVector[i];

	LeaveCriticalSection(&VecLock);
	return(RetInfo);
}

/*****************************************************************/

void CComicUtilityDlg::OnLbnDblclkComiclist()
{
	int i = ComicList.GetCurSel();
	UTILITY_INFO* theComic = GetComicByID(i);
	if (theComic == NULL) 
	{
		return;
	}

	_unlink("source.txt");
	PageFetcher->WriteWebPage(theComic->HomePageServer.c_str(), theComic->HomePagePath.c_str(), "source.txt");
	ShellExecute(NULL, "open", "source.txt", NULL, NULL, SW_SHOW);

	//That opens up the source.  Also launch the web browser.
	ShellExecute(NULL, "open", theComic->HomePage.c_str(), NULL, NULL, SW_SHOW);

}

/*****************************************************************/

void CComicUtilityDlg::OnBnClickedRemoveworking()
{
	//Stop the threads
	Done = true;

	ofstream Writer(ComicUtilityFile);
	if (!Writer)
	{
		MessageBox("Can't open utility ini file!","Error Message",MB_ICONERROR);
		return;
	}

	//Need to write the vector to the ini file
	vector<UTILITY_INFO*>::iterator ComicIterator;
	UTILITY_INFO* theItem;
	for (ComicIterator = ComicURLVector.begin(); ComicIterator != ComicURLVector.end(); ComicIterator++)
	{
		theItem = *ComicIterator;
		if (!theItem->Working)
		{
			Writer << theItem->Name;
			Writer << MidSeperator;
			Writer << theItem->FullURL;
			Writer << MidSeperator;			
			Writer << "N33";	//fake an order number
			Writer << EndSeparator;
		}
	}

	//And now reload everything, which will remove all the working items as we didn't write those
	OnBnClickedRestartchecking();
	return;
}

/*****************************************************************/

void CComicUtilityDlg::OnBnClickedButton2()
{
	// TODO: Add your control notification handler code here
	CString UrlString;
	URLToEdit.GetWindowText(UrlString);
	std::string strUrlString = UrlString;

	//Walk through the tokens, see if any match.
	set<ComicTokens>::iterator TokenIterator;
	set<ComicTokens>::iterator FoundIterator = ComicTokenVector.end();
	DWORD Loc, FoundLoc;
	for (TokenIterator = ComicTokenVector.begin(); TokenIterator != ComicTokenVector.end(); TokenIterator++)
	{
		Loc = strUrlString.find(TokenIterator->Value);
		if (Loc != std::string::npos)
		{
			//Found one!  Look for more, and choose the longest one.
			if (FoundIterator == ComicTokenVector.end() || FoundIterator->Value.length() < TokenIterator->Value.length())
			{
				FoundLoc = Loc;
				FoundIterator = TokenIterator;
			}
		}
	}
	
	if (FoundIterator != ComicTokenVector.end())
	{
		strUrlString.replace(FoundLoc,FoundIterator->Value.length(),FoundIterator->Name);
	}

	URLToEdit.SetWindowText(strUrlString.c_str());
}

/*****************************************************************/


