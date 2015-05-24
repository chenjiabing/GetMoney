/************************************************************************
  Filename   : IniFile.CPP
  Version    : 0.51
  Author(s)  : Carsten Breuer
 --[ Description ]-------------------------------------------------------

  This file contains a complete interface to read and write ini files
  like windows do it. I have ported it to CPP.
  This class have a simple test routine at the end of this file. To make
  a executable file define INIFILE_TEST_THIS_FILE after the includes

  --[ History ] ----------------------------------------------------------
	
  0.10: Original file by Carsten Breuer. First beta version.
  0.20: Ported to a C++ class
  0.30: Linux "\n" instead "\r\n" added. Thanks to Dieter Engelbrecht
  0.40: Problem with WriteString fixed.
  0.50: Problem with file pointer solved
  0.51: We better do smaller steps now. I have reformated to tab4. Sorry 
        New function DeleteKey added. Thanks for the guy who post this.
		I have also renamed the pointer to be sonsinstent with IniFile.Cpp.

  PLEASE update GetVersion!

 --[ How to compile ]----------------------------------------------------

  This file was developed under DJGPP and Rhide. If you are familiar with
  Borland C++ 3.1, you will feel like at home ;-)).
  Both tools are free software.

  Downloads at: http://www.delorie.com/djgpp


 --[ Where to get help/information ]-------------------------------------

  The author   : C.Breuer@OpenWin.de

 --[ License ] ----------------------------------------------------------

  LGPL (Free for private and comercial use)

  This program is free software; you can redistribute it and/or
  modify it under the terms of the GNU Library General Public License
  as published by the Free Software Foundation; either version 2
  of the License, or (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU Library General Public License for more details.

  You should have received a copy of the GNU Library General Public License
  along with this program; if not, write to the Free Software
  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.

 ------------------------------------------------------------------------
  Copyright (c) 2000 Carsten Breuer

  2006-6-26,qianzhenghua 修改一个处理注释的错误
  2010-1-16,xia,zhichun 修改一个处理注释的错误,允许//作为整行的注释
***********************************************************************/

// Standard Lib
#include "stdafx.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#if defined(AIX)
	#include <extension.h>
#endif
#include "inifile.h"
#define INIFILE_TEST_THIS_FILE


/*=========================================================================
   strupr -de-
  -------------------------------------------------------------------------
   Job : String to Uppercase 22.03.2001 Dieter Engelbrecht dieter@wintop.net
========================================================================*/
#ifdef DONT_HAVE_STRUPR
/* DONT_HAVE_STRUPR is set when INI_REMOVE_CR is defined */
void strupr( char *str )
{
    // We dont check the ptr because the original also dont do it.
    while (*str != 0)
    {
        if ( islower( *str ) )
        {
		     *str = toupper( *str );
        }
        str++;
	}
}
#endif

/*=========================================================================
   CIniFile : The constructor
*========================================================================*/
CIniFile::CIniFile (void)
{
	m_pEntry      = NULL;
	m_pCurEntry   = NULL;
	m_result [0]  = 0;
	m_pIniFile    = NULL;
	m_last_error_no = INIFILE_NO_ERROR;
}

/*=========================================================================
   CIniFile : The destructor
*========================================================================*/
CIniFile::~CIniFile (void)
{
	CloseIniFile ();
}

/*=========================================================================
   CIniFile : GetVersion
   Info     : The version is BCD coded. It maintain the major version in
              the upper 8 bits and the minor in the lower.
              0x0120 means version 1.20
*========================================================================*/
UINT CIniFile::GetVersion (void)
{
	return 0x0030;
}

#define IS_BLANK(c) (c==' ' || c=='\t') 

void CIniFile::trim(char *str)
{
	int i,len;
	len=strlen(str);
	for(i=len-1;i>=0;i--)
	{
		if(!IS_BLANK(str[i]))
			break;
		else
			str[i]=0;
	}
	for(i=0;i<len;i++)
	{
        if(!IS_BLANK(str[i]))
			break;
	}
	if(i!=0)
	{
		strncpy(str,str+i,len-i);
		str[len-i]=0;
	}
}

