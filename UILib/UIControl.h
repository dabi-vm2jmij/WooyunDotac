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

protected:
	virtual void OnLoaded(const IUILoadAttrs &attrs) override;
	virtual bool OnHitTest(UIHitTest &hitTest) override;
	virtual bool OnMessage(UINT uMsg, WPARAM wParam, LPARAM lParam) override;
	virtual void OnPaint(CUIDC &dc) const override;
	virtual bool DoMouseLeave(bool bForce) override;
	virtual void OnLButtonDown(CPoint point) {}
	virtual void OnLButtonUp(CPoint point) {}
	virtual void OnRButtonDown(CPoint point) {}
	virtual void OnRButtonUp(CPoint point) {}
	virtual void MyPaint(CUIDC &dc) const {}
	virtual void OnLostCapture();
	virtual void OnSetFocus() {}
	virtual void OnKillFocus() {}
	void SetCapture();
	void ReleaseCapture();
	void SetFocusCtrl(CUIControl *pCtrl);

	CImagex m_curImagex;
	bool    m_bStretch;		// ����ʱʹ�� StretchDraw
	bool    m_bClickable;	// �Ƿ���Ӧ�����Ϣ
	bool    m_bDraggable;	// �϶�ʱ���ؼ��յ� OnChildMoving
	HCURSOR m_hCursor;
	wstring m_strToolTip;
	bool    m_bLButtonDown;
	bool    m_bRButtonDown;
	CSize   m_ptClick;		// �������������Ͻ�ƫ��
};