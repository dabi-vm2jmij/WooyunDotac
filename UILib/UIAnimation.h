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
	virtual void OnLoaded(const IUILoadAttrs &attrs) override;
	void OnUITimer();

	UINT     m_nElapse;
	bool     m_bLoop;
	UINT     m_nFrameIdx;
	CUITimer m_uiTimer;
};