/*=========================================================================
   OpenIniFile
  -------------------------------------------------------------------------
   Job : Opens an ini file or creates a new one if the requested file
         doesnt exists.
========================================================================*/
BOOL CIniFile::OpenIniFile (CCHR * FileName)
{
	char   Str [255];
	char   *pStr;
	struct ENTRY *pEntry;

	FreeAllMem ();

	if (FileName == NULL)                             
	{ 
		m_last_error_no = INIFILE_ERR_OPEN_FILE_FAILED;
		sprintf(m_last_err_msg, "打开ini文件失败,文件名为空");
		return FALSE; 
	}
	if ((m_pIniFile = fopen (FileName, "r")) == NULL) 
	{ 
		m_last_error_no = INIFILE_ERR_OPEN_FILE_FAILED;
		sprintf(m_last_err_msg, "打开ini文件%s失败", FileName);
		return FALSE; 
	}

	while (fgets (Str, 255, m_pIniFile) != NULL)
	{
		pStr = strchr (Str, '\n');
		if (pStr != NULL) { *pStr = 0; }
		pEntry = MakeNewEntry ();
		if (pEntry == NULL) { return FALSE; }

		#ifdef INI_REMOVE_CR
		int Len = strlen(Str);
		if ( Len > 0 )
		{
			if ( Str[Len-1] == '\r' )
			{
				Str[Len-1] = '\0';
			}
		}
		#endif

		pEntry->pText = (char *)malloc (strlen (Str)+1);
		if (pEntry->pText == NULL)
		{
			FreeAllMem ();
			m_last_error_no = INIFILE_ERR_OPEN_FILE_FAILED;
			sprintf(m_last_err_msg, "解析ini文件%s失败,没有足够的空间", FileName);
			return FALSE;
		}
		strcpy (pEntry->pText, Str);
		// xia,zhichun update at 2010-01-16
		pStr = strstr(Str,"//");
        if (pStr != NULL) { *pStr = 0; } /* Cut all comments */
/*		xia,zhichun update at 2010-01-15
		if (strlen(Str) >= 2 && Str[0] == '/' && Str[1] == '/')
		{
				pEntry->Type = tpCOMMENT;
				m_pCurEntry = pEntry;
				continue;
		}
*/
		pStr = strchr (Str,';');
		if (pStr != NULL) { *pStr = 0; } /* Cut all comments */
		if ( (strstr (Str, "[") > 0) && (strstr (Str, "]") > 0) ) /* Is Section */
		{
			pEntry->Type = tpSECTION;
			trim(pEntry->pText);
		}
		else
		{
			if (strstr (Str, "=") > 0)
			{
				pEntry->Type = tpKEYVALUE;
			}
			else
			{
				pEntry->Type = tpCOMMENT;
			}
		}
		m_pCurEntry = pEntry;
	}
	fclose (m_pIniFile);
	m_pIniFile = NULL;
	return TRUE;
}

/*=========================================================================
   CloseIniFile
  -------------------------------------------------------------------------
   Job : Closes the ini file without any modifications. If you want to
         write the file use WriteIniFile instead.
========================================================================*/
void CIniFile::CloseIniFile (void)
{
	FreeAllMem ();
	if (m_pIniFile != NULL)
	{
		fclose (m_pIniFile);
		m_pIniFile = NULL;
	}
}


/*=========================================================================
   WriteIniFile
  -------------------------------------------------------------------------
   Job : Writes the iniFile to the disk and close it. Frees all memory
         allocated by WriteIniFile;
========================================================================*/
bool CIniFile::WriteIniFile (const char *pFileName)
{
	struct ENTRY *pEntry = m_pEntry;
	if (m_pIniFile != NULL)
	{
		fclose (m_pIniFile);
	}
	if ((m_pIniFile = fopen (pFileName, "wb")) == NULL)
	{
		FreeAllMem ();
		return FALSE;
	}

	while (pEntry != NULL)
	{
		if (pEntry->Type != tpNULL)
		{
#			ifdef INI_REMOVE_CR
			fprintf (m_pIniFile, "%s\n", pEntry->pText);
#			else
			fprintf (m_pIniFile, "%s\r\n", pEntry->pText);
#			endif
		}
		pEntry = pEntry->pNext;
	}

	fclose (m_pIniFile);
	m_pIniFile = NULL;
	return TRUE;
}


/*=========================================================================
   WriteString : Writes a string to the ini file
*========================================================================*/
void CIniFile::WriteString (CCHR *pSection, CCHR *pKey, CCHR *pValue)
{
	EFIND List;
	char  Str [255];

	if (ArePtrValid (pSection, pKey, pValue) == FALSE) { return; }
	if (FindKey  (pSection, pKey, &List) == TRUE)
	{
		sprintf (Str, "%s=%s%s", List.KeyText, pValue, List.Comment);
		FreeMem (List.pKey->pText);
		List.pKey->pText = (char *)malloc (strlen (Str)+1);
		strcpy (List.pKey->pText, Str);
	}
	else
	{
		if ((List.pSec != NULL) && (List.pKey == NULL)) /* section exist, Key not */
		{
			AddKey (List.pSec, pKey, pValue);
		}
		else
		{
			AddSectionAndKey (pSection, pKey, pValue);
		}
	}
}

