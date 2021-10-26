#pragma once

#include "UIView.h"

// 轮播控件

class UILIB_API CUICarousel : public CUIView
{
public:
	CUICarousel(CUIView *pParent);

	enum Dir { ToLeft, ToTop, ToRight, ToBottom };

	void BindChange(function<void()> &&fnOnChange) { m_fnOnChange = std::move(fnOnChange); }
	void SetDir(Dir dir) { m_dir = dir; }
	void SetElapse(UINT nElapse) { m_nElapse = nElapse; }
	void Start();
	void Stop();
	void SetIndex(int nIndex);
	int  GetIndex() const { return m_nCurIndex; }

protected:
	virtual void OnLoad(const IUIXmlAttrs &attrs) override;
	virtual void RecalcLayout(LPRECT lpClipRect) override;
	void OnUITimer();

	Dir      m_dir;			// 自动滚动方向
	UINT     m_nElapse;
	bool     m_bRunning;
	int      m_nCurIndex;
	int      m_nCurFrame;	// 当前动画位置
	int      m_nStep;		// 每帧滚动距离
	CUITimer m_uiTimer;
	function<void()> m_fnOnChange;
};
