
// ServerChatDlg.h : header file
//

#pragma once
#include "Manage.h"

// CServerChatDlg dialog
class CServerChatDlg : public CDialogEx
{
// Construction
public:
	CServerChatDlg(CWnd* pParent = nullptr);	// standard constructor

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_SERVERCHAT_DIALOG };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support


// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP();
public:
	void showListClientMsg(CString,CString);
	void ShowMessage(CString msg);
	void updateList(std::vector<psa>);
private:
	void AppendText(CString msg);
	CListCtrl listClient;
	CEdit r_ip; 
	CEdit log;
	CEdit r_port;
	bool status = false;
	bool DlgStart();
	bool DlgStop();
	static UINT __cdecl StaticThreadFunction(LPVOID);
	CWinThread* cTh;
	Manage* pManage;
	UINT ThreadFunc();
public:
	afx_msg void OnBnClickedButton1();
};