/*=========================================================================
   WriteBool : Writes a boolean to the ini file
*========================================================================*/
void CIniFile::WriteBool (CCHR *pSection, CCHR *pKey, bool Value)
{
	char Val [2] = {'0',0};
	if (Value != 0) { Val [0] = '1'; }
	WriteString (pSection, pKey, Val);
}

/*=========================================================================
   WriteInt : Writes an integer to the ini file
*========================================================================*/
void CIniFile::WriteInt (CCHR *pSection, CCHR *pKey, int Value)
{
	char Val [12]; /* 32bit maximum + sign + \0 */
	sprintf (Val, "%d", Value);
	WriteString (pSection, pKey, Val);
}

/*=========================================================================
   WriteDouble : Writes a double to the ini file
*========================================================================*/
void CIniFile::WriteDouble (CCHR *pSection, CCHR *pKey, double Value)
{
	char Val [32]; /* DDDDDDDDDDDDDDD+E308\0 */
	sprintf (Val, "%1.10lE", Value);
	WriteString (pSection, pKey, Val);
}


/*=========================================================================
   ReadString : Reads a string from the ini file, set error info
*========================================================================*/
CCHR *CIniFile::ReadString (CCHR *pSection, CCHR *pKey, CCHR *pDefault)
{
	// Return directly if failed to open file
	if(INIFILE_ERR_OPEN_FILE_FAILED == m_last_error_no)
	{
		return pDefault;
	}

	EFIND List;
	if (ArePtrValid (pSection, pKey, pDefault) == FALSE) 
	{ 
		// Set error info if pointer is invalid
		m_last_error_no = INIFILE_ERR_READ_EMPTY_VALUE;
		sprintf(m_last_err_msg, "读ini配置时,存在键或值的指针无效的情况,使用默认值");

		return pDefault; 
	}
	else if (FindKey  (pSection, pKey, &List) == TRUE)
	{
		// Clear error info if current key is right
		m_last_error_no = INIFILE_NO_ERROR;
		m_last_err_msg[0] = '\0';

		trim(List.ValText);
        strcpy (m_result, List.ValText);
		return m_result;
	}
	else
	{
		// Set error info if failed to find this key
		m_last_error_no = INIFILE_ERR_READ_EMPTY_VALUE;
		sprintf(m_last_err_msg, "ini文件中,配置项[%s]%s不存在,使用默认值\"%s\"", pSection, pKey, pDefault);

		return pDefault;
	}
}

/*=========================================================================
   ReadBool : Reads a boolean from the ini file
*========================================================================*/
BOOL CIniFile::ReadBool (CCHR *pSection, CCHR *pKey, BOOL Default)
{
	char Val [2] = {"0"};
	if (Default != 0) { Val [0] = '1'; }
	return (atoi (ReadString (pSection, pKey, Val))?1:0); /* Only allow 0 or 1 */
}

/*=========================================================================
   ReadInt : Reads a integer from the ini file
*========================================================================*/
int CIniFile::ReadInt (CCHR *pSection, CCHR *pKey, int Default)
{
	char Val [12];
	sprintf (Val,"%d", Default);
	return (atoi (ReadString (pSection, pKey, Val)));
}

/*=========================================================================
   ReadDouble : Reads a double from the ini file
*========================================================================*/
double CIniFile::ReadDouble (CCHR *pSection, CCHR *pKey, double Default)
{
	double Val;
	sprintf (m_result, "%1.10lE", Default);
	sscanf (ReadString (pSection, pKey, m_result), "%lE", &Val);
	return Val;
}

/*=========================================================================
   DeleteKey : Deletes an entry from the ini file
*========================================================================*/
bool CIniFile::DeleteKey (CCHR *pSection, CCHR *pKey)
{
	EFIND         List;
	struct ENTRY *pPrev;
	struct ENTRY *pNext;

	if (FindKey (pSection, pKey, &List) == TRUE)
	{
		pPrev = List.pKey->pPrev;
		pNext = List.pKey->pNext;
		if (pPrev)
		{
			pPrev->pNext=pNext;
		}
		if (pNext)
		{ 
			pNext->pPrev=pPrev;
		}
		FreeMem (List.pKey->pText);
		FreeMem (List.pKey);
		return TRUE;
	}
	return FALSE;
}

