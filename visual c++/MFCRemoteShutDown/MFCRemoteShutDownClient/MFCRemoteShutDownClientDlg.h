
// MFCRemoteShutDownClientDlg.h : ͷ�ļ�
//

#pragma once


// CMFCRemoteShutDownClientDlg �Ի���
class CMFCRemoteShutDownClientDlg : public CDialogEx
{
// ����
public:
	CMFCRemoteShutDownClientDlg(CWnd* pParent = NULL);	// ��׼���캯��

// �Ի�������
	enum { IDD = IDD_MFCREMOTESHUTDOWNCLIENT_DIALOG };

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
public:
	afx_msg void OnBnClickedBtnYes();
};
