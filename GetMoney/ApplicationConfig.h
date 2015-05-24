//-----------------------------------------------------------------------------
//                            ApplicationConfig.h 
//-----------------------------------------------------------------------------
//
//         File Name       : ApplicationConfig.h 
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
//         2013-09-05    �¼ұ�   t001.07	     ���ݿ����Ӳ���ȡ����
//         2013-09-03    �¼ұ�   t001.06        ������
//
//-----------------------------------------------------------------------------
//                          �ļ�����
//-----------------------------------------------------------------------------
//
//   ���ļ��������ڶ�ȡ�����ļ�
//
//-----------------------------------------------------------------------------

#ifndef __ApplicationConfig_H_
#define __ApplicationConfig_H_
#include "stdafx.h"
#include <vector>
#include <ThostFtdcTraderApi.h>

class ApplicationConfig
{
public:

    // ���ڽ���ǰ�û���ַ
    char					    m_trader_front_addr[128]; 
    // ��������ǰ�û���ַ
    char                        m_md_front_addr[128];    
    // ���ù�˾ID
    TThostFtdcBrokerIDType	    m_broker_id;
    // Ͷ����ID
    TThostFtdcInvestorIDType    m_investor_id;
    // Ͷ��������
    TThostFtdcPasswordType	    m_password;
	// �Ƿ���
	bool                        m_is_trading;

	// ���ײ���
	// ���׺�Լ
	char                        m_contract_code[128];
	// �ο��۸�
	double                      m_price_ref;
	// �𶯷���
	double                      m_delta;
	// �ص�����
	double                      m_bodong;
	// 


public:
	ApplicationConfig();
	~ApplicationConfig();


	// ���������ļ�
	int LoadConfig(const char* filename);


};

#endif