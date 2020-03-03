
// ServerChatDlg.cpp : implementation file
//

#include "pch.h"
#include "framework.h"
#include "ServerChat.h"
#include "ServerChatDlg.h"
#include "afxdialogex.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CAboutDlg dialog used for App About

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ABOUTBOX };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

// Implementation
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(IDD_ABOUTBOX)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// CServerChatDlg dialog



CServerChatDlg::CServerChatDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_SERVERCHAT_DIALOG, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CServerChatDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST1, listClient);
	DDX_Control(pDX, IDC_EDIT1, r_ip);
	DDX_Control(pDX, IDC_EDIT2, r_port);
	DDX_Control(pDX, IDC_EDIT3, log);
	DDX_Control(pDX, IDC_EDIT2, r_port);
}

BEGIN_MESSAGE_MAP(CServerChatDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//ON_NOTIFY(LVN_ITEMCHANGED, IDC_LIST1, &CServerChatDlg::OnLvnItemchangedList1)
	ON_BN_CLICKED(IDC_BUTTON1, &CServerChatDlg::OnBnClickedButton1)
END_MESSAGE_MAP()


// CServerChatDlg message handlers

BOOL CServerChatDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// Add "About..." menu item to system menu.

	// IDM_ABOUTBOX must be in the system command range.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != nullptr)
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

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	// TODO: Add extra initialization here
	listClient.InsertColumn(0, L"IP", LVCFMT_CENTER, 60);
	listClient.InsertColumn(1, L"Port", LVCFMT_CENTER, 40);
	listClient.SetExtendedStyle(listClient.GetExtendedStyle() | LVS_EX_FULLROWSELECT);
	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CServerChatDlg::OnSysCommand(UINT nID, LPARAM lParam)
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

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CServerChatDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

// The system calls this function to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CServerChatDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void CServerChatDlg::showListClientMsg(CString ip, CString port)
{
	int n = listClient.InsertItem(0, ip);
	listClient.SetItemText(n, 1, port);
}

void CServerChatDlg::ShowMessage(CString msg)
{
	CString temp;
	temp.Format(_T("%s\r\n"), msg);
	AppendText(temp);
}

void CServerChatDlg::updateList(std::vector<psa>vs)
{
	listClient.DeleteAllItems();
	for (auto i : vs)
	{
		showListClientMsg(i.ip, i.port);
	}
	
}

void CServerChatDlg::AppendText(CString msg)
{
	int n=log.GetWindowTextLengthW();
	log.SetSel(n, n);
	log.ReplaceSel(msg);
}


bool CServerChatDlg::DlgStart()
{
	cTh = AfxBeginThread(StaticThreadFunction, this);
	LPDWORD exitcode=0;
	GetExitCodeThread(cTh, exitcode);
	return exitcode;
}

bool CServerChatDlg::DlgStop()
{
	delete(pManage);
	ShowMessage(L"Server stopped");
	return true;
}

UINT __cdecl CServerChatDlg::StaticThreadFunction(LPVOID pParam)
{
	CServerChatDlg* p = reinterpret_cast<CServerChatDlg*>(pParam);
	UINT retCode = p->ThreadFunc();
	return retCode;
}

UINT CServerChatDlg::ThreadFunc()
{
	CString c_ip;
	GetDlgItemText(IDC_EDIT1, c_ip);
	CString c_port;
	GetDlgItemText(IDC_EDIT2, c_port);
	if (c_ip.GetLength() == 0)
	{
		AfxMessageBox(L"Please provide an IP");
		return -1;
	}
	if ((c_port == L"0") || (c_port.GetLength() == 0))
	{
		AfxMessageBox(L"Please provide an valid port");
		return -1;
	}
	pManage = new Manage(this);
	return pManage->StartListen(c_ip, c_port);
}

void CServerChatDlg::OnBnClickedButton1()
{
	if (status)
	{
		if (CServerChatDlg::DlgStop())
		{
			GetDlgItem(IDC_BUTTON1)->SetWindowTextW(L"Start");
			status = false;
		}
		return;
	}
	if (DlgStart()==0)
	{
		GetDlgItem(IDC_BUTTON1)->SetWindowTextW(L"Stop");
		status = true;
	}
}
