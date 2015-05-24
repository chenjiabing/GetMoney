
// GetMoneyDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "GetMoney.h"
#include "GetMoneyDlg.h"
#include "afxdialogex.h"
#include "ApplicationConfig.h"
#include "ThostFtdcTraderApi.h"

#include "public.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// 交易API
extern      CThostFtdcTraderApi*					        g_trader_user_api;
// CTP 请求ID
extern      int												g_request_id;
// 合约获取完成
// 程序配置
extern      ApplicationConfig								g_app_config;
// 是否是行情采集时间
extern      bool                                            g_is_working_time;
// 是否可交易
extern      bool											g_can_trading;

extern     CString  g_text;
// 用于应用程序“关于”菜单项的 CAboutDlg 对话框
extern     HWND g_dlg_handle;

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// 对话框数据
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

// 实现
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	 
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// CGetMoneyDlg 对话框




CGetMoneyDlg::CGetMoneyDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CGetMoneyDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);

	m_user = g_app_config.m_investor_id;

	//strcpy(m_user, g_app_config.m_investor_id);
	
}

void CGetMoneyDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_STATIC_USER, m_user);
	DDX_Text(pDX, IDC_STATIC_TEXT, g_text);
	DDX_Control(pDX, IDC_COMBO_CODE, m_cbCode);
}

BEGIN_MESSAGE_MAP(CGetMoneyDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BUTTON3, &CGetMoneyDlg::OnBnClickedOpenBuy)
	ON_BN_CLICKED(IDC_BUTTON1, &CGetMoneyDlg::OnBnClickedOpenSell)
	ON_BN_CLICKED(IDC_BUTTON_CLOSE_SELL, &CGetMoneyDlg::OnBnClickedClose)
	ON_BN_CLICKED(IDC_BUTTON_CLOSE_BUY, &CGetMoneyDlg::OnBnClickedButtonCloseBuy)
	ON_CBN_SELCHANGE(IDC_COMBO_CODE, &CGetMoneyDlg::OnCbnSelchangeComboCode)
	ON_CBN_EDITCHANGE(IDC_COMBO_CODE, &CGetMoneyDlg::OnCbnEditchangeComboCode)
	ON_CBN_DROPDOWN(IDC_COMBO_CODE, &CGetMoneyDlg::OnCbnDropdownComboCode)
	ON_MESSAGE(WM_UPDATE_TEXT, OnUpdateText)
END_MESSAGE_MAP()


// CGetMoneyDlg 消息处理程序

BOOL CGetMoneyDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 将“关于...”菜单项添加到系统菜单中。

	// IDM_ABOUTBOX 必须在系统命令范围内。
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// 设置此对话框的图标。当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

	// TODO: 在此添加额外的初始化代码
	this->m_cbCode.InsertString(0, g_app_config.m_contract_code);
	// 增加IH1506 IC1506
	this->m_cbCode.InsertString(1, "IH1506");
	this->m_cbCode.InsertString(2, "IC1506");

	// 选择第一个
	this->m_cbCode.SetCurSel(0);
	
	// 
	g_dlg_handle = this->GetSafeHwnd();

	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

void CGetMoneyDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CGetMoneyDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 用于绘制的设备上下文

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 使图标在工作区矩形中居中
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 绘制图标
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}


	// 初始化界面显示
	//GetDlgItem(IDC_STATIC_USER)->SetWindowTextW(LPCTSTR(g_app_config.m_investor_id));
	//GetDlgItem(IDC_STATIC_CONTRACT)->SetWindowTextW(LPCTSTR(g_app_config.m_contract_code));
}

//当用户拖动最小化窗口时系统调用此函数取得光标
//显示。
HCURSOR CGetMoneyDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}



void CGetMoneyDlg::OnBnClickedOpenBuy()
{
	// TODO: 在此添加控件通知处理程序代码
	CGetMoneyApp* app = (CGetMoneyApp*)AfxGetApp();
	if (app && g_can_trading && IsDlgButtonChecked( IDC_CHECK_SAFE_ORDER ))
	{
		CString text;
		m_cbCode.GetWindowTextA(text);

		strncpy(g_app_config.m_contract_code,  (char*)(text.GetBuffer()), sizeof(g_app_config.m_contract_code) - 1);
		app->RequestOrder(g_app_config.m_contract_code, THOST_FTDC_DEN_Buy, THOST_FTDC_OFEN_Open);
	}
	
}


void CGetMoneyDlg::OnBnClickedOpenSell()
{
		// TODO: 在此添加控件通知处理程序代码
	CGetMoneyApp* app = (CGetMoneyApp*)AfxGetApp();
	if (app && g_can_trading && IsDlgButtonChecked( IDC_CHECK_SAFE_ORDER ))
	{
		CString text;
		m_cbCode.GetWindowTextA(text);

		strncpy(g_app_config.m_contract_code,  (char*)(text.GetBuffer()), sizeof(g_app_config.m_contract_code) - 1);
		app->RequestOrder(g_app_config.m_contract_code, THOST_FTDC_DEN_Sell, THOST_FTDC_OFEN_Open);
	}
}


void CGetMoneyDlg::OnBnClickedClose()
{
	CGetMoneyApp* app = (CGetMoneyApp*)AfxGetApp();
	if (app && g_can_trading && IsDlgButtonChecked( IDC_CHECK_SAFE_ORDER ))
	{
		CString text;
		m_cbCode.GetWindowTextA(text);

		strncpy(g_app_config.m_contract_code,  (char*)(text.GetBuffer()), sizeof(g_app_config.m_contract_code) - 1);
		// 平仓
		app->RequestOrder(g_app_config.m_contract_code, THOST_FTDC_DEN_Sell, THOST_FTDC_OFEN_Close);
	}
}


void CGetMoneyDlg::OnBnClickedButtonCloseBuy()
{
	CGetMoneyApp* app = (CGetMoneyApp*)AfxGetApp();
	if (app && g_can_trading && IsDlgButtonChecked( IDC_CHECK_SAFE_ORDER ))
	{
		CString text;
		m_cbCode.GetWindowTextA(text);

		strncpy(g_app_config.m_contract_code,  (char*)(text.GetBuffer()), sizeof(g_app_config.m_contract_code) - 1);
		// 平仓
		app->RequestOrder(g_app_config.m_contract_code, THOST_FTDC_DEN_Buy, THOST_FTDC_OFEN_Close);
	}
}


void CGetMoneyDlg::OnCbnSelchangeComboCode()
{

	// TODO: 在此添加控件通知处理程序代码
}


void CGetMoneyDlg::OnCbnEditchangeComboCode()
{

}


void CGetMoneyDlg::OnCbnDropdownComboCode()
{

}
afx_msg LRESULT CGetMoneyDlg::OnUpdateText(WPARAM wParam, LPARAM lParam)
{
		GetDlgItem(IDC_STATIC_TEXT)->SetWindowTextA(g_text);

		return 0;
}