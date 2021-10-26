// UIDialog.cpp : 实现文件
//

#include "stdafx.h"
#include "UIDialog.h"

// CUIDialog

IMPLEMENT_DYNAMIC(CUIDialog, CUIWindow)

CUIDialog::CUIDialog(bool bModal) : m_bModal(bModal)
{
}

BEGIN_MESSAGE_MAP(CUIDialog, CUIWindow)
	ON_WM_CREATE()
	ON_WM_WINDOWPOSCHANGING()
	ON_WM_CLOSE()
	ON_WM_KEYDOWN()
END_MESSAGE_MAP()

// CUIDialog 消息处理程序

int CUIDialog::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (__super::OnCreate(lpCreateStruct) == -1)
		return -1;

	// TODO:  在此添加您专用的创建代码
	auto pButton = dynamic_cast<CUIButton *>(m_rootView.Search(L"确定"));
	if (pButton)
		pButton->BindClick([this]{ OnOK(); });

	pButton = dynamic_cast<CUIButton *>(m_rootView.Search(L"取消"));
	if (pButton)
		pButton->BindClick([this]{ OnCancel(); });

	pButton = dynamic_cast<CUIButton *>(m_rootView.Search(L"关闭"));
	if (pButton)
		pButton->BindClick([this]{ OnCancel(); });

	CenterWindow();
	return 0;
}

void CUIDialog::OnWindowPosChanging(WINDOWPOS* lpwndpos)
{
	__super::OnWindowPosChanging(lpwndpos);

	// TODO:  在此处添加消息处理程序代码
	if (m_bModal)
	{
		if (lpwndpos->flags & SWP_SHOWWINDOW)
			GetParent()->EnableWindow(FALSE);
		else if (lpwndpos->flags & SWP_HIDEWINDOW)
			GetParent()->EnableWindow(TRUE);
	}
}

void CUIDialog::OnClose()
{
	// TODO:  在此添加消息处理程序代码和/或调用默认值
	if (m_fnOnClose)
		m_fnOnClose();

	__super::OnClose();
}

void CUIDialog::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	// TODO:  在此添加消息处理程序代码和/或调用默认值
	switch (nChar)
	{
	case VK_RETURN:
	case VK_ESCAPE:
		if (GetKeyState(VK_CONTROL) < 0 || GetKeyState(VK_MENU) < 0 || GetKeyState(VK_SHIFT) < 0)
			break;

		if (nChar == VK_RETURN)
			OnOK();
		else
			OnCancel();
	}

	__super::OnKeyDown(nChar, nRepCnt, nFlags);
}

void CUIDialog::OnOK()
{
	if (m_fnOnOK)
		m_fnOnOK();
	else
		PostMessage(WM_SYSCOMMAND, SC_CLOSE);
}

void CUIDialog::OnCancel()
{
	if (m_fnOnCancel)
		m_fnOnCancel();
	else
		PostMessage(WM_SYSCOMMAND, SC_CLOSE);
}
