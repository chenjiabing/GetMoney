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
//                         �����ʷ
//-----------------------------------------------------------------------------
//      
//         ����          �޸���   �������       ����
//         ----------    ------   --------       --------------
//
//-----------------------------------------------------------------------------
//                          �ļ�����
//-----------------------------------------------------------------------------
//
//   ���ļ��������ڶ�ȡ�����ļ�
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

// ���������ļ�
int ApplicationConfig::LoadConfig(const char* filename)
{
	CIniFile         ini_file;
	char             text[128];


	// �������ļ�
	int status =  ini_file.OpenIniFile(filename);
	if (!status)
	{
		printf("���������ļ� %s ʧ��.\n", filename);
		return -1;
	}

	// Broker Id
	strncpy (m_broker_id,        ini_file.ReadString("SYS",    "BrokerID",      ""),				sizeof (m_broker_id) - 1);
	// �û���
	strncpy (m_investor_id,      ini_file.ReadString("SYS",    "InvestorId",      ""),				sizeof (m_investor_id) - 1);
	// ����
	strncpy (m_password,         ini_file.ReadString("SYS",	   "Password",      ""),				sizeof (m_password) - 1);
	// ����ǰ�û�
	strncpy (text,				 ini_file.ReadString("SYS",	   "FrontAddrMD",      ""),				sizeof (text) - 1);
	_snprintf (m_md_front_addr,
	           sizeof(m_md_front_addr) - 1,
		 	   "tcp://%s",
		       text);
	// ����ǰ�û�
	strncpy (text,				 ini_file.ReadString("SYS",	   "FrontAddrT",      ""),				sizeof (text) - 1);
	_snprintf (m_trader_front_addr,
	           sizeof(m_trader_front_addr) - 1,
		 	   "tcp://%s",
		       text);

	// �Ƿ���
	m_is_trading			   = ini_file.ReadDouble("SYS",	   "IsTrading",     0);
	// �Ƿ�����

	// ���ײ���
	// ��Լ
	strncpy (m_contract_code,         ini_file.ReadString("PARAMETERS",	   "Contract",      ""),				sizeof (m_contract_code) - 1);
	
	// �ο��۸�
	m_price_ref    = ini_file.ReadDouble("PARAMETERS",	   "PriceRef",     0);
	// ����                   ini_file.ReadString("PARAMETERS",	   "Delta",      ""),
	m_delta		   = ini_file.ReadDouble("PARAMETERS",	   "Delta",       5);
	// ����
	m_bodong       = ini_file.ReadDouble("PARAMETERS",	   "BoDong",      2);

	return 0;
}
