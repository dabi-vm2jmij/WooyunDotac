// stdafx.cpp : ֻ������׼�����ļ���Դ�ļ�
// TestLib.pch ����ΪԤ����ͷ
// stdafx.obj ������Ԥ����������Ϣ

#include "stdafx.h"

// TODO:  �� STDAFX.H ��
// �����κ�����ĸ���ͷ�ļ����������ڴ��ļ�������
#pragma comment(lib, "comctl32.lib")

#ifdef UILIB_IMPORTS
#ifdef _DEBUG
#pragma comment(lib, "..\\Debug\\UILib.lib")
#else
#pragma comment(lib, "..\\Release\\UILib.lib")
#endif
#else
#ifdef _DEBUG
#pragma comment(lib, "..\\Debug\\UILib2.lib")
#else
#pragma comment(lib, "..\\Release\\UILib2.lib")
#endif
#endif
