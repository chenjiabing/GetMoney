
// GetMoney.h : PROJECT_NAME Ӧ�ó������ͷ�ļ�
//

#pragma once

#ifndef __AFXWIN_H__
	#error "�ڰ������ļ�֮ǰ������stdafx.h�������� PCH �ļ�"
#endif

#include "resource.h"		// ������


// CGetMoneyApp:
// �йش����ʵ�֣������ GetMoney.cpp
//

class CGetMoneyApp : public CWinApp
{
public:
	CGetMoneyApp();

	// �µ�
	int  RequestOrder(char* contract_code,char bs, char oe);

	//
	int m_request_no;
// ��д
public:
	virtual BOOL InitInstance();

// ʵ��

	DECLARE_MESSAGE_MAP()
};

extern CGetMoneyApp theApp;