#pragma once

#include "UIControl.h"

// �϶����飬����ϵͳ���������ƣ��϶���������

class UILIB_API CUISlider : public CUIControl
{
public:
	CUISlider(CUIView *pParent, LPCWSTR lpFileName, LPCWSTR lpFileNameBg);
	virtual ~CUISlider();

	void OnChanged(std::function<void(int)> &&fnOnChanged) { m_fnOnChanged = std::move(fnOnChanged); }
	void SetMaxPos(int nMaxPos);
	void SetCurPos(int nCurPos);
	int  GetCurPos() const { return (int)(m_fCurPos + 0.5); }

protected:
	virtual void OnLoaded(const IUILoadAttrs &attrs) override;
	virtual void OnRectChanged(LPCRECT lpOldRect, LPRECT lpClipRect) override;
	virtual void OnLButtonDown(CPoint point) override;
	virtual void OnChildMoving(CUIControl *, CPoint point) override;
	void ResetOffset(int nOffset, bool bSetPos);

	CUIProgress *m_pProgress;
	CUIButton   *m_pButton;
	int          m_nMaxPos;
	double       m_fCurPos;
	std::function<void(int)> m_fnOnChanged;
};
