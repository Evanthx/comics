

#include "stdafx.h"
#include "SetupIniReader.h"
#include "Windows.h"
#include <map>
#include "CommonComicHeader.h"

using namespace std;

/*******************************************************/

SetupIniFile::SetupIniFile(void)
{
	Initialize(SetupIniFileName, true);
	return;
}

/*******************************************************/

SetupIniFile::SetupIniFile(const char* FileName, bool MergeUpdates)
{
	Initialize(FileName, MergeUpdates);
}

/*******************************************************/

void SetupIniFile::Initialize(const char* FileName, bool MergeUpdates)
{
	//Get the ini file
	ReadFileIntoVector(FileName,ComicVector, true);	

	//If an update file exists, get it too
	if (MergeUpdates)
	{
		ReadFileIntoVector(UpdateFileName,UpdateVector, false);
		if (UpdateVector.size() > 0)
		{	
			UpdateIniFile();
			int i = remove(UpdateFileName);
			string Err = "Merged updates into ";
			Err.append(SetupIniFileName);
			Err.append(".");

			MessageBox(NULL,Err.c_str(),"Success Message",MB_ICONINFORMATION);

			if (i == -1)
			{
				Err = "However, deletion of file ";
				Err.append(UpdateFileName);
				Err.append(" failed.  Please delete it manually.  Reported error is: ");
				Err.append(strerror(errno));
				MessageBox(NULL,Err.c_str(),"Error Message",MB_ICONERROR);
			}
		}
	}

	ListSize = ComicVector.size();
	WriteFileOnExit = false;
	Unsorted = false;
	
	return;
}

/*******************************************************/

SetupIniFile::~SetupIniFile(void)
{
	if (WriteFileOnExit == false)
	{
		return;
	}
	SortEntries();

	ofstream Writer(SetupIniFileName);
	if (!Writer)
	{
		MessageBox(NULL,"Can't open ini file to keep your changes!","Error Message",MB_ICONERROR);
		return;
	}
	//Need to write the vector to the ini file
	vector<COMIC_INFO>::iterator ComicIterator;
	for (ComicIterator = ComicVector.begin(); ComicIterator != ComicVector.end(); ComicIterator++)
	{
		Writer << ComicIterator->Name;
		Writer << MidSeperator;
		Writer << ComicIterator->URL;
		Writer << MidSeperator;			
		Writer << (ComicIterator->Subscribed?"Y":"N");	
		Writer << (ComicIterator->OrderNumber);	
		Writer << EndSeparator;
	}
	return;
}

/*******************************************************/

void SetupIniFile::ReadFileLineIntoString(ifstream &Reader, char* temp, const char &delim)
{
	Reader.getline(temp,TempArraySize,delim);
	//detect blank lines
	if (temp != NULL)
		if (temp[0] == '\n' || temp[0] == '\r')
			temp[0] = '\0';
	return;
}

/*******************************************************/

COMIC_INFO* SetupIniFile::operator[](int i)
{
	if (i >= ListSize)
		return(NULL);
	
	return(&(ComicVector[i]));
}

/*******************************************************/

void SetupIniFile::ReadFileIntoVector(const char* FileName, vector<COMIC_INFO> &vec, bool FlagErrorIfNoFile)
{
	ifstream Reader(FileName);
	if (!Reader.is_open())
	{	
		if (FlagErrorIfNoFile)
		{
			string Err;
			Err = "There is no ini file - the file ";
			Err.append(FileName);
			Err.append(" should exist in the same directory as ComicSetup.exe but was not found.");
			MessageBox(NULL,Err.c_str(),"Error Message",MB_ICONERROR);
			exit(0);
		}
		else
		{
			return;
		}
	}

	char temp[TempArraySize];
	COMIC_INFO CurrentComic;
	ReadFileLineIntoString(Reader,temp,MidSeperator);
	
	int CheckCounter = 10000;

	while (temp[0] != '\0')
	{
		//		CurrentComic = new COMIC_INFO;
		CurrentComic.Name = temp;
		
		ReadFileLineIntoString(Reader,temp,MidSeperator);
		CurrentComic.URL = temp;

		CurrentComic.OrderNumber = atoi(temp);
		ReadFileLineIntoString(Reader,temp,EndSeparator);

		if ((temp[0] == 'Y') || (temp[0] == 'y'))
		{
			CurrentComic.Subscribed = true;

			if (strlen(temp) == 1)
			{
				//no no length - the just make sure it's a unique number that counts up
				CurrentComic.OrderNumber = CheckCounter++;
			}
			else
			{
				CurrentComic.OrderNumber = atoi(temp+1);
			}
		}
		else
		{
			CurrentComic.Subscribed = false;

			if (strlen(temp) == 1)
			{
				//no no length - the just make sure it's a unique number that counts up
				CurrentComic.OrderNumber = CheckCounter++;
			}
			else
			{
				CurrentComic.OrderNumber = atoi(temp+1);
			}
		}

		//Put a copy of this into the vector
		vec.push_back(CurrentComic);
		ReadFileLineIntoString(Reader,temp,MidSeperator);
	}

	Reader.close();
	return;
}

