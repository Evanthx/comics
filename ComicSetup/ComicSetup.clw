; CLW file contains information for the MFC ClassWizard

[General Info]
Version=1
LastClass=CComicSetupDlg
LastTemplate=CDialog
NewFileInclude1=#include "stdafx.h"
NewFileInclude2=#include "ComicSetup.h"

ClassCount=2
Class1=CComicSetupApp
Class2=CComicSetupDlg

ResourceCount=4
Resource2=IDR_MAINFRAME
Resource3=IDD_COMICSETUP_DIALOG
Resource4=IDR_OptionsMenu

[CLS:CComicSetupApp]
Type=0
HeaderFile=ComicSetup.h
ImplementationFile=ComicSetup.cpp
Filter=N

[CLS:CComicSetupDlg]
Type=0
HeaderFile=ComicSetupDlg.h
ImplementationFile=ComicSetupDlg.cpp
Filter=D
LastObject=CComicSetupDlg
BaseClass=CDialog
VirtualFilter=dWC



[DLG:IDD_COMICSETUP_DIALOG]
Type=1
Class=CComicSetupDlg
ControlCount=9
Control1=IDOK,button,1342242817
Control2=IDCANCEL,button,1342242816
Control3=IDC_AllComics,listbox,1353778443
Control4=IDC_SelectedComics,listbox,1352729857
Control5=IDC_Select,button,1342242816
Control6=IDC_UnSelect,button,1342242816
Control7=IDC_STATIC,static,1342308865
Control8=IDC_STATIC,static,1342308865
Control9=IDC_TextBox,edit,1350633476

[MNU:IDR_OptionsMenu]
Type=1
Class=?
Command1=ID_OPTIONS_SELECTALL
Command2=ID_OPTIONS_UNSELECTALL
CommandCount=2

