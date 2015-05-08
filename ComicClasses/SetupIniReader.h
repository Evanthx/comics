#ifndef __SetupIniReader__
#define __SetupIniReader__

//Read the ini file into arrays.
#include <string>
#include <fstream>
#include <vector>

struct COMIC_INFO
{
	std::string Name;
	std::string URL;
	bool Subscribed;
	int OrderNumber;
};

class SetupIniFile
{
public:
	SetupIniFile(void);
	SetupIniFile(const char* FileName, bool MergeUpdates = false);
	~SetupIniFile(void);
	void Initialize(const char* FileName, bool MergeUpdates);
	COMIC_INFO* operator[](int i);
	int ListSize;
	void SortEntries(void);
	void SetNumber(int Number, const char* Name, const bool Subscribed);
	bool WriteFileOnExit;
	static void ReadFileIntoVector(const char* FileName, std::vector<COMIC_INFO> &vec, bool FlagErrorIfNoFile);

protected:
	static void ReadFileLineIntoString(std::ifstream &Reader, char* temp, const char &delim);
	std::vector<COMIC_INFO> ComicVector;
	std::vector<COMIC_INFO> UpdateVector;

	void UpdateIniFile(void);

	bool Unsorted;

};



#endif
