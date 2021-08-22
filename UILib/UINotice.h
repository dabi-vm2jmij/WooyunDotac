#pragma once
#include "UIView.h"

// ������ʾ�����Թ����κοؼ������Ϊ�������¹����������

class UILIB_API CUINotice : public CUIView
{
public:
	CUINotice(CUIView *pParent);
	virtual ~CUINotice();

	enum Dir { ToLeft, ToTop, ToRight, ToBottom };

	void SetDir(Dir dir) { m_dir = dir; }
	void SetElapse(UINT nElapse) { m_nElapse = nElapse; }
	void Start();
	void Stop();

protected:
	virtual void OnLoaded(const IUILoadAttrs &attrs) override;
	virtual void RecalcLayout(LPRECT lpClipRect) override;
	void OnUITimer();

	Dir      m_dir;
	UINT     m_nElapse;
	bool     m_bRunning;
	int      m_nCurPos;
	CUITimer m_uiTimer;
};
