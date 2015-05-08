
#include "stdafx.h"
#include "ComicMaker.h"

#include <string>  
#include "errno.h"
#include "CommonComicHeader.h"
#include <wininet.h>

using namespace std;

/************************************************/

ComicMaker::ComicMaker(CListBox* inListbox)
{
	PageFetcher = HTTPGet::getInstance();
	theListbox = inListbox;
	return;
}

/************************************************/
ComicMaker::~ComicMaker()
{
	return;
}

/************************************************/
	
void ComicMaker::WriteVersionFile(const int progVersion, const int iniVersion, const int UpdateVersion)
{
	//Write a new version file using the numbers given
	ofstream Writer(VersionFileName);

	Writer << "program";
	Writer << progVersion;
	Writer << "." << endl;

	Writer << "data";
	Writer << iniVersion;
	Writer << "." << endl;

	Writer << "updater";
	Writer << UpdateVersion;
	Writer << "." << endl;

	Writer.close();
}

/************************************************/

void ComicMaker::ParseWebPage(const string &WebPage, string SearchString, string &ComicID)
{
	//WebPage has the entire comic web page.  We are looking for this line (for example):
	//<IMG SRC="/comics/robotman/archive/images/robotman2003479400726.gif" ALT="today's comics strip" ALIGN=TOP BORDER="0">
	//From that, we want the ID number, 2003479400726 in this case.

	//So the basic algorithm -
	//find the string "<IMG SRC="/comics/robotman/archive/images/robotman"
	//Get the numbers after that string and before the following "."
	//Put those numbers in ComicID

	//NEW - this is now more generic.  Rather than have robotman coded into this, it's passed in through
	//SearchString. That lets me use this method for other strips that I have to parse for.

	DWORD Loc, Loc2, Loc3, Loc4;
	Loc = WebPage.find(SearchString);
	if (Loc < 0)
	{
		if (theListbox != NULL) theListbox->InsertString(0,"Error parsing for the comic id from the web page.");
		//Continue and get the date based items anyway
		ComicID = TokenNotFound;
		return;
	}
	Loc += SearchString.length();

	//Now Loc is at the start of the ComicID.  Find the end of the comic ID, and
	//put the ComicID into the string waiting for it.  The end is normally a period, but
	//look for quote marks as well as that is occasionally used.
	Loc2 = WebPage.find(".",Loc);
	Loc3 = WebPage.find("\"",Loc);
	Loc4 = WebPage.find("'",Loc);

	//Find the shortest word
	if (Loc3 != std::string::npos && Loc3 < Loc2) 
	{
		Loc2 = Loc3;
	}
	if (Loc4 != std::string::npos && Loc4 < Loc2) 
	{
		Loc2 = Loc4;
	}

	if (Loc2 < 0)
	{
		if (theListbox != NULL) theListbox->InsertString(0,"Error parsing for the comic id from the web page.");
		//Continue with a default (nonworking) value
		ComicID = TokenNotFound;
		return;
	}

	ComicID = WebPage.substr(Loc,Loc2 - Loc);
	return;
}

/***************************************************************/

bool ComicMaker::WriteOutFile(const std::string& StringToWrite, const std::string& FileName)
{
	ofstream Writer(FileName.c_str());
	if (!Writer)
	{
		string s = "Can't open output file ";
		s += FileName;
		s += " .";
		MessageBox(NULL,s.c_str(),"Error Message",MB_OK);
		return(false);
	}
	Writer << StringToWrite;
	return(true);
}

/***************************************************************/

bool ComicMaker::ReadInFile(const char *FileToRead, string &OutPage)
{
	//Read the file named by the variable inFile to the string OutPage.
	OutPage = "";

	ifstream Reader(FileToRead);
	if (!Reader)
	{
		string s;
		s = "Can't open input file ";
		s += FileToRead;
		s += " .";
		MessageBox(NULL,s.c_str(),"Error Message",MB_OK);
		return(false);
	}
	
	char inBuf[500];
	//Erase any previous contents of OutPage
	while (Reader.getline(inBuf,500))
	{
		OutPage += inBuf;
		OutPage += "\n";
	}

	return(true);
}

