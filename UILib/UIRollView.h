#pragma once

#include "UIView.h"

// 可以滚动任何控件，最常用为从上向下滚动广告文字

class UILIB_API CUIRollView : public CUIView
{
public:
	CUIRollView(CUIView *pParent);
	virtual ~CUIRollView();

	enum Dir { ToLeft, ToTop, ToRight, ToBottom };

	void SetDir(Dir dir) { m_dir = dir; }
	void SetElapse(UINT nElapse) { m_nElapse = nElapse; }
	void Start();
	void Stop();

protected:
	virtual void OnLoad(const IUIXmlAttrs &attrs) override;
	virtual void RecalcLayout(LPRECT lpClipRect) override;
	void OnUITimer();

	Dir      m_dir;
	UINT     m_nElapse;
	bool     m_bRunning;
	int      m_nCurPos;
	CUITimer m_uiTimer;
};
