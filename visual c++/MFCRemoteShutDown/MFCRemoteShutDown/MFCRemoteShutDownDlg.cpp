
// MFCRemoteShutDownDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "MFCRemoteShutDown.h"
#include "MFCRemoteShutDownDlg.h"
#include "afxdialogex.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// ����Ӧ�ó��򡰹��ڡ��˵���� CAboutDlg �Ի���

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


// CMFCRemoteShutDownDlg �Ի���



CMFCRemoteShutDownDlg::CMFCRemoteShutDownDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CMFCRemoteShutDownDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CMFCRemoteShutDownDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CMFCRemoteShutDownDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(btn_ok, &CMFCRemoteShutDownDlg::OnBnClickedok)
END_MESSAGE_MAP()


// CMFCRemoteShutDownDlg ��Ϣ�������

BOOL CMFCRemoteShutDownDlg::OnInitDialog()
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

	// ���ô˶Ի����ͼ�ꡣ  ��Ӧ�ó��������ڲ��ǶԻ���ʱ����ܽ��Զ�
	//  ִ�д˲���
	SetIcon(m_hIcon, TRUE);			// ���ô�ͼ��
	SetIcon(m_hIcon, FALSE);		// ����Сͼ��

	// TODO:  �ڴ���Ӷ���ĳ�ʼ������

	return TRUE;  // ���ǽ��������õ��ؼ������򷵻� TRUE
}

void CMFCRemoteShutDownDlg::OnSysCommand(UINT nID, LPARAM lParam)
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
//  �����Ƹ�ͼ�ꡣ  ����ʹ���ĵ�/��ͼģ�͵� MFC Ӧ�ó���
//  �⽫�ɿ���Զ���ɡ�

void CMFCRemoteShutDownDlg::OnPaint()
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
}

//���û��϶���С������ʱϵͳ���ô˺���ȡ�ù��
//��ʾ��
HCURSOR CMFCRemoteShutDownDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}


BOOL getPrivilege(){
	HANDLE hToken = nullptr;
	TOKEN_PRIVILEGES tkp = {};
	BOOL bRet = FALSE;
	//��Ȩ����
	do{
		if (!OpenProcessToken(GetCurrentProcess(),
			TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, &hToken))
			break;
		LookupPrivilegeValue(NULL, SE_SHUTDOWN_NAME,
			&tkp.Privileges[0].Luid);
		tkp.PrivilegeCount = 1;
		tkp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;

		AdjustTokenPrivileges(hToken, FALSE, &tkp, 0, (PTOKEN_PRIVILEGES)NULL, 0);
		if (GetLastError() != ERROR_SUCCESS)
			break;
		bRet = TRUE;
	} while (FALSE);
	if (bRet == FALSE)
	{
		SetLastError(GetLastError());
	}
	if (hToken)
	{
		CloseHandle(hToken);
	}
	return bRet;
}

void CMFCRemoteShutDownDlg::OnBnClickedok()
{
	WSAData wsadata;
	do{
		if (WSAStartup(MAKEWORD(2, 2), &wsadata) != 0)
			break;
		if (LOBYTE(wsadata.wVersion) != 2 || HIBYTE(wsadata.wVersion) != 2)
			break;
		SOCKET socket_server = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
		sockaddr_in addr_server = { 0 };
		addr_server.sin_family = AF_INET;
		addr_server.sin_port = htons(18080);
		bind(socket_server, (SOCKADDR*)&addr_server, sizeof(addr_server));
		listen(socket_server, SOMAXCONN);
		accept(socket_server, nullptr, nullptr);
		MessageBoxW(L"accept link!");
		closesocket(socket_server);



	} while (FALSE);
//	if (getPrivilege())			//��Ȩ�ɹ�
//		ExitWindowsEx(EWX_SHUTDOWN, TRUE);
}