INIFILE_ERROR_NO CIniFile::GetLastError(void)
{
	return m_last_error_no;
}

const char* CIniFile::GetLastErrMsg(void)
{
	return m_last_err_msg;
}


/* Here we start with our helper functions */

void CIniFile::FreeMem (void *pPtr)
{
	if (pPtr != NULL) { free (pPtr); }
}

void CIniFile::FreeAllMem (void)
{
	struct ENTRY *pEntry;
	struct ENTRY *pNextEntry;
	pEntry = m_pEntry;
	while (1)
	{
		if (pEntry == NULL) { break; }
		pNextEntry = pEntry->pNext;
		FreeMem (pEntry->pText); /* Frees the pointer if not NULL */
		FreeMem (pEntry);
		pEntry = pNextEntry;
	}
	m_pEntry    = NULL;
	m_pCurEntry = NULL;
}

struct ENTRY *CIniFile::FindSection (CCHR *pSection)
{
	char Sec  [130];
	char iSec [130];
	struct ENTRY *pEntry;
	sprintf (Sec, "[%s]", pSection);
#if defined(WIN32)
	_strupr  (Sec);
#else
	strupr  (Sec);
#endif
	pEntry = m_pEntry; /* Get a pointer to the first Entry */
	while (pEntry != NULL)
	{
		if (pEntry->Type == tpSECTION)
		{
			strcpy  (iSec, pEntry->pText);
#if defined(WIN32)
			_strupr  (iSec);
#else
			strupr  (iSec);
#endif
			if (strcmp (Sec, iSec) == 0)
			{
				return pEntry;
			}
		}
		pEntry = pEntry->pNext;
	}
	return NULL;
}

bool CIniFile::FindKey  (CCHR *pSection, CCHR *pKey, EFIND *pList)
{
	char Search [130];
	char Found  [130];
	char Text   [255];
	char *pText;
	struct ENTRY *pEntry;
	pList->pSec        = NULL;
	pList->pKey        = NULL;
	pEntry = FindSection (pSection);
	if (pEntry == NULL) 
	{ 
		return FALSE; 
	}
	pList->pSec        = pEntry;
	pList->KeyText[0] = 0;
	pList->ValText[0] = 0;
	pList->Comment[0] = 0;
	pEntry = pEntry->pNext;
	if (pEntry == NULL) { return FALSE; }
	sprintf (Search, "%s",pKey);
#if defined(WIN32)
	_strupr  (Search);
#else
	strupr  (Search);
#endif
	while (pEntry != NULL)
	{
		if ((pEntry->Type == tpSECTION) || /* Stop after next section or EOF */
			(pEntry->Type == tpNULL   ))
		{
			return FALSE;
		}
		if (pEntry->Type == tpKEYVALUE)
		{
			strcpy (Text, pEntry->pText);
			pText = strchr (Text, ';');
			//modify by qianzhenghua 2006-6-26
			if (pText != NULL)
			{
				strcpy (pList->Comment, pText);
				*pText = 0;
			}
			else
			{
				strcpy (pList->Comment, "");
			}
			pText = strchr (Text, '=');
			if (pText != NULL)
			{
				*pText = 0;
                //使配置项等号两边可以有空格 zyc
                trim(Text);
				strcpy (pList->KeyText, Text);
				strcpy (Found, Text);
				*pText = '=';
#if defined(WIN32)
		
                _strupr (Found);
#else
				strupr (Found);
#endif
				/*printf ("%s,%s\n", Search, Found); */
				if (strcmp (Found,Search) == 0)
				{
				   strcpy (pList->ValText, pText+1);
				   pList->pKey = pEntry;
				   return TRUE;
				}
			}
		}
		pEntry = pEntry->pNext;
	}
	return FALSE;
}

BOOL CIniFile::AddItem (char Type, CCHR *pText)
{
	struct ENTRY *pEntry = MakeNewEntry ();
	if (pEntry == NULL) { return FALSE; }
	pEntry->Type = Type;
	pEntry->pText = (char*)malloc (strlen (pText) +1);
	if (pEntry->pText == NULL)
	{
		free (pEntry);
		return FALSE;
	}
	strcpy (pEntry->pText, pText);
	pEntry->pNext   = NULL;
	if (m_pCurEntry != NULL) { m_pCurEntry->pNext = pEntry; }
	m_pCurEntry    = pEntry;
	return TRUE;
}

