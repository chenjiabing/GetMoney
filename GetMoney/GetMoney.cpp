
// GetMoney.cpp : 定义应用程序的类行为。
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

// 上期行情api 
CThostFtdcMdApi*						g_md_user_api		= NULL; 

// 上期交易api
CThostFtdcTraderApi*					g_trader_user_api	= NULL;

// 描述
CString g_text;

HWND g_dlg_handle;


// 是否已登录
bool  g_can_trading			= false;

// CGetMoneyApp

BEGIN_MESSAGE_MAP(CGetMoneyApp, CWinApp)
	ON_COMMAND(ID_HELP, &CWinApp::OnHelp)
END_MESSAGE_MAP()


// CGetMoneyApp 构造

CGetMoneyApp::CGetMoneyApp()
{
	// TODO: 在此处添加构造代码，
	// 将所有重要的初始化放置在 InitInstance 中

	m_request_no = 0;
}


// 唯一的一个 CGetMoneyApp 对象

CGetMoneyApp theApp;


// CGetMoneyApp 初始化

BOOL CGetMoneyApp::InitInstance()
{
	CWinApp::InitInstance();


	// 创建 shell 管理器，以防对话框包含
	// 任何 shell 树视图控件或 shell 列表视图控件。
	CShellManager *pShellManager = new CShellManager;

	// 标准初始化
	// 如果未使用这些功能并希望减小
	// 最终可执行文件的大小，则应移除下列
	// 不需要的特定初始化例程
	// 更改用于存储设置的注册表项
	// TODO: 应适当修改该字符串，
	// 例如修改为公司或组织名
	SetRegistryKey(_T("应用程序向导生成的本地应用程序"));

	int status = 0;
	// 行情SPI
	CMdSpi*									 m_md_user_spi		= NULL;

	// 交易SPI
	CTraderSpi*								 m_trader_user_spi  = NULL;

	// 日志初始化
	// 打开日志
	g_log_file = new CLogFile();
	status = g_log_file->Open("Logs", 0);
	if (!status)
	{
		printf("日志文件打开失败.程序退出.\n");
		return false;
	}
	// 设置日志记录等级
	SetLogShowLevel(0);
	g_log_file->SetFlushDelay(0);

	g_log_file->WriteLogEx(ITS_LOG_LEVEL_INFO, "程序已启动.");
    // 读取本地配置
	if (g_app_config.LoadConfig("config.ini"))
	{
		g_log_file->WriteLogEx(ITS_LOG_LEVEL_ERROR, "配置文件打开失败.程序退出.\n");
        return -1;
	}

	
	// 交易初始化
	// 初始化交易API
	g_text.Format("初始化CTP交易API， 连接地址：%s", g_app_config.m_trader_front_addr);
	g_log_file->WriteLogEx(ITS_LOG_LEVEL_INFO,"%s", (char*)(g_text.GetBuffer()));

	// 初始化交易API
	g_trader_user_api = CThostFtdcTraderApi::CreateFtdcTraderApi();			
	m_trader_user_spi = new CTraderSpi();
	// 注册事件类
	g_trader_user_api->RegisterSpi((CThostFtdcTraderSpi*)m_trader_user_spi);			
	// 连接
	g_trader_user_api->RegisterFront(g_app_config.m_trader_front_addr);							
	// 初始化
	g_trader_user_api->Init();
	g_log_file->WriteLogEx(ITS_LOG_LEVEL_INFO, "初始化CTP交易API完成");

	CGetMoneyDlg   dlg;
	m_pMainWnd   = &dlg;

	INT_PTR nResponse = dlg.DoModal();
	if (nResponse == IDOK)
	{
		// TODO: 在此放置处理何时用
		//  “确定”来关闭对话框的代码
	}
	else if (nResponse == IDCANCEL)
	{
		// TODO: 在此放置处理何时用
		//  “取消”来关闭对话框的代码
	}

	// 删除上面创建的 shell 管理器。
	if (pShellManager != NULL)
	{
		delete pShellManager;
	}

	// 删除交易api
	if (g_trader_user_api)
	{
		// 交易连接断开释放
		g_trader_user_api->Release();
		if (m_trader_user_spi)
		{
			delete m_trader_user_spi;
			m_trader_user_spi = NULL;
		}
	}

	g_log_file->WriteLogEx(ITS_LOG_LEVEL_INFO, "程序已退出.");

	//
	Sleep(3000);
	if(g_log_file)
	{
		g_log_file->Close();

		delete g_log_file;
		g_log_file = NULL;
	}
	// 由于对话框已关闭，所以将返回 FALSE 以便退出应用程序，
	//  而不是启动应用程序的消息泵。
	return FALSE;
}


