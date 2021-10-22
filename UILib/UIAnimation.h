#pragma once

#include "UIControl.h"

// ∑÷÷°∂Øª≠

class UILIB_API CUIAnimation : public CUIControl
{
public:
	CUIAnimation(CUIView *pParent, LPCWSTR lpFileName);
	virtual ~CUIAnimation();

	void SetElapse(UINT nElapse) { m_nElapse = nElapse; }
	void Start(bool bLoop = true);
	void Stop();
	void Reset();

protected:
	virtual void OnLoad(const IUIXmlAttrs &attrs) override;
	virtual void OnPaint(CUIDC &dc) const override;
	void OnUITimer();

	CImagex  m_imagex;
	UINT     m_nElapse;
	bool     m_bLoop;
	UINT     m_nFrameIdx;
	CUITimer m_uiTimer;
};
