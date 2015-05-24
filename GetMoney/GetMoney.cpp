
// GetMoney.cpp : ����Ӧ�ó��������Ϊ��
//

#include "stdafx.h"
#include "GetMoney.h"
#include "GetMoneyDlg.h"

#include "logfile.h"
#include "ApplicationConfig.h"
#include "ThostFtdcMdApi.h"
#include "ThostFtdcTraderApi.h"
#include "MdSpi.h"
#include "TraderSpi.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif



CLogFile*				g_log_file;
ApplicationConfig		g_app_config;

// ��������api 
CThostFtdcMdApi*						g_md_user_api		= NULL; 

// ���ڽ���api
CThostFtdcTraderApi*					g_trader_user_api	= NULL;

// ����
CString g_text;

HWND g_dlg_handle;


// �Ƿ��ѵ�¼
bool  g_can_trading			= false;

// CGetMoneyApp

BEGIN_MESSAGE_MAP(CGetMoneyApp, CWinApp)
	ON_COMMAND(ID_HELP, &CWinApp::OnHelp)
END_MESSAGE_MAP()


// CGetMoneyApp ����

CGetMoneyApp::CGetMoneyApp()
{
	// TODO: �ڴ˴���ӹ�����룬
	// ��������Ҫ�ĳ�ʼ�������� InitInstance ��

	m_request_no = 0;
}


// Ψһ��һ�� CGetMoneyApp ����

CGetMoneyApp theApp;


// CGetMoneyApp ��ʼ��

BOOL CGetMoneyApp::InitInstance()
{
	CWinApp::InitInstance();


	// ���� shell ���������Է��Ի������
	// �κ� shell ����ͼ�ؼ��� shell �б���ͼ�ؼ���
	CShellManager *pShellManager = new CShellManager;

	// ��׼��ʼ��
	// ���δʹ����Щ���ܲ�ϣ����С
	// ���տ�ִ���ļ��Ĵ�С����Ӧ�Ƴ�����
	// ����Ҫ���ض���ʼ������
	// �������ڴ洢���õ�ע�����
	// TODO: Ӧ�ʵ��޸ĸ��ַ�����
	// �����޸�Ϊ��˾����֯��
	SetRegistryKey(_T("Ӧ�ó��������ɵı���Ӧ�ó���"));

	int status = 0;
	// ����SPI
	CMdSpi*									 m_md_user_spi		= NULL;

	// ����SPI
	CTraderSpi*								 m_trader_user_spi  = NULL;

	// ��־��ʼ��
	// ����־
	g_log_file = new CLogFile();
	status = g_log_file->Open("Logs", 0);
	if (!status)
	{
		printf("��־�ļ���ʧ��.�����˳�.\n");
		return false;
	}
	// ������־��¼�ȼ�
	SetLogShowLevel(0);
	g_log_file->SetFlushDelay(0);

	g_log_file->WriteLogEx(ITS_LOG_LEVEL_INFO, "����������.");
    // ��ȡ��������
	if (g_app_config.LoadConfig("config.ini"))
	{
		g_log_file->WriteLogEx(ITS_LOG_LEVEL_ERROR, "�����ļ���ʧ��.�����˳�.\n");
        return -1;
	}

	
	// ���׳�ʼ��
	// ��ʼ������API
	g_text.Format("��ʼ��CTP����API�� ���ӵ�ַ��%s", g_app_config.m_trader_front_addr);
	g_log_file->WriteLogEx(ITS_LOG_LEVEL_INFO,"%s", (char*)(g_text.GetBuffer()));

	// ��ʼ������API
	g_trader_user_api = CThostFtdcTraderApi::CreateFtdcTraderApi();			
	m_trader_user_spi = new CTraderSpi();
	// ע���¼���
	g_trader_user_api->RegisterSpi((CThostFtdcTraderSpi*)m_trader_user_spi);			
	// ����
	g_trader_user_api->RegisterFront(g_app_config.m_trader_front_addr);							
	// ��ʼ��
	g_trader_user_api->Init();
	g_log_file->WriteLogEx(ITS_LOG_LEVEL_INFO, "��ʼ��CTP����API���");

	CGetMoneyDlg   dlg;
	m_pMainWnd   = &dlg;

	INT_PTR nResponse = dlg.DoModal();
	if (nResponse == IDOK)
	{
		// TODO: �ڴ˷��ô����ʱ��
		//  ��ȷ�������رնԻ���Ĵ���
	}
	else if (nResponse == IDCANCEL)
	{
		// TODO: �ڴ˷��ô����ʱ��
		//  ��ȡ�������رնԻ���Ĵ���
	}

	// ɾ�����洴���� shell ��������
	if (pShellManager != NULL)
	{
		delete pShellManager;
	}

	// ɾ������api
	if (g_trader_user_api)
	{
		// �������ӶϿ��ͷ�
		g_trader_user_api->Release();
		if (m_trader_user_spi)
		{
			delete m_trader_user_spi;
			m_trader_user_spi = NULL;
		}
	}

	g_log_file->WriteLogEx(ITS_LOG_LEVEL_INFO, "�������˳�.");

	//
	Sleep(3000);
	if(g_log_file)
	{
		g_log_file->Close();

		delete g_log_file;
		g_log_file = NULL;
	}
	// ���ڶԻ����ѹرգ����Խ����� FALSE �Ա��˳�Ӧ�ó���
	//  ����������Ӧ�ó������Ϣ�á�
	return FALSE;
}


