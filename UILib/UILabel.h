#pragma once

#include "UIControl.h"
#include "UITextImpl.h"

class UILIB_API CUILabel : public CUIControl, public CUITextImpl
{
public:
	CUILabel(CUIView *pParent);
	virtual ~CUILabel();

protected:
	virtual void OnLoaded(const IUIXmlAttrs &attrs) override;
	virtual void DoPaint(CUIDC &dc) const override;
	virtual void OnTextSize(CSize size) override;
};
