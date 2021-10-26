#include "stdafx.h"
#include "DemoWnd2.h"

CDemoWnd2::CDemoWnd2()
{
}

void CDemoWnd2::OnLoadUI(const IUIXmlAttrs &attrs)
{
	auto pBtnMax = dynamic_cast<CUIStateButton *>(m_rootView.Search(L"最大化"));
	if (pBtnMax)
		pBtnMax->BindClick([this](int nState){ this->SendMessage(WM_SYSCOMMAND, nState == 0 ? SC_MAXIMIZE : SC_RESTORE); });

	__super::OnLoadUI(attrs);
}

void CDemoWnd2::OnCreate()
{
	__super::OnCreate();

	CenterWindow();
}

void CDemoWnd2::OnGetMinMaxInfo(MINMAXINFO *lpMMI)
{
	lpMMI->ptMinTrackSize.x = 500;
	lpMMI->ptMinTrackSize.y = 300;

	__super::OnGetMinMaxInfo(lpMMI);
}

void CDemoWnd2::OnSize(UINT nType, CSize size)
{
	if (nType != SIZE_MINIMIZED)
	{
		auto pBtnMax = dynamic_cast<CUIStateButton *>(m_rootView.Search(L"最大化", 2));
		if (pBtnMax)
			pBtnMax->SetState(nType != SIZE_RESTORED);
	}

	__super::OnSize(nType, size);
}
