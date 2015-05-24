//-----------------------------------------------------------------------------
//                            TraderSpi.cpp
//-----------------------------------------------------------------------------
//
//         File Name       : TraderSpi.cpp
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
//         2013-11-25    刘庆发   t29	         老系统CTP行情，证券代码不再从中间件取合约
//
//-----------------------------------------------------------------------------
//                          文件描述
//-----------------------------------------------------------------------------
//
//   此文件及类用于和前置建立连接，获取证券代码，并根据设定的时间间隔检测证券代码更新
//
//-----------------------------------------------------------------------------
#include "stdafx.h"
#include "public.h"
#include "TraderSpi.h"
#include "ApplicationConfig.h"
#include "GetMoneyDlg.h"


// 会话参数
TThostFtdcFrontIDType	FRONT_ID;	//前置编号
TThostFtdcSessionIDType	SESSION_ID;	//会话编号
TThostFtdcOrderRefType	ORDER_REF;	//报单引用

// 交易请求号
int													g_request_id				= 0;

// 上期交易api
extern      CThostFtdcTraderApi*					g_trader_user_api;
// 程序配置
extern      ApplicationConfig						g_app_config;
// 是否可交易
extern      bool g_can_trading;

extern      CString g_text;

extern      HWND    g_dlg_handle;

CTraderSpi::CTraderSpi()
{
}

CTraderSpi::~CTraderSpi()
{

}

void CTraderSpi::OnFrontConnected()
{
	static int reconnect_count = 1;

		// 连接成功
	g_text.Format("与CTP柜台API连接成功(交易API)，连接计数:%d", reconnect_count++);
	g_log_file->WriteLogEx(ITS_LOG_LEVEL_INFO,"%s", (char*)(g_text.GetBuffer()));
		
	// 用户登录请求
	ReqUserLogin();

}


//-----------------------------------------------------------------------------
// 函数名   :    OnFrontDisconnected
//-----------------------------------------------------------------------------
//
// 函数描述 :    连接断开
// 输入     ：   nReason         - 错误代号
// 说明     :    系统自动连接，不需要本地维护
//
//-----------------------------------------------------------------------------
void CTraderSpi:: OnFrontDisconnected(int nReason)
{
	static int reconnect_count = 1;

	g_can_trading = false;

	g_text.Format("与CTP柜台前置连接断开(交易API)，错误代码：%d, 连接计数:%d", nReason, reconnect_count++);
	SendMessageA(g_dlg_handle, WM_UPDATE_TEXT, 0,  (LPARAM)(void*)(char*)(g_text.GetBuffer()));
	g_log_file->WriteLogEx(ITS_LOG_LEVEL_WARN,"%s", (char*)(g_text.GetBuffer()));
}
		

//-----------------------------------------------------------------------------
// 函数名   :    ReqUserLogin
//-----------------------------------------------------------------------------
//
// 函数描述 :    交易登录请求
// 说明     : 
//-----------------------------------------------------------------------------
int CTraderSpi::ReqUserLogin()
{
	int							status = 0;
	CThostFtdcReqUserLoginField req;

	memset(&req, 0, sizeof(req));
	
	strcpy(req.BrokerID, g_app_config.m_broker_id);
	strcpy(req.UserID,   g_app_config.m_investor_id);
	strcpy(req.Password, g_app_config.m_password);
	
    // 普通用户登录
    status  = g_trader_user_api->ReqUserLogin(&req,  ++g_request_id);
    if (status != 0)
    {
		g_text.Format("帐户[%s]发送用户登录CTP柜台请求失败(交易API), 返回状态: %d.", 
					  g_app_config.m_investor_id,
					  status);
		g_log_file->WriteLogEx(ITS_LOG_LEVEL_ERROR,"%s", (char*)(g_text.GetBuffer()));
		SendMessageA(g_dlg_handle, WM_UPDATE_TEXT, 0,  (LPARAM)(void*)(char*)(g_text.GetBuffer()));

        status = -1;
    }

	return status;
}


