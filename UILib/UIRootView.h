#pragma once
#include "UIView.h"

class UILIB_API CUIRootView : public CUIView
{
	friend class CUIBase;
	friend class CUIControl;
	friend class CUIEdit;
public:
	CUIRootView();
	virtual ~CUIRootView();

	void Attach(CWindowImplRoot<CWindow> *pOwner, WNDPROC pWndProc);
	void OnDrawBg(std::function<void(CUIDC &, LPCRECT)> &&fnDrawBg) { m_fnDrawBg = std::move(fnDrawBg); }
	void SetWndAlpha(BYTE nWndAlpha);
	void RaiseMouseMove();
	void InvalidateRect(LPCRECT lpRect);
	void PrintWindow(CImage &image);
	HWND GetHwnd() const;
	CUIControl *GetCapture() const { return m_pCapture; }

protected:
	bool OnNcHitTest(CPoint point);
	void OnSize(CSize size);
	void OnPaint();
	void OnPaintLayered(HDC hDC);
	void DoPaint(HDC hDC, LPCRECT lpClipRect);
	void OnMouseMessage(UINT uMsg, WPARAM wParam, LPARAM lParam);
	void DoMouseEnter(CUIBase *pItem);
	void CheckMouseLeave(const UIHitTest &hitTest);
	void SetCapture(CUIControl *pCtrl);
	void ReleaseCapture();
	void SetFocusCtrl(CUIControl *pCtrl);
	void AddTabsEdit(CUIEdit *pEdit);
	void DelTabsEdit(CUIEdit *pEdit);
	void NextTabsEdit();
	void EnableImm(bool bEnabled);

	CImage      m_imageWnd;		// ͸�����ڻ���
	CRect       m_rectClip;		// ��ǰ��Ч����
	BYTE        m_nWndAlpha;	// ͸���ȣ�0~255
	bool        m_bLayered;
	bool        m_bMouseEnter;
	HIMC        m_hImc;
	HCURSOR     m_hCursor;
	CUIControl *m_pCapture;
	CUIControl *m_pCurFocus;
	std::vector<CUIEdit *> m_vecEdits;
	std::vector<CUIBase *> m_vecEnterItems;
	std::function<void(CUIDC &, LPCRECT)> m_fnDrawBg;

private:
	LRESULT CALLBACK MyWndProc(UINT uMsg, WPARAM wParam, LPARAM lParam);

	WNDPROC     m_pWndProc;
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