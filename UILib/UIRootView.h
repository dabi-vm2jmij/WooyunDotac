#pragma once

#include "UIView.h"

class IUIWindow
{
public:
	virtual CUIView *OnCustomUI(LPCWSTR lpName, CUIView *pParent) { return NULL; }
	virtual void OnLoadedUI(const IUILoadAttrs &attrs) {}
	virtual void OnDrawBg(CUIDC &dc, LPCRECT lpRect) {}
	virtual HWND GetHwnd() const = 0;
};

class UILIB_API CUIRootView : public CUIView
{
	friend class CUIView;
	friend class CUIEdit;
public:
	CUIRootView(IUIWindow *pWindow);
	virtual ~CUIRootView();

	BOOL ProcessWindowMessage(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, LRESULT &lResult);
	HWND GetHwnd() const;
	void SetWndAlpha(BYTE nWndAlpha);
	void RaiseMouseMove();
	void DoMouseEnter(CUIBase *pItem);
	void SetCapture(CUIControl *pCtrl);
	void SetFocus(CUIControl *pCtrl);
	void PrintWindow(CImage &image);
	BOOL ClientToScreen(LPPOINT lpPoint);
	BOOL ScreenToClient(LPPOINT lpPoint);
	CUIView *OnCustomUI(LPCWSTR lpName, CUIView *pParent);

protected:
	void OnLoaded(const IUILoadAttrs &attrs) override;
	bool OnNcHitTest(CPoint point);
	void OnSize(CSize size);
	void OnPaint();
	void OnPaintLayered(HDC hDC);
	void DoPaint(HDC hDC, LPCRECT lpClipRect);
	void OnMouseMessage(UINT uMsg, WPARAM wParam, LPARAM lParam);
	void CheckMouseLeave(const UIHitTest &hitTest);
	void DoInvalidateRect(LPCRECT lpRect);
	void DoInvalidateLayout();
	void EnableImm(bool bEnabled);
	void AddTabsEdit(CUIEdit *pEdit);
	void DelTabsEdit(CUIEdit *pEdit);
	void NextTabsEdit();

	IUIWindow  *m_pWindow;
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
