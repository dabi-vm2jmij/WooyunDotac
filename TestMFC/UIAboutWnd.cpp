// UIAboutWnd.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "UIAboutWnd.h"

// CUIAboutWnd

IMPLEMENT_DYNAMIC(CUIAboutWnd, CUIWnd)

CUIAboutWnd::CUIAboutWnd() : CUIWnd(true)
{
}

CUIAboutWnd::~CUIAboutWnd()
{
}

BEGIN_MESSAGE_MAP(CUIAboutWnd, CUIWnd)
END_MESSAGE_MAP()

// CUIAboutWnd ��Ϣ�������

void CUIAboutWnd::OnLoadedUI(const IUILoadAttrs &loaded)
{
	// TODO:  �ڴ����ר�ô����/����û���

	__super::OnLoadedUI(loaded);
}
