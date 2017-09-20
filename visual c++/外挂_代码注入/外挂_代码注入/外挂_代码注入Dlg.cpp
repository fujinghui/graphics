
// ���_����ע��Dlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "���_����ע��.h"
#include "���_����ע��Dlg.h"
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


// C���_����ע��Dlg �Ի���



C���_����ע��Dlg::C���_����ע��Dlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(C���_����ע��Dlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void C���_����ע��Dlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(C���_����ע��Dlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BUTTON2, &C���_����ע��Dlg::OnBnClickedButton2)
	ON_BN_CLICKED(IDC_BUTTON1, &C���_����ע��Dlg::OnBnClickedButton1)
END_MESSAGE_MAP()


// C���_����ע��Dlg ��Ϣ�������

BOOL C���_����ע��Dlg::OnInitDialog()
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


	HMODULE hdll = LoadLibrary(L"��̬��.dll");
	if (hdll)
	{
		HookProc = (_HookGameProc)GetProcAddress(hdll, "HookGame");
		if (HookProc == NULL)
		{
			MessageBoxA(NULL, "δ�ҵ���Ҹ�������", "Error!", MB_OK);
			return false;
		}
	}
	m_bGameTop = false;
	m_nBombCnt = 0;

	// TODO:  �ڴ���Ӷ���ĳ�ʼ������

	return TRUE;  // ���ǽ��������õ��ؼ������򷵻� TRUE
}

void C���_����ע��Dlg::OnSysCommand(UINT nID, LPARAM lParam)
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

void C���_����ע��Dlg::OnPaint()
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
HCURSOR C���_����ע��Dlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}



void C���_����ע��Dlg::OnBnClickedButton2()
{
	// TODO:  �ڴ���ӿؼ�֪ͨ����������
	HWND game_hwnd = FindWindowA(NULL, "����˹����-�ŵ���");
	if (game_hwnd == NULL)
	{
		MessageBoxA(NULL, "δ�ҵ���Ϸ", "Error", MB_OK);
		return;
	}
	else
	{
		if (m_bGameTop)		//����Ѿ��ö�����ȡ��
		{
			::SetWindowPos(game_hwnd, HWND_NOTOPMOST, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOMOVE | SWP_HIDEWINDOW);
		}
		else
		{
			::SetWindowPos(game_hwnd, HWND_TOPMOST, 0, 0, 0, 0,SWP_NOSIZE | SWP_NOMOVE | SWP_SHOWWINDOW);
		}
	}
}


void C���_����ע��Dlg::OnBnClickedButton1()
{
	// TODO:  �ڴ���ӿؼ�֪ͨ����������
	LPVOID pbase = (LPVOID)0x0040DF38;
	DWORD data = 0;
	DWORD dwRead = 0;
	HWND game_hwnd = FindWindowA(NULL, "����˹����-�ŵ���");
	if (game_hwnd != NULL)
	{
		DWORD game_pid;
		GetWindowThreadProcessId(game_hwnd, &game_pid);			//��ȡ���̵�pid
		HANDLE game_handle = OpenProcess(PROCESS_ALL_ACCESS, FALSE, game_pid);		//ͨ����id��ȡ���̾��
		if (game_handle != NULL)
		{
			ReadProcessMemory(game_handle, pbase, &data, 4, &dwRead);
			char szTemp[256];
			sprintf_s(szTemp, sizeof(szTemp), "score:%d\0", data);
			SetWindowTextA(this->m_hWnd, szTemp);
			
			CloseHandle(game_handle);

			HHOOK game_hook = HookProc();
			if (game_hook)
			{
				sprintf_s(szTemp, sizeof(szTemp), "��Ұ�װ�ɹ�");
				SetWindowTextA(this->m_hWnd, szTemp);
			}
			else
			{
				sprintf_s(szTemp, sizeof(szTemp), "��Ұ�װʧ��");
				SetWindowTextA(this->m_hWnd, szTemp);
			}
		}

	}
}
