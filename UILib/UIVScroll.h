#pragma once

#include "UIControl.h"

// ���������

class UILIB_API CUIVScroll : public CUIControl
{
public:
	CUIVScroll(CUIView *pParent, LPCWSTR lpFileName, LPCWSTR lpFileNameBg);
	virtual ~CUIVScroll();

	void OnChanged(std::function<void(int)> &&fnOnChanged) { m_fnOnChanged = std::move(fnOnChanged); }
	void SetRange(int nMinPos, int nMaxPos);	// nMinPos : nMaxPos == ����߶� : �������߶�
	void SetCurPos(int nCurPos);
	int  GetCurPos() const { return (int)(m_fCurPos + 0.5); }

protected:
	virtual void OnLoaded(const IUILoadAttrs &attrs) override;
	virtual bool OnMessage(UINT uMsg, WPARAM wParam, LPARAM lParam) override;
	virtual void OnRectChanged(LPCRECT lpOldRect, LPRECT lpClipRect) override;
	virtual void OnLButtonDown(CPoint point) override;
	virtual void OnChildMoving(CUIControl *, CPoint point) override;
	void ResetOffset(int nOffset, bool bSetPos);

	CUIButton *m_pButton;
	int        m_nMinPos;
	int        m_nMaxPos;
	double     m_fCurPos;
	std::function<void(int)> m_fnOnChanged;
};
