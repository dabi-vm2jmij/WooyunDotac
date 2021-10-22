#include "stdafx.h"
#include "UIDialog.h"

CUIDialog::CUIDialog(bool bModal) : m_bModal(bModal)
{
}

void CUIDialog::OnCreate()
{
	__super::OnCreate();

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