void CTraderSpi::OnRspUserLogin(CThostFtdcRspUserLoginField *pRspUserLogin,
								CThostFtdcRspInfoField      *pRspInfo, 
								int                         nRequestID, 
								bool                        bIsLast)
{
   // 登录状态验证
	if (pRspInfo)
	{
		// 保存会话参数
		FRONT_ID			= pRspUserLogin->FrontID;
		SESSION_ID			= pRspUserLogin->SessionID;

		// 读取返回数据
		if(pRspInfo->ErrorID == 0)
		{
            // 结算单确认请求
			ReqSettlementInfoConfirm();
		}
		else
		{
            // 记录登录日志
            g_log_file->WriteLogEx(ITS_LOG_LEVEL_ERROR, 
									"帐号[%s]登录CTP柜台失败(交易API),失败信息:%s",   
									g_app_config.m_investor_id,
									pRspInfo->ErrorMsg);
		}
	}
    else
    {
        g_log_file->WriteLogEx(ITS_LOG_LEVEL_ERROR, 
							  "帐号[%s]请求登录返回空应答(交易API),请求号:%d", 
							  g_app_config.m_investor_id,
							  nRequestID);
    }
}
int CTraderSpi::ReqSettlementInfoConfirm()
{
	int									 times  = 5;
    int									 status = 0;

	CThostFtdcSettlementInfoConfirmField req;
	memset(&req, 0, sizeof(req));

    // 经纪人ID
	strcpy(req.BrokerID,        g_app_config.m_broker_id);
    // 投资者ID
	strcpy(req.InvestorID,      g_app_config.m_investor_id);

	while (times > 0)
	{
		 --times;

		// 结算单确认
		status = g_trader_user_api->ReqSettlementInfoConfirm(&req, ++g_request_id);
		if (status)
		{
			g_log_file->WriteLogEx(ITS_LOG_LEVEL_ERROR, 
								  "帐户[%s]发送结算单确认请求失败(交易API), 返回状态: %d.", 
								  g_app_config.m_investor_id,
								  status);
			Sleep(1000);
		}
		else
		{
			// 结算单确认成功
			break;
		}
	}

	return status;
}

void CTraderSpi::OnRspSettlementInfoConfirm(CThostFtdcSettlementInfoConfirmField *pSettlementInfoConfirm, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
    // 结算单确认信息返回验证
	if (pRspInfo)
	{
		// 读取返回数据
		if(pRspInfo->ErrorID == 0)
		{
            // 记录结算单确认日志
			g_log_file->WriteLogEx(ITS_LOG_LEVEL_INFO, "帐号[%s] CTP确认结算单信息响应成功(交易API)", g_app_config.m_investor_id);

			// 可交易
			g_can_trading = true;
		}
		else
		{
            // 记录结算单确认日志
             g_log_file->WriteLogEx(ITS_LOG_LEVEL_ERROR, 
									"帐号[%s] CTP确认结算单信息响应失败(交易API),失败信息:%s",   
									g_app_config.m_investor_id,
									pRspInfo->ErrorMsg);
		}
    }
}

//-----------------------------------------------------------------------------
// 函数名   :    ReqQryInstrument
//-----------------------------------------------------------------------------
//
// 函数描述 :    发送查询申请，从前置获取全部的合约代码
// 说明     : 
//-----------------------------------------------------------------------------
int CTraderSpi::ReqQryInstrument()
{
	int							 times				= 5;
	int                          status             = 0;
	CThostFtdcQryInstrumentField req;
	
	// 清空请求结构体
	// NOTE：合约变量Instructments填空即查所有合约
	memset(&req, 0, sizeof(req));
	
	// 查询合约列表请求(最多尝试5次请求)
	while (times > 0)
	{
		 --times;

		// 查询合约列表
		status = g_trader_user_api->ReqQryInstrument(&req, ++g_request_id);
		if (status)
		{
			g_log_file->WriteLogEx(ITS_LOG_LEVEL_ERROR, "请求查询合约列表失败，受到流控(交易API)");
			Sleep(1000);
		}
		else
		{
			// 查询成功
			break;
		}
	}

	return status;
}


