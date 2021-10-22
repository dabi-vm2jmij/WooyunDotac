#pragma once

#include "UIControl.h"

// ½ø¶ÈÌõ

class UILIB_API CUIProgress : public CUIControl
{
public:
	CUIProgress(CUIView *pParent, LPCWSTR lpFileName);
	virtual ~CUIProgress();

	void SetMaxPos(int nMaxPos);
	void SetCurPos(int nCurPos);

protected:
	virtual void OnLoad(const IUIXmlAttrs &attrs) override;
	virtual void OnPaint(CUIDC &dc) const override;

	CImagex m_imagex;
	int     m_nMaxPos;
	int     m_nCurPos;
};
