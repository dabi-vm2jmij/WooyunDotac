#pragma once

#include "UIView.h"

class UILIB_API CUIControl : public CUIView
{
	friend class CUIRootView;
public:
	CUIControl(CUIView *pParent);

	void SetDraggable(bool bDraggable) { m_bDraggable = bDraggable; }
	bool IsDraggable() const { return m_bDraggable; }
	void SetCursor(HCURSOR hCursor) { m_hCursor = hCursor; }
	void SetToolTip(LPCWSTR lpText) { m_strToolTip = lpText ? lpText : L""; }
	virtual HCURSOR GetCursor() const { return m_hCursor; }
	virtual LPCWSTR GetToolTip() const { return m_strToolTip.c_str(); }

protected:
	virtual void OnLoad(const IUIXmlAttrs &attrs) override;
	virtual bool OnHitTest(UIHitTest &hitTest) override;
	virtual bool OnMessage(UINT uMsg, WPARAM wParam, LPARAM lParam) override;
	virtual int  IsPaint() const override { return 1; }
	virtual void OnMouseLeave() override;
	virtual void OnLButtonDown(CPoint point) {}
	virtual void OnLButtonUp(CPoint point) {}
	virtual void OnRButtonDown(CPoint point) {}
	virtual void OnRButtonUp(CPoint point);
	virtual void OnSetFocus() {}
	virtual void OnKillFocus() {}
	virtual void OnLostCapture();

	HCURSOR m_hCursor;
	wstring m_strToolTip;
	bool    m_bClickable;	// �Ƿ���Ӧ�����Ϣ
	bool    m_bDraggable;	// �϶�ʱ����ͼ�յ� OnChildMoving
	bool    m_bLButtonDown;
	bool    m_bRButtonDown;
	CSize   m_ptClick;		// �������������Ͻ�ƫ��
};
