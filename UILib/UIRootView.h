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
	friend class CUIEdit;
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
	virtual void ShowToolTip(LPCWSTR lpTipText);
	virtual void SetFocus(CUIControl *pCtrl);
	virtual void SetCapture(CUIControl *pCtrl);
	CUIControl *GetFocus() const { return m_pFocus; }
	CUIControl *GetCapture() const { return m_pCapture; }
	IUIWindow *GetUIWindow() const { return m_pOwner; }

protected:
	virtual CUIView *OnCustomUI(LPCWSTR lpName, CUIView *pParent) override;
	virtual void OnLoad(const IUIXmlAttrs &attrs) override;
	virtual void OnPaint(CUIDC &dc) const override;
	bool OnNcHitTest(CPoint point);
	void OnMouseMessage(UINT uMsg, WPARAM wParam, LPARAM lParam);
	void DoMouseEnter(CUIView *pItem);
	void CheckMouseLeave(const UIHitTest &hitTest);
	void EnableImm(bool bEnable);
	void AddTabsEdit(CUIEdit *pEdit);
	void DelTabsEdit(CUIEdit *pEdit);
	void NextTabsEdit();

	IUIWindow  *m_pOwner;
	HIMC        m_hImc;
	HCURSOR     m_hCursor;
	HWND        m_hToolTip;
	wstring     m_strTipText;
	CUIControl *m_pFocus;
	CUIControl *m_pCapture;
	vector<CUIEdit *> m_vecEdits;
	vector<CUIView *> m_vecEnterItems;
};
