/////////////////////////////////////////////////////////////////////////////
//
// CTask.h: interface for the CTask class.
//


#if !defined(AFX_SCHEDULEDTASK_H__A00D015F_9061_11D2_8D3B_FC17F33B2809__INCLUDED_)
#define AFX_SCHEDULEDTASK_H__A00D015F_9061_11D2_8D3B_FC17F33B2809__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <afx.h>// dla CTime
#include <mstask.h>

class CTask  
{
// Construction
public:
	CTask();


// czestotliwosc
    enum ETaskFrequency 
        {
        freqUnset = 0,                  // zmienne uzyte w debuggerze
        freqFirst = 0,                  
        freqOnce,
        freqDaily,
        freqWeekly,
        freqMonthly,
        freqLast = freqMonthly          // dla debuggera
        };

    // Akcesory  
    BOOL  GetStartDateTime ( CTime& ) const;        // pobierz date i godzine startu
    void  SetStartDateTime ( const CTime& );        // ustaw date i godzine startu
    void  SetStartDateTime ( const SYSTEMTIME& );   // ustaw date i godzine startu

    BOOL  GetEndDate ( CTime& ) const;              // pobierz date i godzine konca zadania
    void  SetEndDate ( const CTime& );              // ustaw date i godzine konca zadania
    void  SetEndDate ( const SYSTEMTIME& );         // ustaw date i godzine konca zadania

    ETaskFrequency GetFrequency() const;            // pobierz czestotliwosc
    void           SetFrequency ( ETaskFrequency ); // ustaw czestotliwosc
    
    CString GetProgram() const;                     // pobierz program do urachamiania
    void    SetProgram ( LPCTSTR szProgram );       // ustaw program do urachamiania

    CString GetParameters() const;                  // pobierz parametry programu
    void    SetParameters ( LPCTSTR szParams );     // ustaw parametry programu

    CString GetStartingDir() const;                 // pobierz katalog poczatkowy programu
    void    SetStartingDir ( LPCTSTR szDir );       // ustaw katalog poczatkowy programu

    CString GetAccountName() const;                 // pobierz nazwe uzytkownika
    void    SetAccountName ( LPCTSTR szAccount );   // ustaw nazwe uzytkownika
    
    void    SetPassword ( LPCTSTR szPassword );     // ustaw haslo

    CString GetComment() const;                     // pobierz komentarz
    void    SetComment ( LPCTSTR szComment );       // ustaw komentarz

    // zmienne czlonkowskie
protected:
    SYSTEMTIME m_timeStart;
    SYSTEMTIME m_timeEnd;

    ETaskFrequency m_eFreq;

    CString    m_sProgramPath;
    CString    m_sParameters;
    CString    m_sStartingDir;
    CString    m_sAccount;
    CString    m_sPassword;
    CString    m_sComment;

// operacje
public:
    void Reset();   // ustaw zmienne na zero

    HRESULT SaveTask ( LPCTSTR szTaskName, 
                       BOOL bFailIfExists = FALSE ) const;  // Dodaj zadanie do harmonogramu
    
    static HRESULT DeleteTask ( LPCTSTR szTaskName );   // Usun zadanie z harmonogramu


// Implementation
public:
	virtual ~CTask();

protected:
    WORD GetDayOfWeekFlag ( const CTime& ) const;
    BOOL m_bNT;
};

#endif // !defined(AFX_SCHEDULEDTASK_H__A00D015F_9061_11D2_8D3B_FC17F33B2809__INCLUDED_)
