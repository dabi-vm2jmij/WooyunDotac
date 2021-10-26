#pragma once

#include "UIView.h"

// �ֲ��ؼ�

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

	Dir      m_dir;			// �Զ���������
	UINT     m_nElapse;
	bool     m_bRunning;
	int      m_nCurIndex;
	int      m_nCurFrame;	// ��ǰ����λ��
	int      m_nStep;		// ÿ֡��������
	CUITimer m_uiTimer;
	function<void()> m_fnOnChange;
};