/*******************************************************/

void SetupIniFile::UpdateIniFile(void)
{
	vector<COMIC_INFO>::iterator ComicIterator;
	vector<COMIC_INFO>::iterator UpdateIniFile;
	//for each element in the update file...
	COMIC_INFO ComicAdder;

	for (UpdateIniFile = UpdateVector.begin(); UpdateIniFile != UpdateVector.end(); UpdateIniFile++)
	{
		//find the matching element in the Ini file and update it
		for (ComicIterator = ComicVector.begin(); ComicIterator != ComicVector.end(); ComicIterator++)
		{
			if (ComicIterator->Name == UpdateIniFile->Name)
			{
				ComicIterator->URL = UpdateIniFile->URL;
				break;
			}
		}

		if (ComicIterator == ComicVector.end())
		{
			//if we got here, the new comic is unknown to us.  Add it.
			ComicAdder.Name = UpdateIniFile->Name;
			ComicAdder.Subscribed = UpdateIniFile->Subscribed;
			ComicAdder.URL = UpdateIniFile->URL;
			ComicVector.push_back(ComicAdder);
		}
	}

	//One more thing to deal with.  How do we go about removing a comic?  So around this time I 
	//decided the easiest thing to do would be to make update.txt a full ComicSetup.ini file that
	//just gets merged with the users ComicSetup.ini in such a way as to keep his or her settings.
	//So any comics NOT in update.txt need to get removed from ComicSetup.ini.
	//Not sure how to do that efficiently, so we just walk though ComicSetup vector and make sure
	//each entry is in the update vector.  If not, yank it.
	bool DeleteComic;

	//for each comic in the ini file...
	for (ComicIterator = ComicVector.begin(); ComicIterator != ComicVector.end(); )
	{
		DeleteComic = true;
		//find the comic in the update vector
		for (UpdateIniFile = UpdateVector.begin(); UpdateIniFile != UpdateVector.end(); UpdateIniFile++)
		{
			if (ComicIterator->Name == UpdateIniFile->Name)
			{
				DeleteComic = false;
				break;
			}
		}
		if (DeleteComic)
		{
			ComicVector.erase(ComicIterator);
		}
		else
		{
			ComicIterator++;
		}
	}

	return;
}

/*******************************************************/

void SetupIniFile::SortEntries(void)
{
	if (!Unsorted)
	{
		//We're already sorted, don't bother running though this routine.
		return;
	}

	map<string,COMIC_INFO> EntrySorter;
	vector<COMIC_INFO>::iterator ComicIterator;
	map<string,COMIC_INFO>::iterator EntrySorterIterator;
	pair<string,COMIC_INFO> SortItem;

	//For each comic in ComicVector -
	//	insert that comic entry into the map, where it will sort.
	for (ComicIterator = ComicVector.begin(); ComicIterator != ComicVector.end(); ComicIterator++)
	{
		char temp[25];
		sprintf_s(temp,25,"%05d",(*ComicIterator).OrderNumber);
//		itoa((*ComicIterator).OrderNumber, temp, 10);

		SortItem.first = temp;
		SortItem.second = (*ComicIterator);
		EntrySorter.insert(SortItem);
	}

	//All entries are copied to EntrySorter.  So now wipe ComicVector clean.
	ComicVector.clear();

	//and refill it with the sorted items in EntrySorter.
	for (EntrySorterIterator = EntrySorter.begin(); EntrySorterIterator != EntrySorter.end(); EntrySorterIterator++)
	{
		ComicVector.push_back((*EntrySorterIterator).second);
	}

	Unsorted = false;
	
	return;
}

/*******************************************************/

//Only called for subscribed files, so set that to true
void SetupIniFile::SetNumber(int Number, const char* Name, bool Subscribed)
{
	//Walk through ComicVector until we find the matching name, and set the number.
	vector<COMIC_INFO>::iterator ComicIterator;
	for (ComicIterator = ComicVector.begin(); ComicIterator != ComicVector.end(); ComicIterator++)
	{
		if (ComicIterator->Name == Name)
		{
			ComicIterator->OrderNumber = Number;
			ComicIterator->Subscribed = Subscribed;
			break;
		}
	}

	Unsorted = true;

	return;
}

/*******************************************************/
