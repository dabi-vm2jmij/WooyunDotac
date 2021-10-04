#pragma once

#include "UIControl.h"

// 拖动滑块，比如系统的音量控制，拖动调整音量

class UILIB_API CUISlider : public CUIControl
{
public:
	CUISlider(CUIView *pParent, LPCWSTR lpFileName, LPCWSTR lpFileNameBg);
	virtual ~CUISlider();

	void OnChanged(function<void(int)> &&fnOnChanged) { m_fnOnChanged = std::move(fnOnChanged); }
	void SetMaxPos(int nMaxPos);
	void SetCurPos(int nCurPos);
	int  GetCurPos() const { return (int)(m_fCurPos + 0.5); }

protected:
	virtual void OnLoaded(const IUIXmlAttrs &attrs) override;
	virtual void OnRectChanged(LPCRECT lpOldRect, LPRECT lpClipRect) override;
	virtual void OnLButtonDown(CPoint point) override;
	virtual void OnChildMoving(CUIControl *, CPoint point) override;
	void ResetOffset(int nOffset, bool bSetPos);

	CUIProgress *m_pProgress;
	CUIButton   *m_pButton;
	int          m_nMaxPos;
	double       m_fCurPos;
	function<void(int)> m_fnOnChanged;
};