//-----------------------------------------------------------------------------
// 函数名   :    OnRspQryInstrument
//-----------------------------------------------------------------------------
//
// 函数描述 :    合约代号查询申请的回调函数，可以获取所有的合约代号
// 输出值   ：   pRspInfo         - 合约信息结构体
// 说明     :    1，因调用前不知道能够获取的条目数，因而发送查询申请后，需要sleep部分时间
//             以确定本函数获取到全部的合约代码，时间可配置。
//               2，函数分两种情况：第一次请求；之后会隔一定的时间重新查询，有更新则重新订阅
//-----------------------------------------------------------------------------
void CTraderSpi::OnRspQryInstrument(CThostFtdcInstrumentField *pInstrument, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{

}


//-----------------------------------------------------------------------------
// 函数名   :    OnRspError
//-----------------------------------------------------------------------------
//
// 函数描述 :    用户请求出错
// 输入     ：   pRspInfo         - 反馈的错误信息结构体
// 说明     :    
//
//-----------------------------------------------------------------------------
void CTraderSpi::OnRspError(CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
	g_log_file->WriteLogEx(ITS_LOG_LEVEL_ERROR,"用户请求出错（交易API），出错原因：%s", pRspInfo->ErrorMsg);
}


void CTraderSpi::RquestLogout()
{
	//CThostFtdcUserLogoutField reqest;
	//int result = 0;

	//memset(&reqest, 0, sizeof(reqest));
	//
	//strcpy(reqest.BrokerID, BROKER_ID);
	//strcpy(reqest.UserID,   m_investor_id);
	//
	//result = g_trader_user_api->ReqUserLogout(&reqest, ++RequestID);
	//g_log_file->WriteLogEx(ITS_LOG_LEVEL_INFO, "发送用户登出请求: %s", ((result == 0) ? "成功" : "失败"));
}

void CTraderSpi::OnRspUserLogout(CThostFtdcUserLogoutField *pUserLogout,
								CThostFtdcRspInfoField     *pRspInfo,
								int                        nRequestID,
								bool                       bIsLast)
{
	//g_log_file->WriteLogEx(ITS_LOG_LEVEL_DEBUG,"合约查询:OnRspUserLogout");
	//bool bResult = ((pRspInfo) && (pRspInfo->ErrorID != 0));
	//
	//if (bIsLast && !bResult)
	//{
	//	g_log_file->WriteLogEx(ITS_LOG_LEVEL_INFO,"合约查询完毕，交易员登出成功");
	//	m_break_flag = true;
	//}
	//else
	//{
	//	g_log_file->WriteLogEx(ITS_LOG_LEVEL_INFO,"合约查询完毕，交易员登出失败， 失败信息：%s", pRspInfo->ErrorMsg);
	//}
}



// 发送客户认证请求
int CTraderSpi::ReqAuthenticate()
{
	int                              status = 0;
	CThostFtdcReqAuthenticateField   reqAuth;
	
	// 初始化
	memset(&reqAuth, 0, sizeof(reqAuth));

	// 经济人
	strcpy(reqAuth.BrokerID,           g_app_config.m_broker_id);
	// 用户名
	strcpy(reqAuth.UserID,             g_app_config.m_investor_id);

	// 客户端认证请求
	status = g_trader_user_api->ReqAuthenticate(&reqAuth, ++g_request_id);
    if (status != 0)
    {
	    g_log_file->WriteLogEx(ITS_LOG_LEVEL_ERROR, 
                      "帐户[%s]发送客户端认证请求失败(CTP), 返回状态: %d.", 
		              g_app_config.m_broker_id,
                      status);
        status = -1;
    }

	return status;
}


///客户端认证响应
void CTraderSpi::OnRspAuthenticate(CThostFtdcRspAuthenticateField *pRspAuthenticateField, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
    // 登录状态验证
	if (pRspInfo)
	{
		// 读取返回数据
		if(pRspInfo->ErrorID == 0)
		{
            // 记录登录日志
			g_log_file->WriteLogEx(ITS_LOG_LEVEL_INFO, "帐号[%s]CTP客户端认证响应成功", g_app_config.m_investor_id);
			
			// 用户登录请求
			ReqUserLogin();
		}
		else
		{
            // 记录登录日志
            g_log_file->WriteLogEx(ITS_LOG_LEVEL_INFO, 
				          "帐号[%s]CTP客户端认证响应失败,失败信息:%s",  
						  g_app_config.m_investor_id, pRspInfo->ErrorMsg);
		}
	}
    else
    {
        g_log_file->WriteLogEx(ITS_LOG_LEVEL_ERROR, 
                      "帐号[%s]请求登录认证返回空应答,请求号:%d",
					  g_app_config.m_investor_id,
                      nRequestID);
    }

}


// 委托应答
void CTraderSpi::OnRspOrderInsert(CThostFtdcInputOrderField *pInputOrder, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
    if (pRspInfo && pRspInfo->ErrorID != 0)
    {
		g_text.Format( "帐号[%s]委托失败通知(CTP); InvestorID: %s, InstrumentID:%s, OrderRef:%s, Direction:%c, "
                                                 "CombOffsetFlag:%s, LimitPrice: %f, VolumeTotalOriginal:%d, ErrorMsg:%s",
                      g_app_config.m_investor_id,
			          pInputOrder->InvestorID, 
			          pInputOrder->InstrumentID,
			          pInputOrder->OrderRef,
			          pInputOrder->Direction, 
			          pInputOrder->CombOffsetFlag, 
			          pInputOrder->LimitPrice,
			          pInputOrder->VolumeTotalOriginal,
                      pRspInfo->ErrorMsg);
		g_log_file->WriteLogEx(ITS_LOG_LEVEL_ERROR,"%s", (char*)(g_text.GetBuffer()));
			SendMessageA(g_dlg_handle, WM_UPDATE_TEXT, 0,  (LPARAM)(void*)(char*)(g_text.GetBuffer()));

		g_log_file->WriteLogEx(ITS_LOG_LEVEL_DEBUG, "");

    }
    else
    {
        g_log_file->WriteLogEx(ITS_LOG_LEVEL_ERROR, "帐号[%s]报单委托返回空应答,请求号:%d", g_app_config.m_investor_id, nRequestID);
    }
}


// 成交通知
void CTraderSpi::OnRtnTrade(CThostFtdcTradeField *pTrade)
{
    // 记录成交日志
		g_text.Format("账号[%s]成交回报(CTP);"
					  "InvestorID: %s, OrderRef:%s, InstrumentID:%s, ExchangeID:%s, OrderSysID:%s, "
					  "TradeID:%s, Direction:%c, OffsetFlag:%c, Price:%f, Volume:%d, TradeDate:%s, TradeTime%s",
					  g_app_config.m_investor_id,
					  pTrade->InvestorID,
					  pTrade->OrderRef,
					  pTrade->InstrumentID,
					  pTrade->ExchangeID,
					  pTrade->OrderSysID,
					  pTrade->TradeID,
					  pTrade->Direction,
					  pTrade->OffsetFlag,
					  pTrade->Price,
					  pTrade->Volume,
					  pTrade->TradeDate,
					  pTrade->TradeTime);
		g_log_file->WriteLogEx(ITS_LOG_LEVEL_DEBUG,"%s", (char*)(g_text.GetBuffer()));
			SendMessageA(g_dlg_handle, WM_UPDATE_TEXT, 0,  (LPARAM)(void*)(char*)(g_text.GetBuffer()));
}


// 委托回报
void CTraderSpi::OnRtnOrder(CThostFtdcOrderField *pOrder)
{
	// 推送委托回报
	OnRtnOrderPush(pOrder, false);
}

// 委托信息推送
bool CTraderSpi::OnRtnOrderPush(CThostFtdcOrderField *pOrder, bool is_query)
{    
		g_text.Format("帐号[%s]收到%s委托回报(CTP): InvestorID:%s, OrderRef:%s, InstrumentID:%s, ExchangeID:%s, StatusMsg:%s, OrderStatus:%c, OrderSysID:%s, OrderLocalID:%s, "
											   "SessionID:%d,FrontID:%d, Direction:%c, CombOffsetFlag:%s, CombHedgeFlag:%s, LimitPrice:%f, VolumeTotalOriginal:%d, VolumeTotal:%d, VolumeTraded:%d, InsertTime:%s,"
										       "OrderPriceType:%d,TimeCondition:%d",
                  g_app_config.m_investor_id,
				  is_query ? "查询" : "推送",
		          pOrder->InvestorID,
                  pOrder->OrderRef,
		          pOrder->InstrumentID,
		          pOrder->ExchangeID,
		          pOrder->StatusMsg, 
                  pOrder->OrderStatus,
		          pOrder->OrderSysID,
                  pOrder->OrderLocalID,
		          pOrder->SessionID,
				  pOrder->FrontID,
		          pOrder->Direction, 
		          pOrder->CombOffsetFlag, 
				  pOrder->CombHedgeFlag,
		          pOrder->LimitPrice,
		          pOrder->VolumeTotalOriginal,
		          pOrder->VolumeTotal,
		          pOrder->VolumeTraded,
		          pOrder->InsertTime,
				  pOrder->OrderPriceType,
				  pOrder->TimeCondition);
		g_log_file->WriteLogEx(ITS_LOG_LEVEL_DEBUG,"%s", (char*)(g_text.GetBuffer()));
			SendMessageA(g_dlg_handle, WM_UPDATE_TEXT, 0,  (LPARAM)(void*)(char*)(g_text.GetBuffer()));
		return 0;
}