/***************************************************************/

void ComicMaker::ParseSingleToken(string &OutPage, const char* Token, const char* NewVal)
{
	//Walk through OutPage.  Replace each occurrence of Token with NewVal.
	int Loc = 0;
	if (strlen(Token) == 0)
		return;

	while ((Loc = OutPage.find(Token, Loc)) >= 0)
	{	
		//Now Loc points to the start of the Token.  Replace it with NewVal.
		OutPage.replace(Loc,strlen(Token),NewVal);
	}	
	return;
}

/***************************************************************/

void ComicMaker::ProcessADateBasedToken(std::string &OutPage, DATE_TOKEN& currentDateToken, const tm& today)
{
	struct tm WorkingTime = today;
	WorkingTime.tm_mday = today.tm_mday - currentDateToken.NumDaysAgo;
	mktime(&WorkingTime);

	//A buffer to hold the date strings
	const int BufSize = 50;
	char CodeBuf[BufSize];

	//Sunday only tokens
	if (currentDateToken.Name.substr(0,6) == "SUNDAY")
	{
		if (WorkingTime.tm_wday != 0)
		{
			CodeBuf[0] = '\0';
		}
		else
		{
			sprintf_s(CodeBuf,50,"%s",currentDateToken.Value.c_str());
		}
		ParseSingleToken(OutPage, currentDateToken.Name.c_str(), CodeBuf);
		return;
	}

	//Weekday only tokens
	if (currentDateToken.Name.substr(0,7) == "WEEKDAY")
	{
		if (WorkingTime.tm_wday == 0)
		{
			CodeBuf[0] = '\0';
		}
		else
		{
			sprintf_s(CodeBuf,50,"%s",currentDateToken.Value.c_str());
		}
		ParseSingleToken(OutPage, currentDateToken.Name.c_str(), CodeBuf);
		return;
	}

	//a kludge...seems like %u isn't implemented on my compiler.  But %w is.  So fake %u by
	//using %w if needed.
	bool FakingU = false;
	if (currentDateToken.Value == "%u")
	{
		FakingU = true;
		currentDateToken.Value = "%w";
	}
	mktime(&WorkingTime);
	strftime(CodeBuf,BufSize,currentDateToken.Value.c_str(),&WorkingTime);
	if (FakingU && !strncmp(CodeBuf,"0",1))
	{
		sprintf_s(CodeBuf,50,"%d",7);
	}
	ParseSingleToken(OutPage, currentDateToken.Name.c_str(), CodeBuf);
	return;
}

/***************************************************************/

void ComicMaker::DownloadAToken(URL_TOKEN& currentURLToken, std::string& ComicID)
{
	string WebPage;
	string s = "About to read web page for ";
	s += currentURLToken.Name;
	if (theListbox != NULL)	theListbox->InsertString(0,s.c_str());
	PageFetcher->GetWebPage(currentURLToken.Domain.c_str(), currentURLToken.Path.c_str(), WebPage);
	if (WebPage.empty()) //nothing there ... don't continue.
		return;

	//Now, parse through that web page to get the desired token
	ParseWebPage(WebPage,currentURLToken.ParseToken,ComicID);

	s = "Finished with ";
	s += currentURLToken.Name;
	if (theListbox != NULL) theListbox->InsertString(0,s.c_str());
	return;
}

/***************************************************************/

