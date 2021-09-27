#pragma once

#include "UIView.h"

class IUIWindow
{
public:
	virtual CUIView *OnCustomUI(LPCWSTR lpName, CUIView *pParent) { return NULL; }
	virtual void OnLoadedUI(const IUILoadAttrs &attrs) {}
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
	HWND GetOwnerWnd() const;
	void SetWndAlpha(BYTE nWndAlpha);
	void DoPaint(HDC hDC, LPCRECT lpClipRect) const;
	void RaiseMouseMove();
	void DoMouseEnter(CUIBase *pItem);
	void SetFocus(CUIControl *pCtrl);
	virtual void SetCapture(CUIControl *pCtrl);
	virtual void InvalidateRect(LPCRECT lpRect);
	virtual BOOL ClientToScreen(LPPOINT lpPoint);
	virtual BOOL ScreenToClient(LPPOINT lpPoint);
	CUIView *OnCustomUI(LPCWSTR lpName, CUIView *pParent);
	CUIControl *GetCapture() const { return m_pCapture; }

protected:
	virtual void OnLoaded(const IUILoadAttrs &attrs) override;
	bool OnNcHitTest(CPoint point);
	void OnSize(CSize size);
	void OnPaint();
	void OnPaintLayered(HDC hDC);
	void OnMouseMessage(UINT uMsg, WPARAM wParam, LPARAM lParam);
	void CheckMouseLeave(const UIHitTest &hitTest);
	void EnableImm(bool bEnabled);
	void AddTabsEdit(CUIEdit *pEdit);
	void DelTabsEdit(CUIEdit *pEdit);
	void NextTabsEdit();

	IUIWindow  *m_pOwner;
	CImage      m_imageWnd;		// 透明窗口缓存
	CRect       m_rectClip;		// 当前无效区域
	BYTE        m_nWndAlpha;	// 透明度：0~255
	bool        m_bLayered;
	bool        m_bMouseEnter;
	HIMC        m_hImc;
	HCURSOR     m_hCursor;
	CUIControl *m_pCapture;
	CUIControl *m_pCurFocus;
	std::vector<CUIEdit *> m_vecEdits;
	std::vector<CUIBase *> m_vecEnterItems;
};