// 下单
int  CGetMoneyApp::RequestOrder(char* contract_code, char bs, char oe)
{
	int                         status = 0;
	CThostFtdcInputOrderField   req;

   // 清空结构体
    memset(&req,            0, sizeof(req));

    // 请求号
    req.RequestID                                   = ++m_request_no;
	// 经纪公司代码
	strncpy(req.BrokerID,                             g_app_config.m_broker_id,         sizeof(req.BrokerID)-1);
	// 投资者代码
	strncpy(req.InvestorID,                           g_app_config.m_investor_id,          sizeof(req.InvestorID)-1);
	// 合约代码
	strncpy(req.InstrumentID,                         contract_code,					 sizeof(req.InstrumentID)-1);
	// 报单引用
    _snprintf(req.OrderRef, 
              sizeof(req.OrderRef) - 1,        
              "%d%s",
              m_request_no,
			  "YYY"); 
	// 市价单
	req.OrderPriceType                             = THOST_FTDC_OPT_BestPrice;
	// 立即完成，否则撤销
	req.TimeCondition						       = THOST_FTDC_TC_IOC;
	// 买卖方向: 
    req.Direction                                  = bs;  
	// 组合开平标志: 开仓
	req.CombOffsetFlag[0]                          = oe;
	// 投机
	req.CombHedgeFlag[0]                           = THOST_FTDC_HF_Speculation;
	
	// 价格
    req.LimitPrice                      = 0;
	// 数量:
    req.VolumeTotalOriginal             = 1;
	// 成交量类型: 任何数量
	req.VolumeCondition                 = THOST_FTDC_VC_AV;
	// 最小成交量:
	req.MinVolume                       = 1;
	// 触发条件: 立即
	req.ContingentCondition             = THOST_FTDC_CC_Immediately;
	// 强平原因: 非强平
	req.ForceCloseReason                = THOST_FTDC_FCC_NotForceClose;
	// 自动挂起标志: 否
	req.IsAutoSuspend                   = 0;
	// 用户强评标志: 否
	req.UserForceClose                  = 0;
	///互换单标志
	req.IsSwapOrder                     = 0;

    // 记录下单数据
    char direction[2]                   = {0};
    direction[0] =  req.Direction;

	// 登记请求到日志
	g_log_file->WriteLogEx(ITS_LOG_LEVEL_DEBUG,
				  "帐户[%s]委托下单参数(CTP);RequestID:%d, OrderRef:%s, InstrumentID:%s, BrokerID:%s, "
                  "InvestorID:%s, Direction:%s, CombOffsetFlag:%s, CombHedgeFlag:%s, LimitPrice:%f, VolumeTotalOriginal:%d"
				  "OrderPriceType:%d,TimeCondition:%d,IsSwapOrder:%d",
                  g_app_config.m_investor_id, req.RequestID, req.OrderRef, req.InstrumentID, req.BrokerID,
				   req.InvestorID,direction,req.CombOffsetFlag,req.CombHedgeFlag,req.LimitPrice,req.VolumeTotalOriginal,
				   req.OrderPriceType,req.TimeCondition, req.IsSwapOrder);

    // 委托下单请求
    status = g_trader_user_api->ReqOrderInsert(&req, req.RequestID);
    if (status != 0)
    {
		status = -1;

		g_text.Format("帐户[%s]发送委托下单请求%d失败(CTP), 返回状态: %d.", 
		              g_app_config.m_investor_id,
					  req.RequestID,
                      status);
		g_log_file->WriteLogEx(ITS_LOG_LEVEL_ERROR,"%s", (char*)(g_text.GetBuffer()));
			SendMessageA(g_dlg_handle, WM_UPDATE_TEXT, 0,  (LPARAM)(void*)(char*)(g_text.GetBuffer()));
    }

	return status;

}