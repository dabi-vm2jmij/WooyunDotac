#pragma once

#include "UIView.h"

class UILIB_API CUIRootView : public CUIView
{
	friend class CUIView;
	friend class CUIEdit;
public:
	CUIRootView();
	virtual ~CUIRootView();

	void Attach(CWindowImplRoot<CWindow> *pOwner, WNDPROC pWndProc);
	void OnDrawBg(std::function<void(CUIDC &, LPCRECT)> &&fnDrawBg) { m_fnDrawBg = std::move(fnDrawBg); }
	void SetWndAlpha(BYTE nWndAlpha);
	void RaiseMouseMove();
	void DoMouseEnter(CUIBase *pItem);
	void SetCapture(CUIControl *pCtrl);
	void SetFocus(CUIControl *pCtrl);
	void EnableImm(bool bEnabled);
	void PrintWindow(CImage &image);
	HWND GetHwnd() const;

protected:
	bool OnNcHitTest(CPoint point);
	void OnSize(CSize size);
	void OnPaint();
	void OnPaintLayered(HDC hDC);
	void DoPaint(HDC hDC, LPCRECT lpClipRect);
	void OnMouseMessage(UINT uMsg, WPARAM wParam, LPARAM lParam);
	void CheckMouseLeave(const UIHitTest &hitTest);
	void DoInvalidateRect(LPCRECT lpRect);
	void DoInvalidateLayout();
	void AddTabsEdit(CUIEdit *pEdit);
	void DelTabsEdit(CUIEdit *pEdit);
	void NextTabsEdit();

	CImage      m_imageWnd;		// 透明窗口缓存
	CRect       m_rectClip;		// 当前无效区域
	BYTE        m_nWndAlpha;	// 透明度：0~255
	bool        m_bLayered;
	bool        m_bMouseEnter;
	bool        m_bPostLayout;
	HIMC        m_hImc;
	HCURSOR     m_hCursor;
	CUIControl *m_pCapture;
	CUIControl *m_pCurFocus;
	std::vector<CUIEdit *> m_vecEdits;
	std::vector<CUIBase *> m_vecEnterItems;
	std::function<void(CUIDC &, LPCRECT)> m_fnDrawBg;

	static UINT m_nLayoutMsgId;

private:
	LRESULT CALLBACK MyWndProc(UINT uMsg, WPARAM wParam, LPARAM lParam);

	WNDPROC m_pWndProc;
	CWindowImplRoot<CWindow> *m_pOwner;
};

#define DECLARE_UI_ROOTVIEW_EX(RootViewT, rootView) \
protected: \
	RootViewT rootView; \
public: \
	WNDPROC GetWindowProc() final \
	{ \
		rootView.Attach(this, __super::GetWindowProc()); \
		__asm mov eax, lb1 \
		__asm jmp lb2 \
		__asm lb1: \
		__asm add dword ptr [esp + 4], rootView \
		__asm jmp CUIRootView::MyWndProc \
		__asm lb2: \
	}

#define DECLARE_UI_ROOTVIEW(rootView)	DECLARE_UI_ROOTVIEW_EX(CUIRootView, rootView)