//a private method used by ParseTheURL and not declared in the header file
bool ParseURLWorker(LPCTSTR pstrURL,
	LPURL_COMPONENTS lpComponents, DWORD dwFlags)
{
	// this function will return bogus stuff if lpComponents
	// isn't set up to copy the components

	ASSERT(lpComponents != NULL && pstrURL != NULL);
	if (lpComponents == NULL || pstrURL == NULL)
		return false;
	ASSERT(lpComponents->dwHostNameLength == 0 ||
			lpComponents->lpszHostName != NULL);
	ASSERT(lpComponents->dwUrlPathLength == 0 ||
			lpComponents->lpszUrlPath != NULL);
	ASSERT(lpComponents->dwUserNameLength == 0 ||
			lpComponents->lpszUserName != NULL);
	ASSERT(lpComponents->dwPasswordLength == 0 ||
			lpComponents->lpszPassword != NULL);

//.	ASSERT(AfxIsValidAddress(lpComponents, sizeof(URL_COMPONENTS), true));

	LPTSTR pstrCanonicalizedURL;
	TCHAR szCanonicalizedURL[INTERNET_MAX_URL_LENGTH];
	DWORD dwNeededLength = INTERNET_MAX_URL_LENGTH;
	BOOL bRetVal;
	bool bMustFree = false;
	DWORD dwCanonicalizeFlags = dwFlags &
		(ICU_NO_ENCODE | ICU_DECODE | ICU_NO_META |
		ICU_ENCODE_SPACES_ONLY | ICU_BROWSER_MODE);
	DWORD dwCrackFlags = dwFlags & (ICU_ESCAPE | ICU_USERNAME);

	bRetVal = InternetCanonicalizeUrl(pstrURL, szCanonicalizedURL,
		&dwNeededLength, dwCanonicalizeFlags);

	if (!bRetVal)
	{
		if (::GetLastError() != ERROR_INSUFFICIENT_BUFFER)
			return false;

		pstrCanonicalizedURL = new TCHAR[dwNeededLength];
		bMustFree = true;
		bRetVal = InternetCanonicalizeUrl(pstrURL, pstrCanonicalizedURL,
			&dwNeededLength, dwCanonicalizeFlags);
		if (!bRetVal)
		{
			delete [] pstrCanonicalizedURL;
			return false;
		}
	}
	else
		pstrCanonicalizedURL = szCanonicalizedURL;

	// now that it's safely canonicalized, crack it

	bRetVal = InternetCrackUrl(pstrCanonicalizedURL, 0,
						dwCrackFlags, lpComponents);
	if (bMustFree)
		delete [] pstrCanonicalizedURL;

	//convert bRetVal to bool
	if (bRetVal)
		return(true);
	return false;
}

/***************************************************************/

bool ComicMaker::ParseTheURL(const std::string& strURL, std::string& strServer, std::string& strObject)
{
	TCHAR hostNameBuffer[INTERNET_MAX_URL_LENGTH+1];
	TCHAR urlBuffer[INTERNET_MAX_URL_LENGTH+1];

	ASSERT(!strURL.empty());
	if (strURL.empty())
		return false;

	URL_COMPONENTS urlComponents;
	memset(&urlComponents, 0, sizeof(URL_COMPONENTS));
	urlComponents.dwStructSize = sizeof(URL_COMPONENTS);

	urlComponents.dwHostNameLength = INTERNET_MAX_URL_LENGTH;
	urlComponents.lpszHostName = hostNameBuffer;
	urlComponents.dwUrlPathLength = INTERNET_MAX_URL_LENGTH;
	urlComponents.lpszUrlPath = urlBuffer;

	bool bRetVal = ParseURLWorker( strURL.c_str(),
								   &urlComponents,
								   ICU_BROWSER_MODE);

	strServer = hostNameBuffer;
	strObject = urlBuffer;
	return bRetVal;
}

/***************************************************************/

bool ComicMaker::IsThisA404(const std::string& Server, const std::string& Path)
{
	DWORD StatusCode = PageFetcher->GetStatusCode(Server.c_str(), Path.c_str());
	if (StatusCode == 404) 
	{
		return(true);
	}

	return(false);
}

/***************************************************************/

