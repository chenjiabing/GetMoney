
// GetMoneyDlg.cpp : ʵ���ļ�
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

// ����API
extern      CThostFtdcTraderApi*					        g_trader_user_api;
// CTP ����ID
extern      int												g_request_id;
// ��Լ��ȡ���
// ��������
extern      ApplicationConfig								g_app_config;
// �Ƿ�������ɼ�ʱ��
extern      bool                                            g_is_working_time;
// �Ƿ�ɽ���
extern      bool											g_can_trading;

extern     CString  g_text;
// ����Ӧ�ó��򡰹��ڡ��˵���� CAboutDlg �Ի���
extern     HWND g_dlg_handle;

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// �Ի�������
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

// ʵ��
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


// CGetMoneyDlg �Ի���




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


// CGetMoneyDlg ��Ϣ�������

BOOL CGetMoneyDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// ��������...���˵�����ӵ�ϵͳ�˵��С�

	// IDM_ABOUTBOX ������ϵͳ���Χ�ڡ�
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

	// ���ô˶Ի����ͼ�ꡣ��Ӧ�ó��������ڲ��ǶԻ���ʱ����ܽ��Զ�
	//  ִ�д˲���
	SetIcon(m_hIcon, TRUE);			// ���ô�ͼ��
	SetIcon(m_hIcon, FALSE);		// ����Сͼ��

	// TODO: �ڴ���Ӷ���ĳ�ʼ������
	this->m_cbCode.InsertString(0, g_app_config.m_contract_code);
	// ����IH1506 IC1506
	this->m_cbCode.InsertString(1, "IH1506");
	this->m_cbCode.InsertString(2, "IC1506");

	// ѡ���һ��
	this->m_cbCode.SetCurSel(0);
	
	// 
	g_dlg_handle = this->GetSafeHwnd();

	return TRUE;  // ���ǽ��������õ��ؼ������򷵻� TRUE
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

// �����Ի��������С����ť������Ҫ����Ĵ���
//  �����Ƹ�ͼ�ꡣ����ʹ���ĵ�/��ͼģ�͵� MFC Ӧ�ó���
//  �⽫�ɿ���Զ���ɡ�

void CGetMoneyDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // ���ڻ��Ƶ��豸������

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// ʹͼ���ڹ����������о���
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// ����ͼ��
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}


	// ��ʼ��������ʾ
	//GetDlgItem(IDC_STATIC_USER)->SetWindowTextW(LPCTSTR(g_app_config.m_investor_id));
	//GetDlgItem(IDC_STATIC_CONTRACT)->SetWindowTextW(LPCTSTR(g_app_config.m_contract_code));
}

//���û��϶���С������ʱϵͳ���ô˺���ȡ�ù��
//��ʾ��
HCURSOR CGetMoneyDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}



void CGetMoneyDlg::OnBnClickedOpenBuy()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
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
		// TODO: �ڴ���ӿؼ�֪ͨ����������
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
		// ƽ��
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
		// ƽ��
		app->RequestOrder(g_app_config.m_contract_code, THOST_FTDC_DEN_Buy, THOST_FTDC_OFEN_Close);
	}
}


void CGetMoneyDlg::OnCbnSelchangeComboCode()
{

	// TODO: �ڴ���ӿؼ�֪ͨ����������
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