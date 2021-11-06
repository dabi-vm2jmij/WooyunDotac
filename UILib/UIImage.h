#pragma once

#include "UIControl.h"

class UILIB_API CUIImage : public CUIControl
{
public:
	CUIImage(CUIView *pParent, LPCWSTR lpFileName);

	void SetImage(const CImagex &imagex);
	CImagex GetImage() const { return m_imagex; }
	void SetClipParent(bool bClipParent) { m_bClipParent = bClipParent; }

protected:
	virtual void OnLoad(const IUIXmlAttrs &attrs) override;
	virtual void OnPaint(CUIDC &dc) const override;
	virtual void CalcRect(LPRECT lpRect, LPRECT lpClipRect) override;

	CImagex m_imagex;
	bool    m_bClipParent;	// 是否剪裁超出父视图的区域。为了效率，只无效超出的区域不重绘
};