std::string ComicMaker::PickToken(const std::string& FirstID, const std::string& SecondID, const std::string& ThirdID)
{
	//We have three tokens.  Try to pick the best one.  Ignore any that are "NotFound" and find a two-out-of-three match.
	if (FirstID == TokenNotFound) 
	{
		if (SecondID == TokenNotFound)
		{
			return(ThirdID);
		}
		if (ThirdID == TokenNotFound)
		{
			return(SecondID);
		}
	}
	else if ((SecondID == TokenNotFound) && (ThirdID == TokenNotFound))
	{
		return(FirstID);
	}

	//Now we know that out of those three, at least two are found.  So we can just pick either the token that matches two of three- or else 
	//default to the first one that is found
	if (FirstID == SecondID)
		return(FirstID);

	if (FirstID == ThirdID)
		return(FirstID);

	if (SecondID == ThirdID)
		return(SecondID);

	//None of them match.  Bad.  Go with the first one that was found.
	if (FirstID != TokenNotFound)
	{
		return(FirstID);
	}
	if (SecondID != TokenNotFound)
	{
		return(SecondID);
	}
	return(ThirdID);
}

/***************************************************************/

void ComicMaker::ProcessAURLBasedToken(std::string &OutPage, URL_TOKEN_LIST& currentURLTokenList)
{
	string ComicID;
	//First, see if we need to get this token.  If the token isn't in the
	//in file, don't bother with it.
	if (OutPage.find(currentURLTokenList.First.Name) != string::npos)
	{
		//Get the web page
		DownloadAToken(currentURLTokenList.First, ComicID);
		if (currentURLTokenList.UseList)
		{
			std::string SecondID, ThirdID;
			DownloadAToken(currentURLTokenList.Second, SecondID);
			DownloadAToken(currentURLTokenList.Third, ThirdID);
			//Now pick the most likely token - if there's a two out of three match, use it.
			ComicID = PickToken(ComicID, SecondID, ThirdID);
		}
		ParseSingleToken(OutPage, currentURLTokenList.First.Name.c_str(), ComicID.c_str());
	}
	else
	{
		string s = "Skipping unused token ";
		s += currentURLTokenList.First.Name;
		if (theListbox != NULL) theListbox->InsertString(0,s.c_str());
	}
	return;
}

/***************************************************************/

void ComicMaker::ParseTokens(string &OutPage, time_t theTime)
{
	theTime -= 18000;  //put into US Eastern Time - strips don't pop up at midnight UTC
	struct tm today;
	gmtime_s(&today, &theTime);

	char temp[TempArraySize];
	DATE_TOKEN currentDateToken;
	URL_TOKEN_LIST currentURLTokenList;
	ifstream Reader(IniFileName);

	//Run through the date based codes.  Easy and fast, so do them first.
	//If the token starts with SUNDAY, only replace it on Sundays - otherwise
	//remove it.
	while (GetDateToken(Reader,currentDateToken,temp))
	{
		ProcessADateBasedToken(OutPage, currentDateToken, today);
	}

	//Now for the hard ones.  These can't be computed.  We
	//actually have to fetch a web page and parse out each token.
	while (GetURLTokenList(Reader,currentURLTokenList,temp))
	{
		ProcessAURLBasedToken(OutPage, currentURLTokenList);
	};

	return;
}

/***************************************************************/

bool ComicMaker::GetDateToken(ifstream &Reader, DATE_TOKEN &token, char* temp)
{
	Reader.getline(temp,TempArraySize,MidSeperator);
	if (strlen(temp) == 0)
		return(false);
	token.Name = temp;
	if (token.Name == "URL")
	{
		Reader.getline(temp,TempArraySize,EndSeparator); // strip the new line character
		return(false);
	}

	//Read the number of days ago
	Reader.getline(temp,TempArraySize,MidSeperator);
	token.NumDaysAgo = atoi(temp);

	Reader.getline(temp,TempArraySize,EndSeparator);
	if (strlen(temp) == 0)
		return(false);
	token.Value = temp;

	return(true);
}

