#include "stdafx.h"
#include "UIDialog.h"

CUIDialog::CUIDialog(bool bModal) : m_bModal(bModal)
{
}

static void SearchButtons(CUIView *pView, LPCWSTR lpszId, vector<CUIButton *> &vecButtons)
{
	if (_wcsicmp(pView->GetId(), lpszId) == 0)
	{
		if (auto pButton = dynamic_cast<CUIButton *>(pView))
			vecButtons.push_back(pButton);
	}

	for (auto pItem : pView->GetChilds())
		SearchButtons(pItem, lpszId, vecButtons);
}

void CUIDialog::OnCreate()
{
	__super::OnCreate();

	vector<CUIButton *> vecButtons;
	SearchButtons(GetRootView(), L"确定", vecButtons);

	for (auto pButton : vecButtons)
		pButton->BindClick([this]{ OnOK(); });

	vecButtons.clear();
	SearchButtons(GetRootView(), L"取消", vecButtons);

	for (auto pButton : vecButtons)
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
	if (m_fnOnClose && !m_fnOnClose())
		return;

	__super::OnClose();
}

void CUIDialog::OnDestroy()
{
	__super::OnDestroy();

	if (m_fnOnDestroy)
		m_fnOnDestroy();
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
			vector<CUIButton *> vecButtons;
			SearchButtons(GetRootView(), L"确定", vecButtons);

			for (auto pButton : vecButtons)
			{
				if (pButton->IsRealVisible() && pButton->IsRealEnabled())
				{
					OnOK();
					break;
				}
			}
		}
		else
			OnCancel();
	}

	bHandled = FALSE;
	return 0;
}
