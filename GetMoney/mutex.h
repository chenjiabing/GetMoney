// mutex.h: interface for the CMUTEX class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_MUTEX_H__41E159AF_3A8F_471C_A5CB_03544AD54994__INCLUDED_)
#define AFX_MUTEX_H__41E159AF_3A8F_471C_A5CB_03544AD54994__INCLUDED_
#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifdef WIN32
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#else
#include <pthread.h>
#endif


class CMUTEX  
{
public:
	int TryLock();
	void Lock();
	void UnLock();
	//void Destroy();
	//void Init();
	CMUTEX();
	virtual ~CMUTEX();
protected:
//public:
#ifdef WIN32
   HANDLE m_lock;
#else
   pthread_mutex_t m_lock;
#endif
};


class CPMUTEX  
{
public:
	CPMUTEX(CMUTEX *pmutex);
	virtual ~CPMUTEX();
private:
   CMUTEX *p_mutex;
};

class CMutexEvent : public CMUTEX  
{
public:
	bool WaitEventTime(unsigned int wait_ms);
	void SetEvent();
	void WaitEvent();
	CMutexEvent();
	virtual ~CMutexEvent();

private:
#ifdef WIN32
   HANDLE m_event;
#else
   pthread_cond_t m_event;
#endif
};


//ÁÙ½çËø
class CSectionLock
{
public:
    CSectionLock();
    ~CSectionLock();
    void Lock();
    void UnLock();
public:
#ifdef WIN32
    CRITICAL_SECTION m_Section;
#else
    pthread_mutex_t m_lock;
#endif
};

class CNullLock
{
public:
    CNullLock(){};
    ~CNullLock(){};
    void Lock(){};
    void UnLock(){};
};

class CSectionEvent : public CSectionLock
{
public:
    bool WaitEventTime(unsigned int wait_ms);
    void SetEvent();
    void WaitEvent();
    CSectionEvent();
    virtual ~CSectionEvent();
    
private:
#ifdef WIN32
    HANDLE m_event;
#else
    pthread_cond_t m_event;
#endif
};


template<class T>
class CLockGuard
{
public:
    CLockGuard(T &mtx) : m_mtx(mtx) {
        m_mtx.Lock();
    };
    
    ~CLockGuard() {
        m_mtx.UnLock();
    };
private:
    T &m_mtx;
};

#endif // !defined(AFX_MUTEX_H__41E159AF_3A8F_471C_A5CB_03544AD54994__INCLUDED_)
