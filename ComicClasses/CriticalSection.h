

/*
To use:
CriticalSection _section; //as a global or class variable or whatever
_section.Enter();  //and that's it to declare a lock.

*/
class CriticalSectionLock
{
public:
    CriticalSectionLock(LPCRITICAL_SECTION section) : _section(section)
    {
        if( _section != NULL )
            EnterCriticalSection(_section);
    }

    CriticalSectionLock(const CriticalSectionLock &right) : _section(right._section)
    {
        if( _section != NULL )
            EnterCriticalSection(_section);
    }

    CriticalSectionLock& operator=(const CriticalSectionLock &right)
    {
        if( _section != NULL )
            LeaveCriticalSection(_section);
        _section = right._section;
        if( _section != NULL )
            EnterCriticalSection(_section);
        return *this;
    }

    ~CriticalSectionLock()
    {
        if( _section != NULL )
            LeaveCriticalSection(_section);
    }

    void Leave()
    {
        if( _section != NULL )
        {
            LeaveCriticalSection(_section);
            _section = NULL;
        }
    }
private:
    LPCRITICAL_SECTION _section;
};

class CriticalSection
{
public:
    CriticalSection()
    {
        InitializeCriticalSection(&_section);
    }

    ~CriticalSection()
    {
        DeleteCriticalSection(&_section);
    }

    CriticalSectionLock Enter()
    {
        return CriticalSectionLock(&_section);
    }
private:
    CRITICAL_SECTION _section;
};

