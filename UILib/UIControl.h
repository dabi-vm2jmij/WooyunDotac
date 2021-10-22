#pragma once

#include "UIView.h"

class UILIB_API CUIControl : public CUIView
{
	friend class CUIRootView;
public:
	CUIControl(CUIView *pParent);
	virtual ~CUIControl();

	void SetCurImage(const CImagex &imagex);
	void SetStretch(bool bStretch) { m_bStretch = bStretch; }
	void SetDraggable(bool bDraggable) { m_bDraggable = bDraggable; }
	void SetCursor(HCURSOR hCursor) { m_hCursor = hCursor; }
	void SetToolTip(LPCWSTR lpText) { m_strToolTip = lpText ? lpText : L""; }
	virtual HCURSOR GetCursor() const { return m_hCursor; }
	virtual LPCWSTR GetToolTip() const { return m_strToolTip.c_str(); }

protected:
	virtual void OnLoad(const IUIXmlAttrs &attrs) override;
	virtual bool OnHitTest(UIHitTest &hitTest) override;
	virtual bool OnMessage(UINT uMsg, WPARAM wParam, LPARAM lParam) override;
	virtual void OnPaint(CUIDC &dc) const override;
	virtual void OnEnable(bool bEnable) override;
	virtual void OnRectChange(LPCRECT lpOldRect, LPRECT lpClipRect) override;
	virtual void OnMouseLeave() override;
	virtual void OnLButtonDown(CPoint point) {}
	virtual void OnLButtonUp(CPoint point) {}
	virtual void OnRButtonDown(CPoint point) {}
	virtual void OnRButtonUp(CPoint point) {}
	virtual void DoPaint(CUIDC &dc) const {}
	virtual void OnSetFocus() {}
	virtual void OnKillFocus() {}
	virtual void OnLostCapture();

	CImagex m_curImagex;
	bool    m_bStretch;		// 绘制时使用 StretchDraw
	bool    m_bClickable;	// 是否响应鼠标消息
	bool    m_bDraggable;	// 拖动时父控件收到 OnChildMoving
	HCURSOR m_hCursor;
	wstring m_strToolTip;
	bool    m_bLButtonDown;
	bool    m_bRButtonDown;
	CSize   m_ptClick;		// 鼠标点击相对于左上角偏移
};
