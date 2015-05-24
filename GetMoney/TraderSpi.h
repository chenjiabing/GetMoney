//-----------------------------------------------------------------------------
//                            TraderSpi.h
//-----------------------------------------------------------------------------
//
//         File Name       : TraderSpi.h
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
//   此文件用于和前置建立连接，获取证券代码声明类的头文件
//
//-----------------------------------------------------------------------------

#pragma once
#include "public.h"
#include <ThostFtdcTraderApi.h>
#include <Windows.h>
#include <map>
#include <string>
#include <iostream>


using namespace std;

class CTraderSpi : public CThostFtdcTraderSpi
{
public:
	//构造函数
	CTraderSpi();
	//析构函数
	virtual ~CTraderSpi();

	///当客户端与交易后台建立起通信连接时（还未登录前），该方法被调用。
	virtual void OnFrontConnected();

	///登录请求响应
	virtual void OnRspUserLogin(CThostFtdcRspUserLoginField *pRspUserLogin,	CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);
		
	///投资者结算结果确认响应
	virtual void OnRspSettlementInfoConfirm(CThostFtdcSettlementInfoConfirmField *pSettlementInfoConfirm, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);
	
	///请求查询合约响应
	virtual void OnRspQryInstrument(CThostFtdcInstrumentField *pInstrument, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);

	///错误应答
	virtual void OnRspError(CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);
	
	///当客户端与交易后台通信连接断开时，该方法被调用。当发生这个情况后，API会自动重新连接，客户端可不做处理。
	virtual void OnFrontDisconnected(int nReason);
		
	//请求登出
	void RquestLogout();

	virtual void OnRspUserLogout(CThostFtdcUserLogoutField *pUserLogout, CThostFtdcRspInfoField *pRspInfo,int nRequestID,bool bIsLast);

	//客户端认证响应
	virtual void OnRspAuthenticate(CThostFtdcRspAuthenticateField *pRspAuthenticateField, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);

	virtual void OnRspOrderInsert(CThostFtdcInputOrderField *pInputOrder, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);

	// 成交通知
	virtual void OnRtnTrade(CThostFtdcTradeField *pTrade);

	// 委托回报
	virtual void OnRtnOrder(CThostFtdcOrderField *pOrder);
	// 委托信息推送
	bool OnRtnOrderPush(CThostFtdcOrderField *pOrder, bool is_query);

private:


	///用户登录请求
	int ReqUserLogin();
	///投资者结算结果确认
	int ReqSettlementInfoConfirm();
	///请求查询合约
	int ReqQryInstrument();
	// 用户认证请求
	int ReqAuthenticate();

	//交易员登出请求
	int ReqUserLogout(CThostFtdcUserLogoutField *pUserLogout, int nRequestID);
};
