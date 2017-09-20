
// 外挂_代码注入Dlg.cpp : 实现文件
//

#include "stdafx.h"
#include "外挂_代码注入.h"
#include "外挂_代码注入Dlg.h"
#include "afxdialogex.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// 用于应用程序“关于”菜单项的 CAboutDlg 对话框

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


// C外挂_代码注入Dlg 对话框



C外挂_代码注入Dlg::C外挂_代码注入Dlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(C外挂_代码注入Dlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void C外挂_代码注入Dlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(C外挂_代码注入Dlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BUTTON2, &C外挂_代码注入Dlg::OnBnClickedButton2)
	ON_BN_CLICKED(IDC_BUTTON1, &C外挂_代码注入Dlg::OnBnClickedButton1)
END_MESSAGE_MAP()


// C外挂_代码注入Dlg 消息处理程序

BOOL C外挂_代码注入Dlg::OnInitDialog()
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

	// 设置此对话框的图标。  当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标


	HMODULE hdll = LoadLibrary(L"动态库.dll");
	if (hdll)
	{
		HookProc = (_HookGameProc)GetProcAddress(hdll, "HookGame");
		if (HookProc == NULL)
		{
			MessageBoxA(NULL, "未找到外挂辅助函数", "Error!", MB_OK);
			return false;
		}
	}
	m_bGameTop = false;
	m_nBombCnt = 0;

	// TODO:  在此添加额外的初始化代码

	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

void C外挂_代码注入Dlg::OnSysCommand(UINT nID, LPARAM lParam)
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
//  来绘制该图标。  对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void C外挂_代码注入Dlg::OnPaint()
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
}

//当用户拖动最小化窗口时系统调用此函数取得光标
//显示。
HCURSOR C外挂_代码注入Dlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}



void C外挂_代码注入Dlg::OnBnClickedButton2()
{
	// TODO:  在此添加控件通知处理程序代码
	HWND game_hwnd = FindWindowA(NULL, "俄罗斯方块-雅道版");
	if (game_hwnd == NULL)
	{
		MessageBoxA(NULL, "未找到游戏", "Error", MB_OK);
		return;
	}
	else
	{
		if (m_bGameTop)		//如果已经置顶，则取消
		{
			::SetWindowPos(game_hwnd, HWND_NOTOPMOST, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOMOVE | SWP_HIDEWINDOW);
		}
		else
		{
			::SetWindowPos(game_hwnd, HWND_TOPMOST, 0, 0, 0, 0,SWP_NOSIZE | SWP_NOMOVE | SWP_SHOWWINDOW);
		}
	}
}


void C外挂_代码注入Dlg::OnBnClickedButton1()
{
	// TODO:  在此添加控件通知处理程序代码
	LPVOID pbase = (LPVOID)0x0040DF38;
	DWORD data = 0;
	DWORD dwRead = 0;
	HWND game_hwnd = FindWindowA(NULL, "俄罗斯方块-雅道版");
	if (game_hwnd != NULL)
	{
		DWORD game_pid;
		GetWindowThreadProcessId(game_hwnd, &game_pid);			//获取进程的pid
		HANDLE game_handle = OpenProcess(PROCESS_ALL_ACCESS, FALSE, game_pid);		//通过该id获取进程句柄
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
				sprintf_s(szTemp, sizeof(szTemp), "外挂安装成功");
				SetWindowTextA(this->m_hWnd, szTemp);
			}
			else
			{
				sprintf_s(szTemp, sizeof(szTemp), "外挂安装失败");
				SetWindowTextA(this->m_hWnd, szTemp);
			}
		}

	}
}
