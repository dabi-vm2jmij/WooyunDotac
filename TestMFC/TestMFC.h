
// TestMFC.h : TestMFC Ӧ�ó������ͷ�ļ�
//
#pragma once

#ifndef __AFXWIN_H__
	#error "�ڰ������ļ�֮ǰ������stdafx.h�������� PCH �ļ�"
#endif

#include "resource.h"       // ������

// CTestMFCApp:
// �йش����ʵ�֣������ TestMFC.cpp
//

class CTestMFCApp : public CWinApp
{
public:
	CTestMFCApp();

// ��д
public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();

// ʵ��
public:
	DECLARE_MESSAGE_MAP()
};

extern CTestMFCApp theApp;
