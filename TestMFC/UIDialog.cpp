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
END_MESSAGE_MAP()

// CUIDialog ��Ϣ�������

int CUIDialog::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (__super::OnCreate(lpCreateStruct) == -1)
		return -1;

	// TODO:  �ڴ������ר�õĴ�������
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
