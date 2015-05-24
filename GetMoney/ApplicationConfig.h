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
//                         变更历史
//-----------------------------------------------------------------------------
//      
//         日期          修改人   任务代号       描述
//         ----------    ------   --------       --------------
//         2013-09-05    陈家兵   t001.07	     数据库连接并读取配置
//         2013-09-03    陈家兵   t001.06        主程序搭建
//
//-----------------------------------------------------------------------------
//                          文件描述
//-----------------------------------------------------------------------------
//
//   此文件及类用于读取配置文件
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

    // 上期交易前置机地址
    char					    m_trader_front_addr[128]; 
    // 上期行情前置机地址
    char                        m_md_front_addr[128];    
    // 经济公司ID
    TThostFtdcBrokerIDType	    m_broker_id;
    // 投资者ID
    TThostFtdcInvestorIDType    m_investor_id;
    // 投资者密码
    TThostFtdcPasswordType	    m_password;
	// 是否交易
	bool                        m_is_trading;

	// 交易参数
	// 交易合约
	char                        m_contract_code[128];
	// 参考价格
	double                      m_price_ref;
	// 震动幅度
	double                      m_delta;
	// 回调点数
	double                      m_bodong;
	// 


public:
	ApplicationConfig();
	~ApplicationConfig();


	// 加载配置文件
	int LoadConfig(const char* filename);


};

#endif