/***************************************************************/

void ComicMaker::WipeToken(URL_TOKEN& theToken)
{
	theToken.Name.clear();
	theToken.Domain.clear();
	theToken.Path.clear();
	theToken.ParseToken.clear();
	return;
}

/***************************************************************/

bool ComicMaker::GetURLTokenList(ifstream &Reader, URL_TOKEN_LIST &tokenlist, char* temp)
{
	//First, wipe out the current token list.
	tokenlist.UseList = false;
	WipeToken(tokenlist.First);
	WipeToken(tokenlist.Second);
	WipeToken(tokenlist.Third);

	if (!GetOneURLToken(Reader, tokenlist.First, temp))
		return(false);

	DWORD Loc = tokenlist.First.Name.find(FirstToken);
	//We got a token.  Is there only one, or are there more?
	if (Loc != std::string::npos)
	{
		tokenlist.First.Name = tokenlist.First.Name.substr(0, Loc);
		tokenlist.UseList = true;
		if (!GetOneURLToken(Reader, tokenlist.Second, temp))
		{
			return(false);
		}
//		Loc = tokenlist.Second.Name.find(SecondToken);
//		tokenlist.Second.Name = tokenlist.Second.Name.substr(0, Loc);

		if (!GetOneURLToken(Reader, tokenlist.Third, temp))
		{
			return(false);
		}
//		Loc = tokenlist.Third.Name.find(ThirdToken);
//		tokenlist.Third.Name = tokenlist.Third.Name.substr(0, Loc);

	}
	return(true);
}

/***************************************************************/

bool ComicMaker::GetOneURLToken(ifstream &Reader, URL_TOKEN &token, char* temp)
{
	Reader.getline(temp,TempArraySize,MidSeperator);
	if (strlen(temp) == 0)
		return(false);
	token.Name = temp;

	Reader.getline(temp,TempArraySize,MidSeperator);
	if (strlen(temp) == 0)
		return(false);
	token.Domain = temp;

	Reader.getline(temp,TempArraySize,MidSeperator);
	if (strlen(temp) == 0)
		return(false);
	token.Path = temp;

	Reader.getline(temp,TempArraySize,EndSeparator);
	if (strlen(temp) == 0)
		return(false);
	token.ParseToken = temp;

	return(true);
}

/***************************************************************/

bool ComicMaker::CheckForUpdates(bool& iniUpdates, bool& progUpdates, bool& updaterUpdate,
								 int& iniVersion, int& progVersion, int& UpdateVersion)
{
	string PageToRead, Version;
	int FileProgram, Filedata, FileUpdater;
	iniUpdates = false;
	progUpdates = false;
	updaterUpdate = false;

	//Get the version numbers of the stuff on the web
	PageFetcher->GetWebPage("www.evan.org","/Software/comicversion.htm",PageToRead);

	if (PageToRead.empty())
	{
		return(false);
	}


	ParseWebPage(PageToRead,"program",Version);
	progVersion = atoi(Version.c_str());
	ParseWebPage(PageToRead,"data",Version);
	iniVersion = atoi(Version.c_str());
	ParseWebPage(PageToRead,"updater",Version);
	UpdateVersion = atoi(Version.c_str());
	
	//Get the version numbers of the stuff currently being run
	ReadInFile(VersionFileName,PageToRead);
	if (PageToRead.empty())
	{
		return(false);
	}

	ParseWebPage(PageToRead,"program",Version);
	FileProgram = atoi(Version.c_str());
	ParseWebPage(PageToRead,"data",Version);
	Filedata = atoi(Version.c_str());
	ParseWebPage(PageToRead,"updater",Version);
	FileUpdater = atoi(Version.c_str());
	
	//Compare versions. 
	if (progVersion > FileProgram)
	{
		progUpdates = true;
	}
	if (iniVersion > Filedata)
	{
		iniUpdates = true;
	}
	if (UpdateVersion > FileUpdater)
	{
		updaterUpdate = true;
	}
	
	return(iniUpdates || progUpdates || updaterUpdate);
}

