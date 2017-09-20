
// 外挂_代码注入Dlg.h : 头文件
//

#pragma once
typedef HHOOK(_stdcall *_HookGameProc)();		//申请一个导入函数

// C外挂_代码注入Dlg 对话框
class C外挂_代码注入Dlg : public CDialogEx
{
// 构造
public:
	C外挂_代码注入Dlg(CWnd* pParent = NULL);	// 标准构造函数

// 对话框数据
	enum { IDD = IDD___DIALOG };

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
private:
	bool m_bGameTop;
	int m_nBombCnt;
	_HookGameProc HookProc;		//导入函数的地址
public:
	afx_msg void OnBnClickedButton2();
	afx_msg void OnBnClickedButton1();
};
