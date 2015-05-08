
#ifndef __HTTPGet__
#define __HTTPGet__

#include <string>  
#include <winsock2.h>



//Just a short class that fetches a file.  Could have been just a method
//but I figured I'd make it a class...
class HTTPGet
{
public:
	static HTTPGet* getInstance();
	virtual ~HTTPGet(void);

	void PrintError(std::string s, bool SocketError);


	//Server name is in lpServerName
	//File to get on that server is in lpFileName
	//A string to store that file is in WebPage
	void GetWebPage(const char* lpServerName, const char* lpFileName, std::string &WebPage);

	//Same thing, but writes to a file instead of into a string
	void WriteWebPage(const char* lpServerName, const char* lpFileName, const std::string FileOnDisk);

	//Same thing again, but only get the status code (200, 404, etc), don't download the file
	DWORD GetStatusCode(const char* lpServerName, const char* lpFileName);

	//See if updates are available
	bool CheckForUpdates(std::string &OutPage, time_t &theTime, bool& iniUpdates, bool& progUpdates);

protected:
	static HTTPGet* g_instance;
	void HTTPGet::InitializeWinsock();

	void WebWork(const char* lpServerName, const char* lpFileName, 
		const std::string FileOnDisk, std::string &WebPage,
		bool WriteToDisk, bool PutStatusCodeInWebPageInputString = false);

private:
	HTTPGet();

};

#endif