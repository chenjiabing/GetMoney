#pragma once

#include"stdafx.h"

#include <deque>
using namespace std;


#include "logfile.h"

// 日志记录全局变量
extern      CLogFile*                                g_log_file;

#define WM_UPDATE_TEXT WM_USER + 0x01


// 行情价格结构体
typedef struct _QuotePrice
{
	// 价格序号
	int   index;
	// 最后价格
	double price_last;
	// 行情时间
	char   price_time[16];
}QuotePrice;

// 行情结构体
typedef struct _QuoteContract
{
	// 合约名称
	char  contract_code[32];
	// 最后更新序号
	int   last_index;
	// 行情结构体	
	deque<QuotePrice> price_list;
	
}QuoteContract;