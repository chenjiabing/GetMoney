
// GetMoney.h : PROJECT_NAME 应用程序的主头文件
//

#pragma once

#ifndef __AFXWIN_H__
	#error "在包含此文件之前包含“stdafx.h”以生成 PCH 文件"
#endif

#include "resource.h"		// 主符号


// CGetMoneyApp:
// 有关此类的实现，请参阅 GetMoney.cpp
//

class CGetMoneyApp : public CWinApp
{
public:
	CGetMoneyApp();

	// 下单
	int  RequestOrder(char* contract_code,char bs, char oe);

	//
	int m_request_no;
// 重写
public:
	virtual BOOL InitInstance();

// 实现

	DECLARE_MESSAGE_MAP()
};

extern CGetMoneyApp theApp;