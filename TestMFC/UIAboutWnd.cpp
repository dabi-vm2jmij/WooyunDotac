// UIAboutWnd.cpp : 实现文件
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

// CUIAboutWnd 消息处理程序

void CUIAboutWnd::OnLoadUI(const IUIXmlAttrs &attrs)
{
	// TODO:  在此添加专用代码和/或调用基类

	__super::OnLoadUI(attrs);
}
