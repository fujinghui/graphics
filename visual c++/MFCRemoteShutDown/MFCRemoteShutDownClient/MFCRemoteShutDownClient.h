
// MFCRemoteShutDownClient.h : PROJECT_NAME Ӧ�ó������ͷ�ļ�
//

#pragma once

#ifndef __AFXWIN_H__
	#error "�ڰ������ļ�֮ǰ������stdafx.h�������� PCH �ļ�"
#endif

#include "resource.h"		// ������


// CMFCRemoteShutDownClientApp: 
// �йش����ʵ�֣������ MFCRemoteShutDownClient.cpp
//

class CMFCRemoteShutDownClientApp : public CWinApp
{
public:
	CMFCRemoteShutDownClientApp();

// ��д
public:
	virtual BOOL InitInstance();

// ʵ��

	DECLARE_MESSAGE_MAP()
};

extern CMFCRemoteShutDownClientApp theApp;