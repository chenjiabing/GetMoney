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
//   ���ļ��������ں�ǰ�ý������ӣ���ȡ֤ȯ���룬�������趨��ʱ�������֤ȯ�������
//
//-----------------------------------------------------------------------------
#include "stdafx.h"
#include "public.h"
#include "TraderSpi.h"
#include "ApplicationConfig.h"
#include "GetMoneyDlg.h"


// �Ự����
TThostFtdcFrontIDType	FRONT_ID;	//ǰ�ñ��
TThostFtdcSessionIDType	SESSION_ID;	//�Ự���
TThostFtdcOrderRefType	ORDER_REF;	//��������

// ���������
int													g_request_id				= 0;

// ���ڽ���api
extern      CThostFtdcTraderApi*					g_trader_user_api;
// ��������
extern      ApplicationConfig						g_app_config;
// �Ƿ�ɽ���
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

		// ���ӳɹ�
	g_text.Format("��CTP��̨API���ӳɹ�(����API)�����Ӽ���:%d", reconnect_count++);
	g_log_file->WriteLogEx(ITS_LOG_LEVEL_INFO,"%s", (char*)(g_text.GetBuffer()));
		
	// �û���¼����
	ReqUserLogin();

}


//-----------------------------------------------------------------------------
// ������   :    OnFrontDisconnected
//-----------------------------------------------------------------------------
//
// �������� :    ���ӶϿ�
// ����     ��   nReason         - �������
// ˵��     :    ϵͳ�Զ����ӣ�����Ҫ����ά��
//
//-----------------------------------------------------------------------------
void CTraderSpi:: OnFrontDisconnected(int nReason)
{
	static int reconnect_count = 1;

	g_can_trading = false;

	g_text.Format("��CTP��̨ǰ�����ӶϿ�(����API)��������룺%d, ���Ӽ���:%d", nReason, reconnect_count++);
	SendMessageA(g_dlg_handle, WM_UPDATE_TEXT, 0,  (LPARAM)(void*)(char*)(g_text.GetBuffer()));
	g_log_file->WriteLogEx(ITS_LOG_LEVEL_WARN,"%s", (char*)(g_text.GetBuffer()));
}
		

