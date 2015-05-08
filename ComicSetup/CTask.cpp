//////////////////////////////////////////////////////////////////////
//
// CTask.cpp: implementation of the CTask class.
//


#include "stdafx.h"
#include "CTask.h"
#include <afxconv.h>    // Dla  makr konwertujacych ANSI->Unicode

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

#pragma comment(lib, "mstask.lib")

//////////////////////////////////////////////////////////////////////
// makra dla wygody

#define DDS_CST_EVERYMONTH \
                        TASK_JANUARY  | TASK_FEBRUARY | TASK_MARCH     | \
                        TASK_APRIL    | TASK_MAY      | TASK_JUNE      | \
                        TASK_JULY     | TASK_AUGUST   | TASK_SEPTEMBER | \
                        TASK_OCTOBER  | TASK_NOVEMBER | TASK_DECEMBER  


//////////////////////////////////////////////////////////////////////
// konstruktor i destruktor

CTask::CTask()
{
    m_bNT = (0 == (GetVersion() & 0x80000000));
    Reset();
}

CTask::~CTask()
{
}


//////////////////////////////////////////////////////////////////////
// Akcesory

//////////////////////////////////////////////////////////////////////////
//
// Funkcja:    GetStartDateTime()
//
// Opis:
//  Wartosc zwracajaca date startu
//
// Parametry wejsciowe:
//  time: [out] obiekt CTime, ktory otrzyma date poczatkowa jesli
//  zostala ustawiona 
//
// Wartosc zwracana:
//  TRUE, jesli czas zostal ustawiony, FALSE jesli nie zostal ustawiony
//
//////////////////////////////////////////////////////////////////////////

BOOL CTask::GetStartDateTime ( CTime& time ) const
{
BOOL bRet = FALSE;

    if ( 0 != m_timeStart.wYear )
        {
        time = m_timeStart;
        bRet = TRUE;
        }
    else
        {
        TRACE0("CTask::GetStartDateTime() - no start time set.\n");
        }

    return bRet;
}


//////////////////////////////////////////////////////////////////////////
//
// Funkcja:    SetStartDateTime()
//
// Opis:
//  Ustawia date i godzine rozpoczecia zadania
//
// Parametry wejsciowe:
//  time: [in] Obiekt CTime object przechowuj¹cy date poczatkowa
//  -lub-
//        [in] struktura SYSTEMTIME przechowuj¹ca date poczatkowa.
//
// Wartosc zwracana:
//  Brak.  
//
// Uwagi:
//  Sekundy i milisekundy nie sa brane pod uwage
//
//////////////////////////////////////////////////////////////////////////

void CTask::SetStartDateTime ( const CTime& time )
{
    time.GetAsSystemTime ( m_timeStart );

    m_timeStart.wSecond = 0;
    m_timeStart.wMilliseconds = 0;
}

void CTask::SetStartDateTime ( const SYSTEMTIME& time )
{
    m_timeStart = time;

    m_timeStart.wSecond = 0;
    m_timeStart.wMilliseconds = 0;
}


//////////////////////////////////////////////////////////////////////////
//
// Funkcja:    GetEndDate()
//
// Opis:
//  Zwraca date konca wykonywania zadania.
//
// Parametry wejsciowe:
//  time: [out] CTime object, ktory otrzyma date konca wykonywania zadania.
//
// Wartosc zwracana:
//  TRUE, jesli data zakonczenia zostala ustawiona, FALSE jesli nie.
//
// Uwagi:
//   Tylko pola daty zostaja wypelnione
//
//////////////////////////////////////////////////////////////////////////

BOOL CTask::GetEndDate ( CTime& time ) const
{
BOOL bRet = FALSE;

    if ( 0 != m_timeEnd.wYear )
        {
        time = m_timeEnd;
        bRet = TRUE;
        }
    else
        {
        TRACE0("CTask::GetEndDateTime() - no end time set.\n");
        }

    return bRet;
}


