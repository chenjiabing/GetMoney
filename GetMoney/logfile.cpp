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
// LogFile.cpp: implementation of the CLogFile class.
//
// 2007-7-4 15:50:19 为了控制日志文件大小不超过系统文件大小限制，特别设置为
//						文件不超过2G，并作相应的修改
//////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include "logfile.h"
#include "time.h"
#include "stdio.h"

#include <sys/stat.h>

#ifdef WIN32
#include  <io.h>
#include <process.h>
#include <direct.h>
#define PATHMARK  '\\'
#define MAKEPATH(a) mkdir(a)
#else
#include <sys/io.h>
#include <sys/types.h>
#include <dirent.h>
#include <string.h>
#include <unistd.h>
#include <stdarg.h>
#define PATHMARK  '/'
#define MAKEPATH(a) mkdir(a,0777)
#endif


#define MAXLOGLENGTH 2000000000  // 最长的LOG文件长度限制

int g_ShowLogLevel=10000;
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CLogFile::CLogFile(const char *szLogPath,int fdelay)
{
   if (szLogPath!=NULL && szLogPath[0]!='\0')
      strncpy(logpath,szLogPath,sizeof(logpath));
   else
      strcpy(logpath,".");  // 缺省为当前目录下
   logpath[sizeof(logpath)-1]='\0';

   fp = NULL;
   lastftime = 0;
   flashdelay = fdelay;
   logdate = 0;
   mode = 0;
	m_FileNumber = 0;
   m_WriteLevel = 0;
   SetLogShowFunc(NULL);
}

CLogFile::~CLogFile()
{
#ifdef SYSLOG
   closelog();
#endif 
   Close();
}

void CLogFile::Close()
{
#ifdef _MT
   CPMUTEX amutex(&m_mutex);
#endif
   close();
}

bool CLogFile::Open(const char *szLogPath, int LogFileMode)
{
#ifdef _MT
   CPMUTEX amutex(&m_mutex);
#endif
   if (szLogPath!=NULL && szLogPath[0]!='\0')
      strncpy(logpath,szLogPath,sizeof(logpath));
   logpath[sizeof(logpath)-1]='\0';
   mode = LogFileMode;
   return(open());
}

const char* CLogFile::GetLevelDescription(int level)
{
    switch(level)
    {
        // 调试
        case ITS_LOG_LEVEL_DEBUG:
            return "调试";
        // 信息
        case ITS_LOG_LEVEL_INFO:
            return "信息";
        // 警告
        case ITS_LOG_LEVEL_WARN:
            return "警告";
        // 错误
        case ITS_LOG_LEVEL_ERROR:
            return "错误";
        // 致命错误
        case ITS_LOG_LEVEL_FATAL:
            return "致命错误";
        default:
            return "";
    }

    return "";
}

void CLogFile::SetFlushDelay(int iFlushDelayInSecond)
{
   flashdelay = iFlushDelayInSecond;
}

void CLogFile::WriteLogEx(int level,const char *pFormat, ...)
{
#ifdef _MT
	CPMUTEX amutex(&m_mutex);
#endif
	if (level<m_WriteLevel)
		return;
   
	char chMsg[8192] = {0};
#ifdef WIN32
	SYSTEMTIME stCurTime = {0};
	::GetLocalTime(&stCurTime);
	sprintf(chMsg,"%4d-%2d-%2d %02d:%02d:%02d.%3d ", stCurTime.wYear, stCurTime.wMonth, stCurTime.wDay,
		stCurTime.wHour, stCurTime.wMinute, stCurTime.wSecond, stCurTime.wMilliseconds);
#else
	time_t tmp_time;
	struct tm *p_time;
	time(&tmp_time);
	p_time = localtime(&tmp_time);
	sprintf(chMsg,"%4d-%2d-%2d %02d:%02d:%02d", p_time->tm_year + 1900, \
		 p_time->tm_mon + 1, p_time->tm_mday, p_time->tm_hour, p_time->tm_min, p_time->tm_sec); 
#endif
	




	int i = strlen(chMsg);
	sprintf(chMsg+i,"[%d] [%s] ",getpid(), GetLevelDescription(level));
	i = strlen(chMsg);

	va_list pArg;
	va_start(pArg,pFormat);
	vsprintf(chMsg+i,pFormat,pArg);
	va_end(pArg);
	i = strlen(chMsg);
	if (chMsg[i-2]!='\n')
	{
		strcat(chMsg,"\n");
	}
#ifdef WIN32
	time_t curtime;	
	time(&curtime);	
	struct tm tmloc;
	memset(&tmloc, 0, sizeof(struct tm));
	localtime_s(&tmloc, &curtime);
	if (logdate!=((tmloc.tm_year+1900)*100+tmloc.tm_mon+1)*100+tmloc.tm_mday)
	{
		m_FileNumber = 0;
		open();
	}
#else
	time_t curtime;	
	time(&curtime);	
	struct tm *tmloc;
	tmloc = localtime(&curtime);
	if (logdate!=((tmloc->tm_year+1900)*100+tmloc->tm_mon+1)*100+tmloc->tm_mday)
	{
		m_FileNumber = 0;
		open();
	}
#endif
#ifdef SYSLOG
   int slevel;
   if (level<10000)
      slevel = LOG_INFO;
   else
      slevel = LOG_CRIT;
   syslog(slevel|LOG_USER,"%s",chMsg);
#else
	/* Updated by CHENYH at 2007-7-4 16:07:44 */
	// 为了能够检查日志文件长度不超过2G：
	i = m_FileNumber;
	do 
	{
		if (fp!=NULL)
		{
			fseek(fp,0,SEEK_END);
			if (ftell(fp)<MAXLOGLENGTH)
				break;
		}
		m_FileNumber++;
		open();
	} while(m_FileNumber<i+100);

	/******************************************/
	if (fp!=NULL)
	{
      fprintf(fp,"%s",chMsg);
      if (curtime-lastftime>flashdelay)
      {
         fflush(fp);
         lastftime = curtime;
      }
   }
#endif

   if (level>=g_ShowLogLevel)
   {
      if (pLogShow)
         (*pLogShow)("%s",chMsg);
      else
         printf("%s",chMsg);
   }
}



