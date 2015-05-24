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
//                         �����ʷ
//-----------------------------------------------------------------------------
//      
//         ����          �޸���   �������       ����
//         ----------    ------   --------       --------------
//         2013-11-25    ���췢   t29	         ��ϵͳCTP���飬֤ȯ���벻�ٴ��м��ȡ��Լ
//
//-----------------------------------------------------------------------------
//                          �ļ�����
//-----------------------------------------------------------------------------
//
//   ���ļ����ں�ǰ�ý������ӣ���ȡ֤ȯ�����������ͷ�ļ�
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
	//���캯��
	CTraderSpi();
	//��������
	virtual ~CTraderSpi();

	///���ͻ����뽻�׺�̨������ͨ������ʱ����δ��¼ǰ�����÷��������á�
	virtual void OnFrontConnected();

	///��¼������Ӧ
	virtual void OnRspUserLogin(CThostFtdcRspUserLoginField *pRspUserLogin,	CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);
		
	///Ͷ���߽�����ȷ����Ӧ
	virtual void OnRspSettlementInfoConfirm(CThostFtdcSettlementInfoConfirmField *pSettlementInfoConfirm, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);
	
	///�����ѯ��Լ��Ӧ
	virtual void OnRspQryInstrument(CThostFtdcInstrumentField *pInstrument, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);

	///����Ӧ��
	virtual void OnRspError(CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);
	
	///���ͻ����뽻�׺�̨ͨ�����ӶϿ�ʱ���÷��������á���������������API���Զ��������ӣ��ͻ��˿ɲ�������
	virtual void OnFrontDisconnected(int nReason);
		
	//����ǳ�
	void RquestLogout();

	virtual void OnRspUserLogout(CThostFtdcUserLogoutField *pUserLogout, CThostFtdcRspInfoField *pRspInfo,int nRequestID,bool bIsLast);

	//�ͻ�����֤��Ӧ
	virtual void OnRspAuthenticate(CThostFtdcRspAuthenticateField *pRspAuthenticateField, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);

	virtual void OnRspOrderInsert(CThostFtdcInputOrderField *pInputOrder, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);

	// �ɽ�֪ͨ
	virtual void OnRtnTrade(CThostFtdcTradeField *pTrade);

	// ί�лر�
	virtual void OnRtnOrder(CThostFtdcOrderField *pOrder);
	// ί����Ϣ����
	bool OnRtnOrderPush(CThostFtdcOrderField *pOrder, bool is_query);

private:


	///�û���¼����
	int ReqUserLogin();
	///Ͷ���߽�����ȷ��
	int ReqSettlementInfoConfirm();
	///�����ѯ��Լ
	int ReqQryInstrument();
	// �û���֤����
	int ReqAuthenticate();

	//����Ա�ǳ�����
	int ReqUserLogout(CThostFtdcUserLogoutField *pUserLogout, int nRequestID);
};