//////////////////////////////////////////////////////////////////////////
//
// Funkcja:    SetEndDate()
//
// Opis:
//  Ustawia date zakonczenia zadania
//
// Parametry wejsciowe:
//  time: [in] obiekt CTime 
//  -lub-
//  time: [in] struktura SYSTEMTIME 
//
// Wartosc zwracana:
//  Brak.
//
//////////////////////////////////////////////////////////////////////////

void CTask::SetEndDate ( const CTime& time )
{
    ZeroMemory ( &m_timeEnd, sizeof(SYSTEMTIME) );

    m_timeEnd.wYear  = time.GetYear();
    m_timeEnd.wMonth = time.GetMonth();
    m_timeEnd.wDay   = time.GetDay();
}

void CTask::SetEndDate ( const SYSTEMTIME& time )
{
    ZeroMemory ( &m_timeEnd, sizeof(SYSTEMTIME) );

    m_timeEnd.wYear  = time.wYear;
    m_timeEnd.wMonth = time.wMonth;
    m_timeEnd.wDay   = time.wDay;
}


//////////////////////////////////////////////////////////////////////////
//
// Funkcja:    GetFrequency()
//
// Opis:
//  Zwraca czestotliwosc wykonywania zadania.
//
// Parametry wejsciowe:
//  Brak.
//
// Wartosc zwracana:
//  Czestotliwosc zadania.  
//
//////////////////////////////////////////////////////////////////////////

CTask::ETaskFrequency CTask::GetFrequency() const
{
    ASSERT ( m_eFreq >= freqFirst  &&  m_eFreq <= freqLast );

    return m_eFreq;
}


//////////////////////////////////////////////////////////////////////////
//
// Funkcja:    SetFrequency()
//
// Opis:
//  Ustawia czestotliwosc wykonywania zadania
//
// Parametry wejsciowe:
//  freq: [in] czstotliwosc zadania.
//
// Wartosc zwracana:
//  Brak.
//
//////////////////////////////////////////////////////////////////////////

void CTask::SetFrequency  ( CTask::ETaskFrequency freq )
{
    ASSERT ( freq >= freqFirst  &&  freq <= freqLast );

    m_eFreq = freq;
}


//////////////////////////////////////////////////////////////////////////
//
// Funkcja:    GetProgram()
//
// Opis:
//  Zwraca sciezke do programu wykonywanego jako zadanie.
//
// Parametry wejsciowe:
//  Brak.
//
// Wartosc zwracana:
//  Pelna sciezka do wykonywanego programu lub pusty lancuch jesli
//zmienna nie zostala ustawiona
//
//////////////////////////////////////////////////////////////////////////

CString CTask::GetProgram() const
{
    return m_sProgramPath;
}


//////////////////////////////////////////////////////////////////////////
//
// Funkcja:    SetProgram()
//
// Opis:
//  Ustawia program ktory ma byc wykonywany
//
// Parametry wejsciowe:
//  szProgram: [in] Pelna sciezka do programu wykonywanego
//
// Wartosc zwracana:
//  Brak.
//
//////////////////////////////////////////////////////////////////////////

void CTask::SetProgram ( LPCTSTR szProgram )
{
    ASSERT ( AfxIsValidString ( szProgram ));

#ifdef _DEBUG
    if ( 0xFFFFFFFF == ::GetFileAttributes ( szProgram ) )
        {
        TRACE1("CTask::SetProgram() - warning, program <%s> not found.\n",
               szProgram );
        }
#endif

    m_sProgramPath = szProgram;
}


//////////////////////////////////////////////////////////////////////////
//
// Funkcja:    GetParameters()
//
// Opis:
//  Zwraca parametry wykonania dla zadania
//
// Parametry wejsciowe:
//  Brak.
//
// Wartosc zwracana:
//  Parametry dla zadania
//
//////////////////////////////////////////////////////////////////////////

CString CTask::GetParameters() const
{
    return m_sParameters;
}


//////////////////////////////////////////////////////////////////////////
//
// Funkcja:    SetParameters()
//
// Opis:
//  Ustawia parametry dla zadania
//
// Parametry wejsciowe:
//  szParameters: [in] Parametry dla zadania.
//
// Wartosc zwracana:
//  Brak.
//
//////////////////////////////////////////////////////////////////////////

