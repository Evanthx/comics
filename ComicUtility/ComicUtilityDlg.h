
// ComicUtilityDlg.h : header file
//

#pragma once
#include "afxwin.h"
#include <string>
#include <vector>
#include <set>
#include "MyListBox.h"
#include "ComicMaker.h"
#include "HTTPGet.h"
#include "SetupIniReader.h"

struct UTILITY_INFO
{
	std::string Name;
	std::string FullURL;
	std::string URL;
	std::string Server;
	std::string Path;
	std::string HomePage;
	std::string HomePageServer;
	std::string HomePagePath;
	bool Working;
};

struct ComicTokens
{
	std::string Name;
	std::string Value;
	bool operator==(const ComicTokens& Other) const {return(Name==Other.Name);};
    bool operator!=(const ComicTokens& Other) const {return(Name!=Other.Name);};
    bool operator>(const ComicTokens& Other) const {return(Name>Other.Name);};
    bool operator<(const ComicTokens& Other) const {return(Name<Other.Name);};
};

// CComicUtilityDlg dialog
class CComicUtilityDlg : public CDialog
{
// Construction
public:
	CComicUtilityDlg(CWnd* pParent = NULL);	// standard constructor
	~CComicUtilityDlg(void);

// Dialog Data
	enum { IDD = IDD_COMICUTILITY_DIALOG };
	void NewStatusText(const std::string& NewText);
	void NewURLListItem(const std::string& Name, const std::string& Value, bool ErrorFlag, bool UnusedFlag);
	void NewDateListItem(const std::string& Name, const std::string& Value, bool ErrorFlag, bool UnusedFlag);
	void NewComicListItem(UTILITY_INFO* NewComic, bool ErrorFlag, bool UnusedFlag);

protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	void KickEverythingOff(void);
//	UINT FillInTokenListBox(LPVOID pParam);

// Implementation
protected:
	HICON m_hIcon;
	HTTPGet* PageFetcher;

	// Generated message map functions
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	MyListBox DateTokenList;
	MyListBox URLTokenList;
	MyListBox ComicList;
	void LoadEvanImage();
	void DisplayImage(const std::string& FileName);
	CEdit StatusText;
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedButton1();
	afx_msg void OnBnClickedRestartchecking();
	CStatic TheComicImage;
	CImage m_image, new_image;
	afx_msg void OnLbnSelchangeComiclist();

	std::vector<UTILITY_INFO*> ComicURLVector;
	std::set<ComicTokens> ComicTokenVector;
	
	void AddComicToVector(UTILITY_INFO* NewComic);
	UTILITY_INFO* GetComicByID(int i);
	CRITICAL_SECTION VecLock;
	afx_msg void OnLbnDblclkComiclist();
	afx_msg void OnBnClickedRemoveworking();
	CEdit URLToEdit;
	afx_msg void OnBnClickedButton2();
};
