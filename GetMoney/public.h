#pragma once

#include"stdafx.h"

#include <deque>
using namespace std;


#include "logfile.h"

// ��־��¼ȫ�ֱ���
extern      CLogFile*                                g_log_file;

#define WM_UPDATE_TEXT WM_USER + 0x01


// ����۸�ṹ��
typedef struct _QuotePrice
{
	// �۸����
	int   index;
	// ���۸�
	double price_last;
	// ����ʱ��
	char   price_time[16];
}QuotePrice;

// ����ṹ��
typedef struct _QuoteContract
{
	// ��Լ����
	char  contract_code[32];
	// ���������
	int   last_index;
	// ����ṹ��	
	deque<QuotePrice> price_list;
	
}QuoteContract;