void CTask::SetParameters ( LPCTSTR szParameters )
{
    ASSERT ( AfxIsValidString ( szParameters ));

    m_sParameters = szParameters;
}


//////////////////////////////////////////////////////////////////////////
//
// Funkcja:    GetStartingDir()
//
// Opis:
//  Zwraca folder startowy dla zadania
//
// Parametry wejsciowe:
//  Brak
//
// Wartosc zwracana:
//  Katalog startowy
//
//////////////////////////////////////////////////////////////////////////

CString CTask::GetStartingDir() const
{
    return m_sStartingDir;
}


//////////////////////////////////////////////////////////////////////////
//
// Funkcja:    SetStartingDir()
//
// Opis:
// Ustawia katalog startowy dla zadania 
//
// Parametry wejsciowe:
//  szStartingDir: [in] Katalog startowy
//
// Wartosc zwracana:
//  Brak
//
//////////////////////////////////////////////////////////////////////////

void CTask::SetStartingDir ( LPCTSTR szStartingDir )
{
    ASSERT ( AfxIsValidString ( szStartingDir ));

    m_sStartingDir = szStartingDir;
}


//////////////////////////////////////////////////////////////////////////
//
// Funkcja:    GetAccountName
//
// Opis:
// Zwraca nazwe uzytkownika pod ktora bedzie dzialalo zadanie 
//
// Parametry wejsciowe:
//  Brak
//
// Wartosc zwracana:
//  Nazwa uzytkownika. Pod Win9x funkcja zwraca pusty lancuch.
//
//////////////////////////////////////////////////////////////////////////

CString CTask::GetAccountName() const
{
    return m_sAccount;
}


//////////////////////////////////////////////////////////////////////////
//
// Funkcja:    SetAccountName
//
// Opis:
//  Ustawia nazwe uzytkownika.
//
// Parametry wejsciowe:
//  szAccount: [in] nazwa uzytkownika
//
// Wartosc zwracana:
//  Brak
//
// Uwagi:
//  Funkcja nie odnosi zadnego efektu pod Win9x.
//
//////////////////////////////////////////////////////////////////////////

void CTask::SetAccountName ( LPCTSTR szAccount )
{
    ASSERT ( AfxIsValidString ( szAccount ));

    m_sAccount = szAccount;
}


//////////////////////////////////////////////////////////////////////////
//
// Funkcja:    SetPassword
//
// Opis:
//  Ustawia haslo dla uzytkownika ustawionego w SetAccount().
//
// Parametry wejsciowe:
//  szPassword: [in] haslo
//
// Wartosc zwracana:
//  Brak.
//
// Uwagi:
//  Funkcji GetPassword() nie ma, bo Task Scheduler nie udostepnia 
// pozyskiwania hasla
// Bez efektu pod Win9x.
//
//////////////////////////////////////////////////////////////////////////

void CTask::SetPassword ( LPCTSTR szPassword )
{
    ASSERT ( AfxIsValidString ( szPassword ));

    m_sPassword = szPassword;
}


//////////////////////////////////////////////////////////////////////////
//
// Funkcja:    GetComment()
//
// Opis:
//  Funkcja zwraca komentarz jaki bedzie wyswietlany w aplecie 
// Harmonogramu Zadan dla danego zadania
//
// Parametry wejsciowe:
//  Brak.
//
// Wartosc zwracana:
//  Komentarz dla danego zadania
//
//////////////////////////////////////////////////////////////////////////

CString CTask::GetComment() const
{
    return m_sComment;
}


//////////////////////////////////////////////////////////////////////////
//
// Funkcja:    SetComment()
//
// Opis:
// Ustawia komentarz jaki bedzie wyswietlany przez harmonogram zadan
// dla danego zadania 
//
// Parametry wejsciowe:
//  szComment: [in] Komentarz
//
// Wartosc zwracana:
//  Brak
//
//////////////////////////////////////////////////////////////////////////

