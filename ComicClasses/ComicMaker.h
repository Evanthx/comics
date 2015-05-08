
#ifndef __ComicMaker__h__
#define __ComicMaker__h__

#include "HTTPGet.h"
#include <time.h>
#include <fstream>


class ComicMaker
{
protected:
	CListBox* theListbox;
	HTTPGet* PageFetcher;

public:
	struct DATE_TOKEN
	{
		std::string Name;
		int NumDaysAgo;
		std::string Value;
	};

	struct URL_TOKEN
	{
		std::string Name;
		std::string Domain;
		std::string Path;
		std::string ParseToken;
	};

	struct URL_TOKEN_LIST
	{
		bool UseList;
		URL_TOKEN First;
		URL_TOKEN Second;
		URL_TOKEN Third;
	};

	ComicMaker(CListBox* Listbox);
	~ComicMaker();

	bool Runner(void);

	bool CheckForUpdates(std::string &OutPage, time_t theTime);
	void ParseWebPage(const std::string &WebPage, std::string SearchString, std::string &ComicID);
	static bool ReadInFile(const char *FileToRead, std::string &OutPage);
	static bool WriteOutFile(const std::string& StringToWrite, const std::string& FileName);
	static void ParseSingleToken(std::string &OutPage, const char* Token, const char* NewVal);
	void ProcessADateBasedToken(std::string &OutPage, DATE_TOKEN& currentDateToken, const tm& today);
	void WipeToken(URL_TOKEN& theToken);
	void DownloadAToken(URL_TOKEN& currentURLToken, std::string& ComicID);
	bool ParseTheURL(const std::string& strURL, std::string& strServer, std::string& strObject);
	bool IsThisA404(const std::string& Server, const std::string& Path);  //return false if a URL returns a 404 error
	std::string PickToken(const std::string& FirstID, const std::string& SecondID, const std::string& ThirdID);
	void ProcessAURLBasedToken(std::string &OutPage, URL_TOKEN_LIST& currentURLTokenList);
	void ParseTokens(std::string &OutPage, time_t theTime);
	bool GetDateToken(std::ifstream &Reader, DATE_TOKEN &token, char* temp);
	bool GetURLTokenList(std::ifstream &Reader, URL_TOKEN_LIST &tokenlist, char* temp);
	bool GetOneURLToken(std::ifstream &Reader, URL_TOKEN &token, char* temp);
	bool CheckForUpdates(bool& iniUpdates, bool& progUpdates, bool& updaterUpdate,
		int& iniVersion, int& progVersion, int& UpdateVersion);
	void UpdateTheUpdater(const int iniVersion, const int progVersion, const int UpdateVersion);
	static bool DoesFileExist(std::string strFullPath);
	void WriteVersionFile(const int progVersion, const int iniVersion, const int UpdateVersion);
};

#endif