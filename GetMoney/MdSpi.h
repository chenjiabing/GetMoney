#pragma once
#include <ThostFtdcMdApi.h>

class CMdSpi : public CThostFtdcMdSpi
{
public:
	CMdSpi();
	virtual ~CMdSpi();

	///����Ӧ��
	virtual void OnRspError(CThostFtdcRspInfoField *pRspInfo,
							int nRequestID,
							bool bIsLast);

	///���ͻ����뽻�׺�̨ͨ�����ӶϿ�ʱ���÷��������á���������������API���Զ��������ӣ��ͻ��˿ɲ�������
	///@param nReason ����ԭ��
	///        0x1001 �����ʧ��
	///        0x1002 ����дʧ��
	///        0x2001 ����������ʱ
	///        0x2002 ��������ʧ��
	///        0x2003 �յ�������
	virtual void OnFrontDisconnected(int nReason);

	///���ͻ����뽻�׺�̨������ͨ������ʱ����δ��¼ǰ�����÷��������á�
	virtual void OnFrontConnected();
	
	///��¼������Ӧ
	virtual void OnRspUserLogin(CThostFtdcRspUserLoginField *pRspUserLogin, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);

	///��������Ӧ��
	virtual void OnRspSubMarketData(CThostFtdcSpecificInstrumentField *pSpecificInstrument, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);

	///ȡ����������Ӧ��
	virtual void OnRspUnSubMarketData(CThostFtdcSpecificInstrumentField *pSpecificInstrument, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);

	///�������֪ͨ
	virtual void OnRtnDepthMarketData(CThostFtdcDepthMarketDataField *pDepthMarketData);

	// �µ�
	int RequestOrder(char* contract_code, double price, char bs, char oe);


private:

	// ί������No
	int		m_request_no;

	// �Ƿ��п���
	int 	m_open_status;

	// ���ּ۸�
	double	m_open_price;
	// ���ַ���
	char    m_open_buy;
	// ��¼
	int ReqUserLogin();
	// ��������
	int SubscribeMarketData();
};