
// ���_����ע��.h : PROJECT_NAME Ӧ�ó������ͷ�ļ�
//

#pragma once

#ifndef __AFXWIN_H__
	#error "�ڰ������ļ�֮ǰ������stdafx.h�������� PCH �ļ�"
#endif

#include "resource.h"		// ������


// C���_����ע��App: 
// �йش����ʵ�֣������ ���_����ע��.cpp
//

class C���_����ע��App : public CWinApp
{
public:
	C���_����ע��App();

// ��д
public:
	virtual BOOL InitInstance();

// ʵ��

	DECLARE_MESSAGE_MAP()
};

extern C���_����ע��App theApp;