void CTask::SetComment ( LPCTSTR szComment )
{
    ASSERT ( AfxIsValidString ( szComment ));

    m_sComment = szComment;
}


//////////////////////////////////////////////////////////////////////////
// Pozostale funkcje

//////////////////////////////////////////////////////////////////////////
//
// Funkcja:    Reset()
//
// Opis:
//  Ustawia wszystkie dane obiektu na zero.
//
// Parametry wejsciowe:
//  Brak.
//
// Wartosc zwracana:
//  Brak.
//
//////////////////////////////////////////////////////////////////////////

void CTask::Reset()
{
    ZeroMemory ( &m_timeStart, sizeof(SYSTEMTIME) );
    ZeroMemory ( &m_timeEnd, sizeof(SYSTEMTIME) );

    m_eFreq = freqUnset;

    m_sProgramPath.Empty();
    m_sParameters.Empty();
    m_sStartingDir.Empty();
    m_sComment.Empty();
}


//////////////////////////////////////////////////////////////////////////
//
// Funkcja:    SaveTask()
//
// Opis:
//  Dodaje zadanie do harmonogramu zadañ
//
// Parametry wejsciowe:
//  szTaskName: [in] Nazwa zadania.  Liczba znakow musi byc mniejsza
// niz MAX_PATH
//  bFailIfExists: [in] funkcja zakonczy sie niepowodzeniem,
// jesli podane zadanie juz istnieje
// Wartosc zwracana:
// S_OK if wywolanie funkcji zakonczy sie powodzeniem, 
// E_FAIL jesli nazwa progamu lub zadania nie zostaly ustawione,
// w innym przypadku kod bledu OLE

