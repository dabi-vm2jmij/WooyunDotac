#pragma once

#include "UIControl.h"

// ½ø¶ÈÌõ

class UILIB_API CUIProgress : public CUIControl
{
public:
	CUIProgress(CUIView *pParent, LPCWSTR lpFileName);
	virtual ~CUIProgress();

	void SetValue(int nCurPos, int nMaxPos = 0);

protected:
	virtual void OnLoaded(const IUIXmlAttrs &attrs) override;
	virtual void MyPaint(CUIDC &dc) const override;

	CImagex m_imagexs[2];
	int     m_nMaxPos;
	int     m_nCurPos;
};
