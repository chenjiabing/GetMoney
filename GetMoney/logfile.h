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
//                         变更历史
//-----------------------------------------------------------------------------
//      
//         日期          修改人   任务代号       描述
//         ----------    ------   --------       --------------
//         2013-09-10    陈家兵   t001.12        加入 GetLevelDescription()
//         xxxx-xx-xx    陈永华   xxxx.xx        最初版本
//
//-----------------------------------------------------------------------------
//                          文件描述
//-----------------------------------------------------------------------------
//
//   此文件用于定义日志类
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

/// 日志级别
// 致命错误
#define ITS_LOG_LEVEL_FATAL                       6000
// 错误
#define ITS_LOG_LEVEL_ERROR                       4000
// 警告
#define ITS_LOG_LEVEL_WARN                        3000
// 信息
#define ITS_LOG_LEVEL_INFO                        2000
// 调试信息
#define ITS_LOG_LEVEL_DEBUG                       1000
// 调试信息
#define ITS_LOG_LEVEL_ALL						  0

class BUPUB_API CLogFile  
{
private:
   FILE *fp;      // 日志文件句柄
   char logpath[256];   // 日志文件存放路径
   int  logdate;        // 当前日志文件日期
   time_t lastftime;
   int flashdelay;      // 刷新延时, 单位秒
   int mode;            // 0:不带PID，1:带PID
   int m_FileNumber;  // CYH Added for controlling file length

   int m_WriteLevel;    // 2007-10-31 17:28:54 for disable writing

   FUNC_LOGSHOW pLogShow;

#ifdef _MT
   CMUTEX m_mutex;   // 互斥量
#endif

private:
	bool open();
	void close();

    // 获取日志等级描述
    const char* GetLevelDescription(int level);
   
public:
	void SetLogShowFunc(FUNC_LOGSHOW pShowFunc);  // 当为图形界面的时候，可以通过它将logshowlevel级别以上的日志信息显示到指定窗口上
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

// 全局变量
extern CLogFile* g_log_file;

#undef BUPUB_API
#undef _MT

//extern int g_ShowLogLevel;
#endif // !defined(AFX_LOGFILE_H__2A6C7BA5_B629_4353_928E_F19768A217CC__INCLUDED_)