bool CIniFile::AddItemAt (struct ENTRY *pEntryAt, char Mode, CCHR *pText)
{
	struct ENTRY *pNewEntry;
	if (pEntryAt == NULL)  { return FALSE; }
	pNewEntry = (struct ENTRY*) malloc (sizeof (ENTRY));
	if (pNewEntry == NULL) { return FALSE; }
	pNewEntry->pText = (char *) malloc (strlen (pText)+1);
	if (pNewEntry->pText == NULL)
	{
		free (pNewEntry);
		return FALSE;
	}
	strcpy (pNewEntry->pText, pText);
	if (pEntryAt->pNext == NULL) /* No following nodes. */
	{
		pEntryAt->pNext   = pNewEntry;
		pNewEntry->pNext  = NULL;
	}
	else
	{
		pNewEntry->pNext = pEntryAt->pNext;
		pEntryAt->pNext  = pNewEntry;
	}
	pNewEntry->pPrev = pEntryAt;
	pNewEntry->Type  = Mode;
	return TRUE;
}

bool CIniFile::AddSectionAndKey (CCHR *pSection, CCHR *pKey, CCHR *pValue)
{
	char Text [255];
	sprintf (Text, "[%s]", pSection);
	if (AddItem (tpSECTION, Text) == FALSE) { return FALSE; }
	sprintf (Text, "%s=%s", pKey, pValue);
	return AddItem (tpKEYVALUE, Text)? 1 : 0;
}

void CIniFile::AddKey (struct ENTRY *pSecEntry, CCHR *pKey, CCHR *pValue)
{
	char Text [255];
	sprintf (Text, "%s=%s", pKey, pValue);
	AddItemAt (pSecEntry, tpKEYVALUE, Text);
}

struct ENTRY *CIniFile::MakeNewEntry (void)
{
	struct ENTRY *pEntry;
	pEntry = (struct ENTRY *)malloc (sizeof (ENTRY));
	if (pEntry == NULL)
	{
		FreeAllMem ();
		return NULL;
	}
	if (m_pEntry == NULL)
	{
		m_pEntry = pEntry;
	}
	pEntry->Type  = tpNULL;
	pEntry->pPrev = m_pCurEntry;
	pEntry->pNext = NULL;
	pEntry->pText = NULL;
	if (m_pCurEntry != NULL)
	{
		m_pCurEntry->pNext = pEntry;
	}
	return pEntry;
}


/*
int main (void)
{
	CIniFile iFile;
	printf ("Hello World\n");
	iFile.OpenIniFile  ("Test.Ini");
	iFile.WriteString  ("Test", "Name", "Value");
	iFile.WriteString  ("Test", "Name", "OverWrittenValue");
	iFile.WriteString  ("Test", "Port", "COM1");
	iFile.WriteString  ("Test", "User", "James Brown jr.");
	iFile.WriteString  ("Configuration", "eDriver", "MBM2.VXD");
	iFile.WriteString  ("Configuration", "Wrap", "LPT.VXD");
	iFile.WriteInt     ("IO-Port", "Com", 2);
	iFile.WriteBool    ("IO-Port", "IsValid", 0);
	iFile.WriteDouble  ("TheMoney", "TheMoney", 67892.00241);
	iFile.WriteInt     ("Test"    , "ToDelete", 1234);
	iFile.WriteIniFile ("Test.Ini");
	printf ("Key ToDelete created. Check ini file. Any key to continue");
	while (!kbhit());
	iFile.OpenIniFile  ("Test.Ini");
	iFile.DeleteKey    ("Test"	  , "ToDelete");
	iFile.WriteIniFile ("Test.Ini");
	printf ("[Test] Name = %s\n", iFile.ReadString ("Test", "Name", "NotFound"));
	printf ("[Test] Port = %s\n", iFile.ReadString ("Test", "Port", "NotFound"));
	printf ("[Test] User = %s\n", iFile.ReadString ("Test", "User", "NotFound"));
	printf ("[Configuration] eDriver = %s\n", iFile.ReadString ("Configuration", "eDriver", "NotFound"));
	printf ("[Configuration] Wrap = %s\n", iFile.ReadString ("Configuration", "Wrap", "NotFound"));
	printf ("[IO-Port] Com = %d\n", iFile.ReadInt ("IO-Port", "Com", 0));
	printf ("[IO-Port] IsValid = %d\n", iFile.ReadBool ("IO-Port", "IsValid", 0));
	printf ("[TheMoney] TheMoney = %1.10lf\n", iFile.ReadDouble ("TheMoney", "TheMoney", 111));
	iFile.CloseIniFile ();
	return 0;
}
*/


