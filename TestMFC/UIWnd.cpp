// UIWnd.cpp : 实现文件
//

#include "stdafx.h"
#include "UIWnd.h"

// CUIWnd

IMPLEMENT_DYNAMIC(CUIWnd, CWnd)

CUIWnd::CUIWnd(bool bModal) : m_rootView(this), m_bModal(bModal), m_hWndParent(NULL), m_nBorderSize(0), m_nCaptionHeight(0), m_pBtnMax(NULL)
{
}

CUIWnd::~CUIWnd()
{
}

BEGIN_MESSAGE_MAP(CUIWnd, CWnd)
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_WM_WINDOWPOSCHANGING()
	ON_WM_NCACTIVATE()
	ON_WM_NCCALCSIZE()
	ON_WM_NCHITTEST()
END_MESSAGE_MAP()

// CUIWnd 消息处理程序

BOOL CUIWnd::OnWndMsg(UINT message, WPARAM wParam, LPARAM lParam, LRESULT* pResult)
{
	// TODO:  在此添加专用代码和/或调用基类
	if (m_rootView.ProcessWindowMessage(m_hWnd, message, wParam, lParam, *pResult))
		return TRUE;

	return __super::OnWndMsg(message, wParam, lParam, pResult);
}

void CUIWnd::PostNcDestroy()
{
	// TODO:  在此添加专用代码和/或调用基类
	delete this;
}

bool CUIWnd::CreateFromXml(LPCWSTR lpXmlName, HWND hParent)
{
	m_hWndParent = hParent;
	return UILib::LoadFromXml(lpXmlName, &m_rootView);
}

void CUIWnd::OnLoadedUI(const IUILoadAttrs &attrs)
{
	m_nBorderSize = attrs.GetInt(L"border");
	m_nCaptionHeight = attrs.GetInt(L"caption");

	CUIButton *pButton = dynamic_cast<CUIButton *>(attrs.GetView(L"关闭"));
	if (pButton)
		pButton->OnClick([this]{ this->SendMessage(WM_SYSCOMMAND, SC_CLOSE); });

	pButton = dynamic_cast<CUIButton *>(attrs.GetView(L"最小化"));
	if (pButton)
		pButton->OnClick([this]{ this->SendMessage(WM_SYSCOMMAND, SC_MINIMIZE); });

	m_pBtnMax = dynamic_cast<CUIStateButton *>(attrs.GetView(L"最大化"));
	if (m_pBtnMax)
		m_pBtnMax->OnClick([this](int nState){ this->SendMessage(WM_SYSCOMMAND, nState == 0 ? SC_MAXIMIZE : SC_RESTORE); });

	DWORD dwStyle = attrs.GetInt(L"style");
	if (m_nBorderSize > 0)
		dwStyle |= WS_THICKFRAME;

	if (m_hWnd == NULL)
	{
		USES_CONVERSION;
		CreateEx(attrs.GetInt(L"exStyle"), AfxRegisterWndClass(0, LoadCursor(NULL, IDC_ARROW)), W2CT(attrs.GetStr(L"title")), dwStyle, 0, 0, attrs.GetInt(L"width"), attrs.GetInt(L"height"), m_hWndParent, 0);
	}
}

int CUIWnd::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (__super::OnCreate(lpCreateStruct) == -1)
		return -1;

	// TODO:  在此添加您专用的创建代码
	SetWindowRgn(CUIRgn(0, 0, 4000, 3000), FALSE);
	CenterWindow();

	return 0;
}

void CUIWnd::OnSize(UINT nType, int cx, int cy)
{
	__super::OnSize(nType, cx, cy);

	// TODO:  在此处添加消息处理程序代码
	if (m_pBtnMax)
		m_pBtnMax->SetState(nType == SIZE_MAXIMIZED);
}

void CUIWnd::OnWindowPosChanging(WINDOWPOS* lpwndpos)
{
	__super::OnWindowPosChanging(lpwndpos);

	// TODO:  在此处添加消息处理程序代码
	if (!m_bModal)
	{
	}
	else if (lpwndpos->flags & SWP_SHOWWINDOW)
	{
		GetParent()->EnableWindow(FALSE);
		EnableWindow(TRUE);
	}
	else if (lpwndpos->flags & SWP_HIDEWINDOW)
	{
		GetParent()->EnableWindow(TRUE);
	}
}

BOOL CUIWnd::OnNcActivate(BOOL bActive)
{
	// TODO:  在此添加消息处理程序代码和/或调用默认值

	return TRUE;
//	return __super::OnNcActivate(bActive);
}

void CUIWnd::OnNcCalcSize(BOOL bCalcValidRects, NCCALCSIZE_PARAMS* lpncsp)
{
	// TODO:  在此添加消息处理程序代码和/或调用默认值

//	__super::OnNcCalcSize(bCalcValidRects, lpncsp);
}

LRESULT CUIWnd::OnNcHitTest(CPoint point)
{
	// TODO:  在此添加消息处理程序代码和/或调用默认值
	CRect rect;
	GetWindowRect(rect);

	if (m_nBorderSize > 0)
	{
		UINT nFlags = 0;

		if (point.x < rect.left + m_nBorderSize)
			nFlags |= 1;
		else if (point.x > rect.right - m_nBorderSize)
			nFlags |= 2;
		if (point.y < rect.top + m_nBorderSize)
			nFlags |= 4;
		else if (point.y > rect.bottom - m_nBorderSize)
			nFlags |= 8;

		switch (nFlags)
		{
		case 1:
			return HTLEFT;

		case 2:
			return HTRIGHT;

		case 4:
			return HTTOP;

		case 5:
			return HTTOPLEFT;

		case 6:
			return HTTOPRIGHT;

		case 8:
			return HTBOTTOM;

		case 9:
			return HTBOTTOMLEFT;

		case 10:
			return HTBOTTOMRIGHT;
		}
	}

	if (m_nCaptionHeight < 0 || m_nCaptionHeight > 0 && point.y < rect.top + m_nCaptionHeight)
		return HTCAPTION;

	return __super::OnNcHitTest(point);
}
