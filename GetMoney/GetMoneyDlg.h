
// GetMoneyDlg.h : 头文件
//

#pragma once
#include "afxwin.h"
#include "resource.h"


// CGetMoneyDlg 对话框
class CGetMoneyDlg : public CDialogEx
{
// 构造
public:
	CGetMoneyDlg(CWnd* pParent = NULL);	// 标准构造函数

// 对话框数据
	enum { IDD = IDD_GETMONEY_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持


// 实现
protected:
	HICON m_hIcon;

	// 生成的消息映射函数
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedOpenBuy();
	afx_msg void OnBnClickedOpenSell();
	afx_msg void OnBnClickedClose();
private:
	char  m_open_direction;
	
    CString m_user;

public:
	afx_msg void OnBnClickedButtonCloseBuy();
	afx_msg void OnCbnSelchangeComboCode();
	CComboBox m_cbCode;
	afx_msg void OnCbnEditchangeComboCode();
	afx_msg void OnCbnDropdownComboCode();
	afx_msg LRESULT OnUpdateText(WPARAM wParam, LPARAM lParam);
};
