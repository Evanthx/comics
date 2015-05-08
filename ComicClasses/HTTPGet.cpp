
#include "stdafx.h"
#include "HTTPGet.h"
#include <wininet.h>
#include "CriticalSection.h"

HTTPGet* HTTPGet::g_instance = NULL;

CriticalSection _section;

//using namespace std;

/******************************************************/

HTTPGet::HTTPGet()
{
	InitializeWinsock();
}

/******************************************************/

HTTPGet::~HTTPGet()
{
	WSACleanup();
}

/******************************************************/

HTTPGet* HTTPGet::getInstance()
{
	//This is a singleton class.  This method hands out the singleton pointer.
	if (HTTPGet::g_instance == NULL)
	{
		HTTPGet::g_instance = new HTTPGet();
	}
	return HTTPGet::g_instance;
}

/******************************************************/

void HTTPGet::PrintError(std::string s, bool SocketError)
{
	s.append(":  ");
	s.append(strerror(errno));

	if (SocketError)
	{
		char ErrRet[100];
		sprintf_s(ErrRet,100," and WSAGetLastError returns %d",WSAGetLastError());
		s.append(ErrRet); 
	}
	MessageBox(NULL,s.c_str(),"Error Message",MB_OK);


	return;
}

/******************************************************/

void HTTPGet::InitializeWinsock()
{
	WORD wVersionRequested = MAKEWORD(2,0);
	WSADATA wsaData;
	int nRet;
	//
	// Initialize WinSock.dll
	//

	nRet = WSAStartup(wVersionRequested, &wsaData);
	if (nRet)
	{
		PrintError("Error in WSAStartup",false);
		WSACleanup();
		exit(1);
		return;
	}

	
	//
	// Check WinSock version
	//
	if (wsaData.wVersion != wVersionRequested)
	{
		PrintError("You need to have winsock 2.0 to run this program...sorry about that.  You can get it for free - see the readme file for information.",false);
		WSACleanup();
		exit(1);
		return;
	}
	
	return;
}

/******************************************************/

//Server name is in lpServerName
//File to get on that server is in lpFileName
//A string to store that file is in WebPage

void HTTPGet::GetWebPage(const char* lpServerName, const char* lpFileName, std::string &WebPage)
{
	WebWork(lpServerName, lpFileName, "", WebPage, false);
	return;
}

/******************************************************/

//Server name is in lpServerName
//File to get on that server is in lpFileName
//A string to store that file is in WebPage

void HTTPGet::WriteWebPage(const char* lpServerName, const char* lpFileName, const std::string FileOnDisk)
{
	std::string WebPage;
	WebWork(lpServerName, lpFileName, FileOnDisk, WebPage, true);
	return;
}
	
/******************************************************/

DWORD HTTPGet::GetStatusCode(const char* lpServerName, const char* lpFileName)
{
	DWORD StatusCode = 0;
	std::string TempStr;
	WebWork(lpServerName, lpFileName, TempStr, TempStr, false, true);
	StatusCode = atoi(TempStr.c_str());
	return(StatusCode);
}

/******************************************************/

