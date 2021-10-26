#pragma once

#include "UIView.h"

class IUIWindow
{
public:
	virtual CUIView *OnCustomUI(LPCWSTR lpName, CUIView *pParent) { return NULL; }
	virtual void OnLoadUI(const IUIXmlAttrs &attrs) {}
	virtual HWND GetHwnd() const = 0;
	virtual void InvalidateRect(LPCRECT lpRect) = 0;
};

class UILIB_API CUIRootView : public CUIView
{
public:
	CUIRootView(IUIWindow *pOwner);
	virtual ~CUIRootView();

	void Destroy();
	BOOL ProcessWindowMessage(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, LRESULT &lResult);
	HWND GetHwnd() const;
	void UpdateLayout();
	void DoMouseMove();
	void InvalidateRect(LPCRECT lpRect);
	virtual void ClientToScreen(LPPOINT lpPoint);
	virtual void ScreenToClient(LPPOINT lpPoint);
	virtual UINT PopupUIMenu(CUIMenu *pUIMenu, int x1, int y1, int x2, int y2);
	virtual void SetCaretPos(LPCRECT lpRect);
	virtual void SetFocus(CUIControl *pFocus);
	virtual void SetCapture(CUIControl *pCapture);
	CUIControl *GetFocus() const { return m_pFocus; }
	IUIWindow *GetUIWindow() const { return m_pOwner; }

protected:
	virtual CUIView *OnCustomUI(LPCWSTR lpName, CUIView *pParent) override;
	virtual void OnLoad(const IUIXmlAttrs &attrs) override;
	virtual void PaintChilds(CUIDC &dc) const override;
	virtual void ShowToolTip(LPCWSTR lpTipText);
	bool OnNcHitTest(CPoint point);
	void OnMouseMessage(UINT uMsg, WPARAM wParam, LPARAM lParam);
	void DoMouseEnter(CUIView *pItem);
	void CheckMouseLeave(const UIHitTest &hitTest);
	void OnCaretTimer();

	IUIWindow  *m_pOwner;
	HCURSOR     m_hCursor;
	HWND        m_hToolTip;
	wstring     m_strTipText;
	CUIControl *m_pFocus;
	CUIControl *m_pCapture;
	bool        m_bComposing;	//  «∑Ò‘⁄ ‰»Î∫∫◊÷
	CRect       m_caretRect;
	CUITimer    m_caretTimer;
	vector<CUIView *> m_vecEnterItems;
};
