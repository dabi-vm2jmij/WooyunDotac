#include "stdafx.h"
#include "UIDialog.h"

CUIDialog::CUIDialog(bool bModal) : m_bModal(bModal)
{
}

void CUIDialog::OnCreate()
{
	__super::OnCreate();

	auto pButton = dynamic_cast<CUIButton *>(m_rootView.Search(L"确定"));
	if (pButton)
		pButton->BindClick([this]{ OnOK(); });

	pButton = dynamic_cast<CUIButton *>(m_rootView.Search(L"取消"));
	if (pButton)
		pButton->BindClick([this]{ OnCancel(); });

	CenterWindow();
}

void CUIDialog::OnWindowPosChanging(WINDOWPOS *lpWndPos)
{
	__super::OnWindowPosChanging(lpWndPos);

	if (m_bModal)
	{
		if (lpWndPos->flags & SWP_SHOWWINDOW)
			GetParent().EnableWindow(FALSE);
		else if (lpWndPos->flags & SWP_HIDEWINDOW)
			GetParent().EnableWindow(TRUE);
	}
}

void CUIDialog::OnClose()
{
	if (m_fnOnClose)
		m_fnOnClose();

	__super::OnClose();
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

LRESULT CUIDialog::OnKeyDown(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL &bHandled)
{
	switch (wParam)
	{
	case VK_RETURN:
	case VK_ESCAPE:
		if (GetKeyState(VK_CONTROL) < 0 || GetKeyState(VK_MENU) < 0 || GetKeyState(VK_LBUTTON) < 0)
			break;

		if (wParam == VK_RETURN)
		{
			auto pButton = dynamic_cast<CUIButton *>(m_rootView.Search(L"确定"));
			if (pButton && pButton->IsRealVisible())
				OnOK();
		}
		else
			OnCancel();
	}

	bHandled = FALSE;
	return 0;
}
