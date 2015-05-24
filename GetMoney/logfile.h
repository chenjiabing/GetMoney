//-----------------------------------------------------------------------------
//                            logfile.cpp 
//-----------------------------------------------------------------------------
//
//         File Name       : logfile.cpp 
//
//         Reference       : P006-00-01
//
//         (C) Copyright SUNGRAD, 2013
//
//-----------------------------------------------------------------------------
//                         �����ʷ
//-----------------------------------------------------------------------------
//      
//         ����          �޸���   �������       ����
//         ----------    ------   --------       --------------
//         2013-09-10    �¼ұ�   t001.12        ���� GetLevelDescription()
//         xxxx-xx-xx    ������   xxxx.xx        ����汾
//
//-----------------------------------------------------------------------------
//                          �ļ�����
//-----------------------------------------------------------------------------
//
//   ���ļ����ڶ�����־��
//
//-----------------------------------------------------------------------------
// logfile.h: interface for the CLogFile class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_LOGFILE_H__2A6C7BA5_B629_4353_928E_F19768A217CC__INCLUDED_)
#define AFX_LOGFILE_H__2A6C7BA5_B629_4353_928E_F19768A217CC__INCLUDED_
#include "stdafx.h"
#include "mutex.h"
#include "stdio.h"

typedef void (*FUNC_LOGSHOW)(const char *fmt,...);

#ifndef BUDLL
#define BUPUB_API 
#else
#ifdef BUPUB_EXPORTS
#define BUPUB_API __declspec(dllexport)
#else
#ifdef _MSC_VER
#define BUPUB_API __declspec(dllimport)
#else
#define BUPUB_API 
#endif
#endif
#endif

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

/// ��־����
// ��������
#define ITS_LOG_LEVEL_FATAL                       6000
// ����
#define ITS_LOG_LEVEL_ERROR                       4000
// ����
#define ITS_LOG_LEVEL_WARN                        3000
// ��Ϣ
#define ITS_LOG_LEVEL_INFO                        2000
// ������Ϣ
#define ITS_LOG_LEVEL_DEBUG                       1000
// ������Ϣ
#define ITS_LOG_LEVEL_ALL						  0

class BUPUB_API CLogFile  
{
private:
   FILE *fp;      // ��־�ļ����
   char logpath[256];   // ��־�ļ����·��
   int  logdate;        // ��ǰ��־�ļ�����
   time_t lastftime;
   int flashdelay;      // ˢ����ʱ, ��λ��
   int mode;            // 0:����PID��1:��PID
   int m_FileNumber;  // CYH Added for controlling file length

   int m_WriteLevel;    // 2007-10-31 17:28:54 for disable writing

   FUNC_LOGSHOW pLogShow;

#ifdef _MT
   CMUTEX m_mutex;   // ������
#endif

private:
	bool open();
	void close();

    // ��ȡ��־�ȼ�����
    const char* GetLevelDescription(int level);
   
public:
	void SetLogShowFunc(FUNC_LOGSHOW pShowFunc);  // ��Ϊͼ�ν����ʱ�򣬿���ͨ������logshowlevel�������ϵ���־��Ϣ��ʾ��ָ��������
	void SetWriteLevel(int level);
	void ToFlush();
	int RemoveOverdueLogFile(int overdue=30);
	bool Open(const char *szLogPath=NULL,int LogFileMode=0);
	void WriteLogEx(int level, const char *pFormat, ...);
	void SetFlushDelay(int iFlushDelayInSecond);
	void Close();
	CLogFile(const char *szLogPath=NULL,int fdelay=60);
	virtual ~CLogFile();

};

BUPUB_API void SetLogShowLevel(int logshowlevel);

// ȫ�ֱ���
extern CLogFile* g_log_file;

#undef BUPUB_API
#undef _MT

//extern int g_ShowLogLevel;
#endif // !defined(AFX_LOGFILE_H__2A6C7BA5_B629_4353_928E_F19768A217CC__INCLUDED_)