HRESULT CTask::SaveTask ( LPCTSTR szTaskName,
                                   BOOL    bFailIfExists /*=FALSE*/ ) const
{
HRESULT         hr;
ITaskScheduler* pISched    = NULL;
IUnknown*       pIUnk      = NULL;
IPersistFile*   pIFile     = NULL;
ITask*          pITask     = NULL;
ITaskTrigger*   pITaskTrig = NULL;
TASK_TRIGGER    rTrigger;
DWORD           dwTaskFlags;
WORD            wTrigNumber;

    USES_CONVERSION;

    ASSERT ( AfxIsValidString ( szTaskName ));
    // Sciezka programu nie moze byc pusta, ale inne moga

    if ( 0 == m_timeStart.wYear      ||
         NULL == szTaskName          ||
         0 == lstrlen ( szTaskName ) ||
         m_sProgramPath.GetLength() < 1 )
        {
        return E_FAIL;
        }


    // Pobieramy interface IScheduler
    hr = ::CoCreateInstance (
               CLSID_CTaskScheduler,
               NULL,
               CLSCTX_INPROC_SERVER,
               IID_ITaskScheduler,
               (void **) &pISched );


    if ( FAILED(hr) )
        {
#ifdef _DEBUG
        TRACE1("CTask::SaveTask() - failed to create a task scheduler interface. Return = 0x%08X\n",
               (DWORD) hr );

	// Harmonogram zadan nie zostal zainstalowany lub zostal odinstalowany
        if ( REGDB_E_CLASSNOTREG == hr )
            {
            TRACE0("    The error was REGDB_E_CLASSNOTREG, meaning you don't have the scheduler installed.\n"
                _T("    If you are running 95 or NT 4 with IE 4, you must install the task scheduler from the\n")
                _T("    IE components install page on MS's web site or the IE CD.\n")
                _T("    If you're on 98, NT 5, or 95/NT 4 with IE 5, then something's wrong with your install\n")
                _T("    because the scheduler should always be present.\n")
                _T("    Note that this class does *NOT* work with the \"AT\" service, which is the default\n")
                _T("    scheduler on NT 4 and earlier.\n") );
            }
#endif  // _DEBUG

        return hr;
        }


__try
    {
    

//sprawdz czy zadanie o podanek przez uzytkownika nazwie istnieje
    hr = pISched->Activate ( T2COLE ( szTaskName ), IID_ITask, &pIUnk );

    if ( SUCCEEDED(hr) )
        {
        
	//zadanie o podanej nazwie juz istnieje, sprawdzmy bFailIfExists.

        pIUnk->Release();               // zwalniamy interfejs
        pIUnk = NULL;

        if ( bFailIfExists )
            {
            TRACE0("CTask::SaveTask() - A task with the given name already exists; failing.\n");
            return HRESULT_FROM_WIN32 ( ERROR_FILE_EXISTS );
            }
        else
            {
           	//probujemy usunac istniejace zadanie, by zrobic miejsce dla naszego.
            TRACE0("CTask::SaveTask() - A task with the given name already exists; deleting it.\n");

            hr = CTask::DeleteTask ( szTaskName );

            if ( FAILED(hr) )
                {
                TRACE1("CTask::SaveTask() - couldn't delete existing task! Bailing out. Return = 0x%08X\n",
                       (DWORD) hr );
                return hr;
                }
            }
        }


    // tworzymy nowe zadanie.
    hr = pISched->NewWorkItem ( T2COLE ( szTaskName ), CLSID_CTask,
                                IID_ITask, &pIUnk );

    if ( FAILED(hr) )
        {
        TRACE1("CTask::SaveTask() - couldn't create a new work item. Return = 0x%08X\n",
               (DWORD) hr );
        return hr;
        }

	//mamy wskaznik do IUnknown, odpytujemy go, aby uzyskac 
	//wskaznik do interfejsu ITask  
    hr = pIUnk->QueryInterface ( IID_ITask, (void **) &pITask );

    if ( FAILED(hr) )
        {
        TRACE1("CTask::SaveTask() - QI() on IUnknown failed to get an ITask. Return = 0x%08X\n",
               (DWORD) hr );
        return hr;
        }
    

    // wskaznika do IUnknown juz nie potrzebujemy  
    pIUnk->Release();
    pIUnk = NULL;


    // ustawiamy nazwe dla aplikacji
    hr = pITask->SetApplicationName ( T2COLE( (LPCTSTR) m_sProgramPath ));

    if ( FAILED(hr) )
        {
        TRACE1("CTask::SaveTask() - failed to set application. Return = 0x%08X\n",
               (DWORD) hr );
        return hr;
        }


    // ustawiamy parametry dla aplikacji
    if ( m_sParameters.GetLength() > 0 )
        {
        hr = pITask->SetParameters ( T2COLE( (LPCTSTR) m_sParameters ));

        if ( FAILED(hr) )
            {
            TRACE1("CTask::SaveTask() - failed to set parameters. Return = 0x%08X\n",
                   (DWORD) hr );
            return hr;
            }
        }        


    // ustawiamy folder startowy dla aplikacji
    if ( m_sStartingDir.GetLength() > 0 )
        {
        hr = pITask->SetWorkingDirectory ( T2COLE( (LPCTSTR) m_sStartingDir ));

        if ( FAILED(hr) )
            {
            TRACE1("CTask::SaveTask() - failed to set starting directory. Return = 0x%08X\n",
                   (DWORD) hr );
            return hr;
            }
        }


    // ustawiamy nazwe uzytkownika i haslo, jesli jestesmy na NT
    if ( m_bNT )
        {
        hr = pITask->SetAccountInformation ( T2COLE( (LPCTSTR) m_sAccount),
                                             T2COLE( (LPCTSTR) m_sPassword) );

        if ( FAILED(hr) )
            {
            TRACE1("CTask::SaveTask() - failed to set account info.  Return = 0x%08X\n",
                   (DWORD) hr );
            return hr;
            }
        }


    // ustawiamy komentarz do zadania
    if ( m_sComment.GetLength() > 0 )
        {
        hr = pITask->SetComment ( T2COLE( (LPCTSTR) m_sComment ));

        if ( FAILED (hr) )
            {
            TRACE1("CTask::SaveTask() - failed to set task comment. Return = 0x%08X\n",
                   (DWORD) hr );
            return hr;
            }
        }


    // ustawiamy flagi
    //zapraszam do eksperymentow z flaga TASK_FLAG_DELETE_WHEN_DONE...
    dwTaskFlags = TASK_FLAG_DONT_START_IF_ON_BATTERIES |
                  TASK_FLAG_KILL_IF_GOING_ON_BATTERIES;

    // na NT ustawimy flage interactive
    if ( !( GetVersion() & 0x80000000 ) )
        {
        dwTaskFlags |= TASK_FLAG_INTERACTIVE;
        }

    hr = pITask->SetFlags ( dwTaskFlags );

    if ( FAILED (hr) )
        {
        TRACE1("CTask::SaveTask() - failed to set task flags. Return = 0x%08X\n",
               (DWORD) hr );
        return hr;
        }


    // teraz tworzymy Trigger, czyli wyzwalacz
    hr = pITask->CreateTrigger ( &wTrigNumber, &pITaskTrig );

    if ( FAILED (hr) )
        {
        TRACE1("CTask::SaveTask() - failed to create a task trigger. Return = 0x%08X\n",
               (DWORD) hr );
        return hr;
        }


    //ustawimy parametry triggera
	//zwroc uwage ze sekundy sa ignorowane
    ZeroMemory ( &rTrigger, sizeof (TASK_TRIGGER) );

    rTrigger.cbTriggerSize = sizeof (TASK_TRIGGER);
    rTrigger.wBeginYear    = m_timeStart.wYear;
    rTrigger.wBeginMonth   = m_timeStart.wMonth;
    rTrigger.wBeginDay     = m_timeStart.wDay;
    rTrigger.wStartHour    = m_timeStart.wHour;
    rTrigger.wStartMinute  = m_timeStart.wMinute;

    if ( 0 != m_timeEnd.wYear )
        {
        rTrigger.rgFlags   = TASK_TRIGGER_FLAG_HAS_END_DATE;
        rTrigger.wEndYear  = m_timeEnd.wYear;
        rTrigger.wEndMonth = m_timeEnd.wMonth;
        rTrigger.wEndDay   = m_timeEnd.wDay;
        }


    switch ( m_eFreq )
        {
        case freqOnce: 
            rTrigger.TriggerType = TASK_TIME_TRIGGER_ONCE;
        break;
        
        case freqDaily:
            rTrigger.TriggerType = TASK_TIME_TRIGGER_DAILY;

        // codziennie.
            rTrigger.Type.Daily.DaysInterval = 1;
        break;

        case freqWeekly:
            rTrigger.TriggerType = TASK_TIME_TRIGGER_WEEKLY;

            rTrigger.Type.Weekly.rgfDaysOfTheWeek = 
                GetDayOfWeekFlag ( m_timeStart );

        // co tydzien.
            rTrigger.Type.Weekly.WeeksInterval = 1;
        break;

		//co miesiac.
        case freqMonthly:
            rTrigger.TriggerType = TASK_TIME_TRIGGER_MONTHLYDATE;

            rTrigger.Type.MonthlyDate.rgfDays = 1 << ( m_timeStart.wDay - 1 );
            rTrigger.Type.MonthlyDate.rgfMonths = DDS_CST_EVERYMONTH;
        break;

        DEFAULT_UNREACHABLE;
        }


    // skojarz Trigger z zadaniem
    hr = pITaskTrig->SetTrigger ( &rTrigger );

    if ( FAILED(hr) )
        {
        TRACE1("CTask::SaveTask() - failed to add trigger to the task. Return = 0x%08X\n",
               (DWORD) hr );
        return hr;
        }


    // zapisujemy zmiany w PersistFile
    hr = pITask->QueryInterface ( IID_IPersistFile, (void **) &pIFile );

    if ( FAILED (hr) )
        {
        TRACE1("CTask::SaveTask() - failed to get an IPersistFile interface on the task. Return = 0x%08X\n",
               (DWORD) hr );
        return hr;
        }

    hr = pIFile->Save ( NULL, FALSE );

    if ( FAILED(hr) )
        {
        TRACE1("CTask::SaveTask() - error saving task. Return = 0x%08X\n",
               (DWORD) hr );
        return hr;
        }

    }   // end __try
__finally
    {
    // sprzatamy  
    if ( pIFile != NULL )
        pIFile->Release();

    if ( pITaskTrig != NULL )
        pITaskTrig->Release();

    if ( pITask != NULL )
        pITask->Release();

    if ( pISched != NULL )
        pISched->Release();

    }   // end __finally

    return hr;
}


