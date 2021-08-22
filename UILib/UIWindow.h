#pragma once

class CUIWindow : public CWindowImpl<CUIWindow>, public IUILoadNotify
{
public:
	DECLARE_WND_CLASS_EX(NULL, CS_DBLCLKS, COLOR_WINDOW)
	DECLARE_UI_ROOTVIEW(m_rootView)

	CUIWindow() : m_hWndParent(NULL), m_nBorderSize(0), m_nCaptionHeight(0), m_pBtnMax(NULL) {}
	virtual ~CUIWindow() = default;

	bool CreateFromXml(LPCWSTR lpXmlName, HWND hParent = NULL);
	BOOL ProcessWindowMessage(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, LRESULT &lResult, DWORD dwMsgMapID = 0) override;

protected:
	virtual CUIView *OnCustomUI(LPCWSTR lpName, CUIView *pParent) override { return NULL; }
	virtual void OnLoadedUI(const IUILoadAttrs &attrs) override;
	virtual void OnClose() { DefWindowProc(); }
	virtual int  OnCreate(LPCREATESTRUCT lpCreateStruct) { return DefWindowProc(); }
	virtual void OnDestroy() { DefWindowProc(); }
	virtual void OnGetMinMaxInfo(MINMAXINFO *lpMMI) { DefWindowProc(); }
	virtual void OnKillFocus(HWND hNewWnd) { DefWindowProc(); }
	virtual BOOL OnNcActivate(BOOL bActive) { return DefWindowProc(); }
	virtual void OnNcCalcSize(BOOL bCalcValidRects, NCCALCSIZE_PARAMS *lpNCSP) { DefWindowProc(); }
	virtual long OnNcHitTest(CPoint point);
	virtual void OnSetFocus(HWND hOldWnd) { DefWindowProc(); }
	virtual void OnSize(UINT nType, CSize size);
	virtual void OnTimer(UINT_PTR nIDEvent) { DefWindowProc(); }
	virtual void OnWindowPosChanging(WINDOWPOS *lpWndPos) { DefWindowProc(); }
	virtual void OnWindowPosChanged(WINDOWPOS *lpWndPos) { DefWindowProc(); }

	HWND m_hWndParent;
	int  m_nBorderSize;
	int  m_nCaptionHeight;
	CUIStateButton *m_pBtnMax;
};

inline bool CUIWindow::CreateFromXml(LPCWSTR lpXmlName, HWND hParent)
{
	m_hWndParent = hParent;
	return UILib::LoadFromXml(lpXmlName, &m_rootView, this);
}

inline void CUIWindow::OnLoadedUI(const IUILoadAttrs &attrs)
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
		Create(m_hWndParent, CRect(0, 0, attrs.GetInt(L"width"), attrs.GetInt(L"height")), W2CT(attrs.GetStr(L"title")), dwStyle, attrs.GetInt(L"exStyle"));
	}
}

inline BOOL CUIWindow::ProcessWindowMessage(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, LRESULT &lResult, DWORD dwMsgMapID)
{
	if (dwMsgMapID)
	{
		ATLASSERT(0);
		return FALSE;
	}

	lResult = 0;

	switch (uMsg)
	{
	case WM_CLOSE:
		OnClose();
		break;

	case WM_CREATE:
		lResult = OnCreate(reinterpret_cast<LPCREATESTRUCT>(lParam));
		break;

	case WM_DESTROY:
		OnDestroy();
		break;

	case WM_GETMINMAXINFO:
		OnGetMinMaxInfo(reinterpret_cast<MINMAXINFO *>(lParam));
		break;

	case WM_KILLFOCUS:
		OnKillFocus(reinterpret_cast<HWND>(wParam));
		break;

	case WM_NCACTIVATE:
		lResult = OnNcActivate(wParam);
		break;

	case WM_NCCALCSIZE:
		OnNcCalcSize(wParam, reinterpret_cast<NCCALCSIZE_PARAMS *>(lParam));
		break;

	case WM_NCHITTEST:
		lResult = OnNcHitTest(lParam);
		break;

	case WM_SETFOCUS:
		OnSetFocus(reinterpret_cast<HWND>(wParam));
		break;

	case WM_SIZE:
		OnSize(wParam, lParam);
		break;

	case WM_TIMER:
		OnTimer(wParam);
		break;

	case WM_WINDOWPOSCHANGING:
		OnWindowPosChanging(reinterpret_cast<WINDOWPOS *>(lParam));
		break;

	case WM_WINDOWPOSCHANGED:
		OnWindowPosChanged(reinterpret_cast<WINDOWPOS *>(lParam));
		break;

	default:
		return FALSE;
	}

	return TRUE;
}

inline long CUIWindow::OnNcHitTest(CPoint point)
{
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

	return DefWindowProc();
}

inline void CUIWindow::OnSize(UINT nType, CSize size)
{
	DefWindowProc();

	if (m_pBtnMax)
		m_pBtnMax->SetState(nType == SIZE_MAXIMIZED);
}
