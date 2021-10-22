#pragma once

#include "UIControl.h"

class UILIB_API CUIImage : public CUIControl
{
public:
	CUIImage(CUIView *pParent, LPCWSTR lpFileName);
	virtual ~CUIImage();

	void SetImage(const CImagex &imagex);
	void SetNoClipRgn() { m_bClipRgn = false; }

protected:
	virtual void OnLoad(const IUIXmlAttrs &attrs) override;
	virtual void OnPaint(CUIDC &dc) const override;
	virtual void CalcRect(LPRECT lpRect, LPRECT lpClipRect) override;

	CImagex m_imagex;
	bool    m_bClipRgn;		// 是否剪裁超出父视图的区域。为了效率，只无效超出的区域不重绘
};