//////////////////////////////////////////////////////////////////////////
//
// Funkcja:    DeleteTask()
//
// Opis:
//  usuwa zadanie o podanej nazwie
//
// Parametry wejsciowe:
//  szTaskName: [in] nazwa zadania do usuniecia
//
// Wartosc zwracana:
//  S_OK, jesli funkcja zakonczyla sie powodzeniem, 
// E_FAIL jesli nazwa zadania do usuniecia byla pusta
// lub kod bledu OLE jesli wystapil inny blad
//
//////////////////////////////////////////////////////////////////////////

/*static*/ HRESULT CTask::DeleteTask ( LPCTSTR szTaskName )
{
HRESULT         hr;
ITaskScheduler* pISched    = NULL;
    
    USES_CONVERSION;

    ASSERT ( AfxIsValidString ( szTaskName ));

    // sprawdz poprawnosc parametrow
    if ( 0 == lstrlen ( szTaskName ) )
        {
        return E_FAIL;
        }


    // Pobierz interfejs
    hr = ::CoCreateInstance (
               CLSID_CTaskScheduler,
               NULL,
               CLSCTX_INPROC_SERVER,
               IID_ITaskScheduler,
               (void **) &pISched );

 
    if ( FAILED(hr) )
        {
#ifdef _DEBUG
        TRACE1("CTask::DeleteTask() - failed to create a task scheduler interface. Return = 0x%08X\n",
               (DWORD) hr );

        // sprawdz czy Task Scheduler jest zainstalowany
        if ( REGDB_E_CLASSNOTREG == hr )
            {
            TRACE0("    The error was REGDB_E_CLASSNOTREG, meaning you don't have the scheduler installed.\n"
                _T("    If you are running 95 or NT 4 with IE 4, you must install the task scheduler from the\n")
                _T("    IE components install page on MS's web site or the IE CD.\n")
                _T("    If you're on 98, NT 5, or 95/NT 4 with IE 5, then something's wrong with your install\n")
                _T("    because the scheduler should always be present.\n")
                _T("    Note that this class does *NOT* work with the \"AT\" service, which is the default\n")
                _T("    scheduler on NT 4 and earlier.\n") );
            }
#endif  // _DEBUG

        return hr;
        }


    // usun zadanie
    hr = pISched->Delete ( T2COLE ( szTaskName ));
  
    if ( FAILED(hr) )
        {
        TRACE1("CTask::DeleteTask() - failed to delete task. Return = 0x%08X\n",
               (DWORD) hr );
        }

	//sprzatamy
    pISched->Release();

    return hr;
}


//////////////////////////////////////////////////////////////////////////
//
// Funkcja:    GetDayOfWeekFlag()
//
// Opis:
// oblicza dzien tygodnia w ktory przypada dana data
//
// Parametry wejsciowe:
//  time: [in] data
//
// Wartosc zwracana:
// wskaznik do TASK, wskazujacy na dzien tygodnia
//
//////////////////////////////////////////////////////////////////////////

WORD CTask::GetDayOfWeekFlag ( const CTime& time ) const
{
static WORD s_wDayFlags[] = { 0, TASK_SUNDAY, TASK_MONDAY, TASK_TUESDAY,
                              TASK_WEDNESDAY, TASK_THURSDAY, TASK_FRIDAY,
                              TASK_SATURDAY };
WORD wRet = 0;
int  nDayOfWeek = time.GetDayOfWeek();

    ASSERT ( nDayOfWeek >= 1  &&  nDayOfWeek <= 7 );

    wRet = s_wDayFlags [ nDayOfWeek ];

    ASSERT ( wRet != 0 );

    return wRet;
}
