#pragma once

#include "UIView.h"

class IUIWindow
{
public:
	virtual CUIView *OnCustomUI(LPCWSTR lpName, CUIView *pParent) { return NULL; }
	virtual void OnLoadedUI(const IUIXmlAttrs &attrs) {}
	virtual void OnDrawBg(CUIDC &dc, LPCRECT lpRect) const {}
	virtual HWND GetHwnd() const = 0;
};

class UILIB_API CUIRootView : public CUIView
{
	friend class CUIEdit;
public:
	CUIRootView(IUIWindow *pOwner);
	virtual ~CUIRootView();

	BOOL ProcessWindowMessage(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, LRESULT &lResult);
	HWND GetHwnd() const;
	void SetWndAlpha(BYTE nWndAlpha);
	void UpdateLayout();
	void DoPaint(HDC hDC, LPCRECT lpClipRect) const;
	void RaiseMouseMove();
	void SetFocus(CUIControl *pCtrl);
	virtual void SetCapture(CUIControl *pCtrl);
	virtual void InvalidateRect(LPCRECT lpRect);
	virtual BOOL ClientToScreen(LPPOINT lpPoint);
	virtual BOOL ScreenToClient(LPPOINT lpPoint);
	CUIView *OnCustomUI(LPCWSTR lpName, CUIView *pParent);
	CUIControl *GetFocus() const { return m_pFocus; }
	CUIControl *GetCapture() const { return m_pCapture; }

protected:
	virtual void ShowToolTip(LPCWSTR lpTipText);
	virtual void OnLoaded(const IUIXmlAttrs &attrs) override;
	bool OnNcHitTest(CPoint point);
	void OnSize(CSize size);
	void OnPaint();
	void OnPaintLayered(HDC hDC);
	void OnMouseMessage(UINT uMsg, WPARAM wParam, LPARAM lParam);
	void DoMouseEnter(CUIBase *pItem);
	void CheckMouseLeave(const UIHitTest &hitTest);
	void EnableImm(bool bEnable);
	void AddTabsEdit(CUIEdit *pEdit);
	void DelTabsEdit(CUIEdit *pEdit);
	void NextTabsEdit();

	IUIWindow  *m_pOwner;
	CImage      m_imageWnd;		// 透明窗口缓存
	CRect       m_clipRect;		// 当前无效区域
	BYTE        m_nWndAlpha;	// 透明度：0~255
	bool        m_bLayered;
	HIMC        m_hImc;
	HCURSOR     m_hCursor;
	HWND        m_hToolTip;
	wstring     m_strTipText;
	CUIControl *m_pFocus;
	CUIControl *m_pCapture;
	vector<CUIEdit *> m_vecEdits;
	vector<CUIBase *> m_vecEnterItems;
};
