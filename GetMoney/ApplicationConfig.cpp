//-----------------------------------------------------------------------------
//                            ApplicationConfig.cpp 
//-----------------------------------------------------------------------------
//
//         File Name       : ApplicationConfig.cpp 
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
//
//-----------------------------------------------------------------------------
//                          文件描述
//-----------------------------------------------------------------------------
//
//   此文件及类用于读取配置文件
//
//-----------------------------------------------------------------------------
#include "stdafx.h"
#include "public.h"
#include <stdio.h>
#include <string.h>
#include "inifile.h"


#include "ApplicationConfig.h"



ApplicationConfig::ApplicationConfig()
{
}


ApplicationConfig::~ApplicationConfig()
{

}

// 加载配置文件
int ApplicationConfig::LoadConfig(const char* filename)
{
	CIniFile         ini_file;
	char             text[128];


	// 打开配置文件
	int status =  ini_file.OpenIniFile(filename);
	if (!status)
	{
		printf("加载配置文件 %s 失败.\n", filename);
		return -1;
	}

	// Broker Id
	strncpy (m_broker_id,        ini_file.ReadString("SYS",    "BrokerID",      ""),				sizeof (m_broker_id) - 1);
	// 用户名
	strncpy (m_investor_id,      ini_file.ReadString("SYS",    "InvestorId",      ""),				sizeof (m_investor_id) - 1);
	// 密码
	strncpy (m_password,         ini_file.ReadString("SYS",	   "Password",      ""),				sizeof (m_password) - 1);
	// 行情前置机
	strncpy (text,				 ini_file.ReadString("SYS",	   "FrontAddrMD",      ""),				sizeof (text) - 1);
	_snprintf (m_md_front_addr,
	           sizeof(m_md_front_addr) - 1,
		 	   "tcp://%s",
		       text);
	// 交易前置机
	strncpy (text,				 ini_file.ReadString("SYS",	   "FrontAddrT",      ""),				sizeof (text) - 1);
	_snprintf (m_trader_front_addr,
	           sizeof(m_trader_front_addr) - 1,
		 	   "tcp://%s",
		       text);

	// 是否交易
	m_is_trading			   = ini_file.ReadDouble("SYS",	   "IsTrading",     0);
	// 是否行情

	// 交易参数
	// 合约
	strncpy (m_contract_code,         ini_file.ReadString("PARAMETERS",	   "Contract",      ""),				sizeof (m_contract_code) - 1);
	
	// 参考价格
	m_price_ref    = ini_file.ReadDouble("PARAMETERS",	   "PriceRef",     0);
	// 幅度                   ini_file.ReadString("PARAMETERS",	   "Delta",      ""),
	m_delta		   = ini_file.ReadDouble("PARAMETERS",	   "Delta",       5);
	// 波动
	m_bodong       = ini_file.ReadDouble("PARAMETERS",	   "BoDong",      2);

	return 0;
}
