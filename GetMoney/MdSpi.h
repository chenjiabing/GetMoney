#pragma once
#include <ThostFtdcMdApi.h>

class CMdSpi : public CThostFtdcMdSpi
{
public:
	CMdSpi();
	virtual ~CMdSpi();

	///错误应答
	virtual void OnRspError(CThostFtdcRspInfoField *pRspInfo,
							int nRequestID,
							bool bIsLast);

	///当客户端与交易后台通信连接断开时，该方法被调用。当发生这个情况后，API会自动重新连接，客户端可不做处理。
	///@param nReason 错误原因
	///        0x1001 网络读失败
	///        0x1002 网络写失败
	///        0x2001 接收心跳超时
	///        0x2002 发送心跳失败
	///        0x2003 收到错误报文
	virtual void OnFrontDisconnected(int nReason);

	///当客户端与交易后台建立起通信连接时（还未登录前），该方法被调用。
	virtual void OnFrontConnected();
	
	///登录请求响应
	virtual void OnRspUserLogin(CThostFtdcRspUserLoginField *pRspUserLogin, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);

	///订阅行情应答
	virtual void OnRspSubMarketData(CThostFtdcSpecificInstrumentField *pSpecificInstrument, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);

	///取消订阅行情应答
	virtual void OnRspUnSubMarketData(CThostFtdcSpecificInstrumentField *pSpecificInstrument, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);

	///深度行情通知
	virtual void OnRtnDepthMarketData(CThostFtdcDepthMarketDataField *pDepthMarketData);

	// 下单
	int RequestOrder(char* contract_code, double price, char bs, char oe);


private:

	// 委托请求No
	int		m_request_no;

	// 是否有开仓
	int 	m_open_status;

	// 开仓价格
	double	m_open_price;
	// 开仓方向
	char    m_open_buy;
	// 登录
	int ReqUserLogin();
	// 订阅行情
	int SubscribeMarketData();
};