// �µ�
int  CGetMoneyApp::RequestOrder(char* contract_code, char bs, char oe)
{
	int                         status = 0;
	CThostFtdcInputOrderField   req;

   // ��սṹ��
    memset(&req,            0, sizeof(req));

    // �����
    req.RequestID                                   = ++m_request_no;
	// ���͹�˾����
	strncpy(req.BrokerID,                             g_app_config.m_broker_id,         sizeof(req.BrokerID)-1);
	// Ͷ���ߴ���
	strncpy(req.InvestorID,                           g_app_config.m_investor_id,          sizeof(req.InvestorID)-1);
	// ��Լ����
	strncpy(req.InstrumentID,                         contract_code,					 sizeof(req.InstrumentID)-1);
	// ��������
    _snprintf(req.OrderRef, 
              sizeof(req.OrderRef) - 1,        
              "%d%s",
              m_request_no,
			  "YYY"); 
	// �м۵�
	req.OrderPriceType                             = THOST_FTDC_OPT_BestPrice;
	// ������ɣ�������
	req.TimeCondition						       = THOST_FTDC_TC_IOC;
	// ��������: 
    req.Direction                                  = bs;  
	// ��Ͽ�ƽ��־: ����
	req.CombOffsetFlag[0]                          = oe;
	// Ͷ��
	req.CombHedgeFlag[0]                           = THOST_FTDC_HF_Speculation;
	
	// �۸�
    req.LimitPrice                      = 0;
	// ����:
    req.VolumeTotalOriginal             = 1;
	// �ɽ�������: �κ�����
	req.VolumeCondition                 = THOST_FTDC_VC_AV;
	// ��С�ɽ���:
	req.MinVolume                       = 1;
	// ��������: ����
	req.ContingentCondition             = THOST_FTDC_CC_Immediately;
	// ǿƽԭ��: ��ǿƽ
	req.ForceCloseReason                = THOST_FTDC_FCC_NotForceClose;
	// �Զ������־: ��
	req.IsAutoSuspend                   = 0;
	// �û�ǿ����־: ��
	req.UserForceClose                  = 0;
	///��������־
	req.IsSwapOrder                     = 0;

    // ��¼�µ�����
    char direction[2]                   = {0};
    direction[0] =  req.Direction;

	// �Ǽ�������־
	g_log_file->WriteLogEx(ITS_LOG_LEVEL_DEBUG,
				  "�ʻ�[%s]ί���µ�����(CTP);RequestID:%d, OrderRef:%s, InstrumentID:%s, BrokerID:%s, "
                  "InvestorID:%s, Direction:%s, CombOffsetFlag:%s, CombHedgeFlag:%s, LimitPrice:%f, VolumeTotalOriginal:%d"
				  "OrderPriceType:%d,TimeCondition:%d,IsSwapOrder:%d",
                  g_app_config.m_investor_id, req.RequestID, req.OrderRef, req.InstrumentID, req.BrokerID,
				   req.InvestorID,direction,req.CombOffsetFlag,req.CombHedgeFlag,req.LimitPrice,req.VolumeTotalOriginal,
				   req.OrderPriceType,req.TimeCondition, req.IsSwapOrder);

    // ί���µ�����
    status = g_trader_user_api->ReqOrderInsert(&req, req.RequestID);
    if (status != 0)
    {
		status = -1;

		g_text.Format("�ʻ�[%s]����ί���µ�����%dʧ��(CTP), ����״̬: %d.", 
		              g_app_config.m_investor_id,
					  req.RequestID,
                      status);
		g_log_file->WriteLogEx(ITS_LOG_LEVEL_ERROR,"%s", (char*)(g_text.GetBuffer()));
			SendMessageA(g_dlg_handle, WM_UPDATE_TEXT, 0,  (LPARAM)(void*)(char*)(g_text.GetBuffer()));
    }

	return status;

}