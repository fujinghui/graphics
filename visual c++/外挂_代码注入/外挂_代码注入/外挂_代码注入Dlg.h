
// ���_����ע��Dlg.h : ͷ�ļ�
//

#pragma once
typedef HHOOK(_stdcall *_HookGameProc)();		//����һ�����뺯��

// C���_����ע��Dlg �Ի���
class C���_����ע��Dlg : public CDialogEx
{
// ����
public:
	C���_����ע��Dlg(CWnd* pParent = NULL);	// ��׼���캯��

// �Ի�������
	enum { IDD = IDD___DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV ֧��


// ʵ��
protected:
	HICON m_hIcon;

	// ���ɵ���Ϣӳ�亯��
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
private:
	bool m_bGameTop;
	int m_nBombCnt;
	_HookGameProc HookProc;		//���뺯���ĵ�ַ
public:
	afx_msg void OnBnClickedButton2();
	afx_msg void OnBnClickedButton1();
};