//if WriteToDisk is true, then write to a file.  Otherwise,
//write to the string in WebPage.
void HTTPGet::WebWork(const char* lpServerName, const char* lpFileName, 
		const std::string FileOnDisk, std::string &WebPage,
		bool WriteToDisk, bool PutStatusCodeInWebPageInputString)
{
	_section.Enter();  //Only download one file at a time.
	FILE *FileWriter;

	if (WriteToDisk)
	{
		fopen_s(&FileWriter, FileOnDisk.c_str(),"w+b");
		if (FileWriter == NULL)
		{
			MessageBox(NULL,"Can't create file to hold downloads.","Error",MB_OK);
			return;
		}
	}
	
	HINTERNET InternetSession;
	HINTERNET HttpSession;
	HINTERNET FileSession;

	InternetSession = InternetOpen("", INTERNET_OPEN_TYPE_PRECONFIG, NULL, NULL, 0) ;
	
	if (!InternetSession)
	{
		//can't connect
		return;
	}
		 
	// Opening the Url and getting a Handle for HTTP file	
	HttpSession = InternetConnect(InternetSession, lpServerName, INTERNET_DEFAULT_HTTP_PORT, "", "", INTERNET_SERVICE_HTTP, 0, 0);
	if (!HttpSession)
	{
		//can't connect
		return;
	}
	
	std::string Referrer = "";
	FileSession = (HINTERNET)HttpOpenRequest( HttpSession, NULL, lpFileName, 
													NULL, Referrer.c_str(), NULL, INTERNET_FLAG_PRAGMA_NOCACHE, 0);
	if (!FileSession)
	{
		return;
		//can't connect
	}

	BOOL RetVal;
	RetVal = HttpSendRequest( FileSession, NULL, 0, NULL, 0);
	if( !RetVal)
	{
		//can't connect
		InternetCloseHandle(FileSession);
		return;
	}

	// Get the size of HTTP Files
	char SizeBuffer[32];
	DWORD LengthSizeBuffer = 32;
	RetVal = HttpQueryInfo(FileSession, HTTP_QUERY_STATUS_CODE, SizeBuffer, &LengthSizeBuffer, NULL);
	
	if (PutStatusCodeInWebPageInputString)
	{
		//This is a bit of awkwardness I stuck in here to let me just get the status code (404, 200, whatever)
		//without downloading the file.  If I get some time I'll split this method up right here
		//to be cleaner ... as is this works, but confuses the interface.
		//TODO split method apart here	
		WebPage = SizeBuffer;
		InternetCloseHandle(FileSession);
		return;
	}

	RetVal = HttpQueryInfo(FileSession, HTTP_QUERY_STATUS_CODE, SizeBuffer, &LengthSizeBuffer, NULL);
	if (strncmp(SizeBuffer, "200", 3))
	{
		InternetCloseHandle(FileSession);
		if (WriteToDisk)
		{
			fclose(FileWriter);
		}
		_unlink(FileOnDisk.c_str());
		return;
	}

	RetVal = HttpQueryInfo(FileSession, HTTP_QUERY_CONTENT_LENGTH, SizeBuffer, &LengthSizeBuffer, NULL) ;

	bool SizeKnown = true;
			
	if (!RetVal)
	{
		SizeKnown = false;
	}


	DWORD FileSize=1;
	DWORD TotalBytesRead = 0;
	const DWORD OrginalChunkSize = 5000;
	DWORD ChunkSize = OrginalChunkSize;
	DWORD BytesRead = 0;

	if (SizeKnown)
	{
		FileSize=atol(SizeBuffer);
	}


	char ChunkString[OrginalChunkSize];

	bool StillReading = true;

	while(StillReading)
	{
		if (SizeKnown)
		{
			if( ChunkSize > FileSize)
				ChunkSize = FileSize;
		}
		
		if (!(InternetReadFile( FileSession, ChunkString, ChunkSize, &BytesRead)))
		{
			DWORD ErrVal = GetLastError();
			std::string temp;
			temp.resize (4096);			
			FormatMessage (FORMAT_MESSAGE_FROM_SYSTEM, NULL, ErrVal, 0, (LPSTR)temp.c_str(),
				temp.size(), NULL);
			if (WriteToDisk)
			{
				fclose(FileWriter);
			}
			return;
		}
		TotalBytesRead += BytesRead;

		if (BytesRead == 0)
		{
			//finished reading the file at this point
			ChunkString[BytesRead] = '\0';
			StillReading = false;
		}

		if (WriteToDisk)
		{
			//Write ChunkString to FileWriter
			fwrite(ChunkString,BytesRead,1,FileWriter);
		}
		else
		{
			WebPage += ChunkString;
		}

	}

	InternetCloseHandle(FileSession);

	if (WriteToDisk)
	{
		fclose(FileWriter);
	}

	return;
}