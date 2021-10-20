#pragma once

#include "UIControl.h"

class UILIB_API CUIGif : public CUIControl
{
public:
	CUIGif(CUIView *pParent, LPCWSTR lpFileName);
	virtual ~CUIGif();

	void Start(bool bLoop = true);
	void Stop();
	void Reset();

protected:
	virtual void DoPaint(CUIDC &dc) const override;
	void OnUITimer();

	Gdiplus::Image *m_pImage;
	vector<UINT> m_vecElapses;
	bool     m_bLoop;
	UINT     m_nFrameIdx;
	CUITimer m_uiTimer;
};
