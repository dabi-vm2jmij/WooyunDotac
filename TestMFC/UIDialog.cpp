// UIDialog.cpp : ʵ���ļ�
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

// CUIDialog ��Ϣ�������

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

int CUIDialog::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (__super::OnCreate(lpCreateStruct) == -1)
		return -1;

	// TODO:  �ڴ������ר�õĴ�������
	vector<CUIButton *> vecButtons;
	SearchButtons(GetRootView(), L"ȷ��", vecButtons);

	for (auto pButton : vecButtons)
		pButton->BindClick([this]{ OnOK(); });

	vecButtons.clear();
	SearchButtons(GetRootView(), L"ȡ��", vecButtons);

	for (auto pButton : vecButtons)
		pButton->BindClick([this]{ OnCancel(); });

	CenterWindow();
	return 0;
}

void CUIDialog::OnWindowPosChanging(WINDOWPOS* lpwndpos)
{
	__super::OnWindowPosChanging(lpwndpos);

	// TODO:  �ڴ˴������Ϣ����������
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
	// TODO:  �ڴ������Ϣ�����������/�����Ĭ��ֵ
	if (m_fnOnClose)
		m_fnOnClose();

	__super::OnClose();
}

void CUIDialog::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	// TODO:  �ڴ������Ϣ�����������/�����Ĭ��ֵ
	switch (nChar)
	{
	case VK_RETURN:
	case VK_ESCAPE:
		if (GetKeyState(VK_CONTROL) < 0 || GetKeyState(VK_MENU) < 0 || GetKeyState(VK_LBUTTON) < 0)
			break;

		if (nChar == VK_RETURN)
		{
			vector<CUIButton *> vecButtons;
			SearchButtons(GetRootView(), L"ȷ��", vecButtons);

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