//-----------------------------------------------------------------------------
// ������   :    ReqUserLogin
//-----------------------------------------------------------------------------
//
// �������� :    ���׵�¼����
// ˵��     : 
//-----------------------------------------------------------------------------
int CTraderSpi::ReqUserLogin()
{
	int							status = 0;
	CThostFtdcReqUserLoginField req;

	memset(&req, 0, sizeof(req));
	
	strcpy(req.BrokerID, g_app_config.m_broker_id);
	strcpy(req.UserID,   g_app_config.m_investor_id);
	strcpy(req.Password, g_app_config.m_password);
	
    // ��ͨ�û���¼
    status  = g_trader_user_api->ReqUserLogin(&req,  ++g_request_id);
    if (status != 0)
    {
		g_text.Format("�ʻ�[%s]�����û���¼CTP��̨����ʧ��(����API), ����״̬: %d.", 
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
   // ��¼״̬��֤
	if (pRspInfo)
	{
		// ����Ự����
		FRONT_ID			= pRspUserLogin->FrontID;
		SESSION_ID			= pRspUserLogin->SessionID;

		// ��ȡ��������
		if(pRspInfo->ErrorID == 0)
		{
            // ���㵥ȷ������
			ReqSettlementInfoConfirm();
		}
		else
		{
            // ��¼��¼��־
            g_log_file->WriteLogEx(ITS_LOG_LEVEL_ERROR, 
									"�ʺ�[%s]��¼CTP��̨ʧ��(����API),ʧ����Ϣ:%s",   
									g_app_config.m_investor_id,
									pRspInfo->ErrorMsg);
		}
	}
    else
    {
        g_log_file->WriteLogEx(ITS_LOG_LEVEL_ERROR, 
							  "�ʺ�[%s]�����¼���ؿ�Ӧ��(����API),�����:%d", 
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

    // ������ID
	strcpy(req.BrokerID,        g_app_config.m_broker_id);
    // Ͷ����ID
	strcpy(req.InvestorID,      g_app_config.m_investor_id);

	while (times > 0)
	{
		 --times;

		// ���㵥ȷ��
		status = g_trader_user_api->ReqSettlementInfoConfirm(&req, ++g_request_id);
		if (status)
		{
			g_log_file->WriteLogEx(ITS_LOG_LEVEL_ERROR, 
								  "�ʻ�[%s]���ͽ��㵥ȷ������ʧ��(����API), ����״̬: %d.", 
								  g_app_config.m_investor_id,
								  status);
			Sleep(1000);
		}
		else
		{
			// ���㵥ȷ�ϳɹ�
			break;
		}
	}

	return status;
}

void CTraderSpi::OnRspSettlementInfoConfirm(CThostFtdcSettlementInfoConfirmField *pSettlementInfoConfirm, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
    // ���㵥ȷ����Ϣ������֤
	if (pRspInfo)
	{
		// ��ȡ��������
		if(pRspInfo->ErrorID == 0)
		{
            // ��¼���㵥ȷ����־
			g_log_file->WriteLogEx(ITS_LOG_LEVEL_INFO, "�ʺ�[%s] CTPȷ�Ͻ��㵥��Ϣ��Ӧ�ɹ�(����API)", g_app_config.m_investor_id);

			// �ɽ���
			g_can_trading = true;
		}
		else
		{
            // ��¼���㵥ȷ����־
             g_log_file->WriteLogEx(ITS_LOG_LEVEL_ERROR, 
									"�ʺ�[%s] CTPȷ�Ͻ��㵥��Ϣ��Ӧʧ��(����API),ʧ����Ϣ:%s",   
									g_app_config.m_investor_id,
									pRspInfo->ErrorMsg);
		}
    }
}

//-----------------------------------------------------------------------------
// ������   :    ReqQryInstrument
//-----------------------------------------------------------------------------
//
// �������� :    ���Ͳ�ѯ���룬��ǰ�û�ȡȫ���ĺ�Լ����
// ˵��     : 
//-----------------------------------------------------------------------------
int CTraderSpi::ReqQryInstrument()
{
	int							 times				= 5;
	int                          status             = 0;
	CThostFtdcQryInstrumentField req;
	
	// �������ṹ��
	// NOTE����Լ����Instructments��ռ������к�Լ
	memset(&req, 0, sizeof(req));
	
	// ��ѯ��Լ�б�����(��ೢ��5������)
	while (times > 0)
	{
		 --times;

		// ��ѯ��Լ�б�
		status = g_trader_user_api->ReqQryInstrument(&req, ++g_request_id);
		if (status)
		{
			g_log_file->WriteLogEx(ITS_LOG_LEVEL_ERROR, "�����ѯ��Լ�б�ʧ�ܣ��ܵ�����(����API)");
			Sleep(1000);
		}
		else
		{
			// ��ѯ�ɹ�
			break;
		}
	}

	return status;
}


//-----------------------------------------------------------------------------
// ������   :    OnRspQryInstrument
//-----------------------------------------------------------------------------
//
// �������� :    ��Լ���Ų�ѯ����Ļص����������Ի�ȡ���еĺ�Լ����
// ���ֵ   ��   pRspInfo         - ��Լ��Ϣ�ṹ��
// ˵��     :    1�������ǰ��֪���ܹ���ȡ����Ŀ����������Ͳ�ѯ�������Ҫsleep����ʱ��
//             ��ȷ����������ȡ��ȫ���ĺ�Լ���룬ʱ������á�
//               2�������������������һ������֮����һ����ʱ�����²�ѯ���и��������¶���
//-----------------------------------------------------------------------------
void CTraderSpi::OnRspQryInstrument(CThostFtdcInstrumentField *pInstrument, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{

}


//-----------------------------------------------------------------------------
// ������   :    OnRspError
//-----------------------------------------------------------------------------
//
// �������� :    �û��������
// ����     ��   pRspInfo         - �����Ĵ�����Ϣ�ṹ��
// ˵��     :    
//
//-----------------------------------------------------------------------------
void CTraderSpi::OnRspError(CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
	g_log_file->WriteLogEx(ITS_LOG_LEVEL_ERROR,"�û������������API��������ԭ��%s", pRspInfo->ErrorMsg);
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
	//g_log_file->WriteLogEx(ITS_LOG_LEVEL_INFO, "�����û��ǳ�����: %s", ((result == 0) ? "�ɹ�" : "ʧ��"));
}

void CTraderSpi::OnRspUserLogout(CThostFtdcUserLogoutField *pUserLogout,
								CThostFtdcRspInfoField     *pRspInfo,
								int                        nRequestID,
								bool                       bIsLast)
{
	//g_log_file->WriteLogEx(ITS_LOG_LEVEL_DEBUG,"��Լ��ѯ:OnRspUserLogout");
	//bool bResult = ((pRspInfo) && (pRspInfo->ErrorID != 0));
	//
	//if (bIsLast && !bResult)
	//{
	//	g_log_file->WriteLogEx(ITS_LOG_LEVEL_INFO,"��Լ��ѯ��ϣ�����Ա�ǳ��ɹ�");
	//	m_break_flag = true;
	//}
	//else
	//{
	//	g_log_file->WriteLogEx(ITS_LOG_LEVEL_INFO,"��Լ��ѯ��ϣ�����Ա�ǳ�ʧ�ܣ� ʧ����Ϣ��%s", pRspInfo->ErrorMsg);
	//}
}



// ���Ϳͻ���֤����
int CTraderSpi::ReqAuthenticate()
{
	int                              status = 0;
	CThostFtdcReqAuthenticateField   reqAuth;
	
	// ��ʼ��
	memset(&reqAuth, 0, sizeof(reqAuth));

	// ������
	strcpy(reqAuth.BrokerID,           g_app_config.m_broker_id);
	// �û���
	strcpy(reqAuth.UserID,             g_app_config.m_investor_id);

	// �ͻ�����֤����
	status = g_trader_user_api->ReqAuthenticate(&reqAuth, ++g_request_id);
    if (status != 0)
    {
	    g_log_file->WriteLogEx(ITS_LOG_LEVEL_ERROR, 
                      "�ʻ�[%s]���Ϳͻ�����֤����ʧ��(CTP), ����״̬: %d.", 
		              g_app_config.m_broker_id,
                      status);
        status = -1;
    }

	return status;
}


///�ͻ�����֤��Ӧ
void CTraderSpi::OnRspAuthenticate(CThostFtdcRspAuthenticateField *pRspAuthenticateField, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
    // ��¼״̬��֤
	if (pRspInfo)
	{
		// ��ȡ��������
		if(pRspInfo->ErrorID == 0)
		{
            // ��¼��¼��־
			g_log_file->WriteLogEx(ITS_LOG_LEVEL_INFO, "�ʺ�[%s]CTP�ͻ�����֤��Ӧ�ɹ�", g_app_config.m_investor_id);
			
			// �û���¼����
			ReqUserLogin();
		}
		else
		{
            // ��¼��¼��־
            g_log_file->WriteLogEx(ITS_LOG_LEVEL_INFO, 
				          "�ʺ�[%s]CTP�ͻ�����֤��Ӧʧ��,ʧ����Ϣ:%s",  
						  g_app_config.m_investor_id, pRspInfo->ErrorMsg);
		}
	}
    else
    {
        g_log_file->WriteLogEx(ITS_LOG_LEVEL_ERROR, 
                      "�ʺ�[%s]�����¼��֤���ؿ�Ӧ��,�����:%d",
					  g_app_config.m_investor_id,
                      nRequestID);
    }

}


// ί��Ӧ��
void CTraderSpi::OnRspOrderInsert(CThostFtdcInputOrderField *pInputOrder, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
    if (pRspInfo && pRspInfo->ErrorID != 0)
    {
		g_text.Format( "�ʺ�[%s]ί��ʧ��֪ͨ(CTP); InvestorID: %s, InstrumentID:%s, OrderRef:%s, Direction:%c, "
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
        g_log_file->WriteLogEx(ITS_LOG_LEVEL_ERROR, "�ʺ�[%s]����ί�з��ؿ�Ӧ��,�����:%d", g_app_config.m_investor_id, nRequestID);
    }
}


// �ɽ�֪ͨ
void CTraderSpi::OnRtnTrade(CThostFtdcTradeField *pTrade)
{
    // ��¼�ɽ���־
		g_text.Format("�˺�[%s]�ɽ��ر�(CTP);"
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


// ί�лر�
void CTraderSpi::OnRtnOrder(CThostFtdcOrderField *pOrder)
{
	// ����ί�лر�
	OnRtnOrderPush(pOrder, false);
}

// ί����Ϣ����
bool CTraderSpi::OnRtnOrderPush(CThostFtdcOrderField *pOrder, bool is_query)
{    
		g_text.Format("�ʺ�[%s]�յ�%sί�лر�(CTP): InvestorID:%s, OrderRef:%s, InstrumentID:%s, ExchangeID:%s, StatusMsg:%s, OrderStatus:%c, OrderSysID:%s, OrderLocalID:%s, "
											   "SessionID:%d,FrontID:%d, Direction:%c, CombOffsetFlag:%s, CombHedgeFlag:%s, LimitPrice:%f, VolumeTotalOriginal:%d, VolumeTotal:%d, VolumeTraded:%d, InsertTime:%s,"
										       "OrderPriceType:%d,TimeCondition:%d",
                  g_app_config.m_investor_id,
				  is_query ? "��ѯ" : "����",
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