void CLogFile::close()
{
#ifdef SYSLOG
   closelog();
#else
   if (fp!=NULL)
   {
      fclose(fp);
   }
#endif
   fp = NULL;
   lastftime = time(NULL);
//   logdate = 0;
}

bool CLogFile::open()
{
#ifdef WIN32
   int shflag;
#endif
   int len;
   struct tm tmloc;
   char fname[1024];
   close();

   len = strlen(logpath);
   if (logpath[len-1]==PATHMARK)
      logpath[len-1]='\0';

   time(&lastftime);
   memcpy(&tmloc,localtime(&lastftime),sizeof(tmloc));
   logdate = ((tmloc.tm_year+1900)*100+tmloc.tm_mon+1)*100+tmloc.tm_mday;
#ifdef SYSLOG
   sprintf(fname,"%d.log",logdate);
   openlog(fname,LOG_PID|LOG_PERROR,LOG_USER);
   return(true);
#else
#ifdef WIN32
   if (_access(logpath,0)!=0)
   {
      MAKEPATH(logpath);
   }
#else
   if (access(logpath, F_OK)!=0)
   {
      MAKEPATH(logpath);
   }
#endif

   /* ****** Updated by CHENYH at 2005-11-17 16:28:25 ****** */
   if (mode)
   {
#ifdef WIN32
      sprintf(fname,"%s%c%d_%u_%d.log",logpath,PATHMARK,logdate,GetCurrentProcessId(),m_FileNumber);
#else
      sprintf(fname,"%s%c%d_%d_%d.log",logpath,PATHMARK,logdate,getpid(),m_FileNumber);
#endif
   }
   else
   {
      sprintf(fname,"%s%c%d_%d.log",logpath,PATHMARK,logdate,m_FileNumber);
   }
   /**********************************************************/
#ifdef WIN32
   shflag = 0x40;
   fp = _fsopen(fname,"a+t",shflag);
   if (fp==NULL)
   {
      fp = _fsopen(fname,"w+t",shflag);
   }
#else
   fp = fopen(fname,"a+t");
   if (fp==NULL)
   {
      fp = fopen(fname,"w+t");
   }
#endif
   return(fp!=NULL);
#endif
}

int CLogFile::RemoveOverdueLogFile(int overdue)
{
   char szpath[1024];
#ifdef SYSLOG
   return(-1);
#endif   
   sprintf(szpath,"%s%c",logpath,PATHMARK);
   //return(RemoveOverdueFile(szpath,".log",overdue));
   return 0;
}

void CLogFile::ToFlush()
{
#ifdef _MT
   CPMUTEX amutex(&m_mutex);
#endif
#ifdef SYSLOG
#else
   if (fp!=NULL)
   {
      fflush(fp);
      lastftime = time(NULL);
   }
#endif
}


void SetLogShowLevel(int logshowlevel)
{
   g_ShowLogLevel = logshowlevel;
}

void CLogFile::SetWriteLevel(int level)
{
   m_WriteLevel = level;
}

void CLogFile::SetLogShowFunc(FUNC_LOGSHOW pShowFunc)
{
   pLogShow = pShowFunc;
}