/***************************************************************/

void ComicMaker::UpdateTheUpdater(const int iniVersion, const int progVersion, const int UpdateVersion)
{
	if (AfxMessageBox("The updater program needs to be updated.  Press \"YES\" to update it now or \"NO\" to leave it alone.",
		MB_YESNO) == IDYES)
	{
		std::string FileName = "ComicUpdater.exe";
		std::string WebFilePath;
		WebFilePath = "/Software/files/";
		WebFilePath += FileName;
		
		PageFetcher->WriteWebPage("www.evan.org",WebFilePath.c_str(),"temp.file");
		remove(FileName.c_str());
		rename("temp.file",FileName.c_str());


		//Write a new version file
		WriteVersionFile(progVersion, iniVersion, UpdateVersion);
	}

	return;
}

/***************************************************************/

bool ComicMaker::CheckForUpdates(std::string &OutPage, time_t theTime)
{	
	//If it's Monday, check for updates on the web.
	tm MondayCheck;
	localtime_s(&MondayCheck, &theTime);

	bool updates = false;
	bool updatesToRun = false;
	string UpdateInfo = "";

	bool ini = false;
	bool prog = false;
	bool upd = false;	
	if (MondayCheck.tm_wday == 1)
	{
		int prog_version,ini_version,updater_version;
		updates = CheckForUpdates(ini, prog, upd, ini_version, prog_version, updater_version);
		if (upd)
		{
			//The updating program itself needs updating.  Get permission to update it and then do it.
			UpdateTheUpdater(ini_version, prog_version, updater_version);
		}
		
		if (ini || prog)
		{
			UpdateInfo += "New updates are available.  A update program should launch automatically, if not you can either launch it from the start menu or go to <a href = \"http://www.evan.org/Software/Comics.html\">this web page</a> to get the downloads yourself.  If you do that, don't forget to run Comic Setup afterwards to incorporate the updates.<p>";
		}
	}	

	//See some updates have been downloaded, but ComicSetup hasn't run to include them.
	ifstream UpdateCheck(UpdateFileName);
	if (UpdateCheck.is_open())
	{
		UpdateInfo += "<b>You have downloaded updates, but you need to run ComicSetup to install them!</b><p>";
		UpdateCheck.close();
		updatesToRun = true;
	}

	if (ini || prog || updatesToRun)
	{
		int Loc = 0;
		Loc = OutPage.find("<body>");
		if (Loc > 0)
		{
			Loc += 6;
			
			OutPage.insert(Loc,UpdateInfo);
		}
	}
	return(updates);
}

/***************************************************************/

//returns true if updates area available
bool ComicMaker::Runner(void)
{
	string OutPage;
	//What's today's date?
	time_t theTime;
	_tzset();  //set for local time zone
	time(&theTime);

	//Now read in the in file.
	if (!ReadInFile(ComicInFile, OutPage))
	{
		MessageBox(NULL,"Can't find the input file.  Make sure you ran the comic setup program to choose your strips before running this program to download them.","Error",MB_OK);
		return(false);
	}

	//Are any updates available?
	bool updates = CheckForUpdates(OutPage, theTime);
	
	//Adjust OutPage to the desired form.
	ParseTokens(OutPage, theTime);

	//Write it to file.
	WriteOutFile(OutPage, ComicOutFile);
	
	return(updates);
}

/***************************************************************/

bool ComicMaker::DoesFileExist(std::string strFullPath)
{
	if(strFullPath.empty())
        return false;

	HANDLE hFindFile;
	WIN32_FIND_DATA FindData;
	bool bReturn = false;

	hFindFile = FindFirstFile( strFullPath.c_str(), &FindData);
	if (hFindFile != INVALID_HANDLE_VALUE)
	{
		bReturn = true;
		FindClose(hFindFile);
	}
	return bReturn;
}

/***************************************************************/
