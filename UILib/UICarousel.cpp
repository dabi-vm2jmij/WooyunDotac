#include "stdafx.h"
#include "UICarousel.h"

#define FRAMES	10

CUICarousel::CUICarousel(CUIView *pParent) : CUIView(pParent), m_dir(ToLeft), m_nElapse(3000), m_bRunning(false), m_nCurIndex(0), m_nCurFrame(0), m_nStep(1), m_uiTimer([this]{ OnUITimer(); })
{
}

void CUICarousel::RecalcLayout(LPRECT lpClipRect)
{
	if (m_rect.left == MAXINT16 || m_vecChilds.empty())
		return;

	int nIndex1 = m_nCurFrame / FRAMES, nOffset = m_nCurFrame % FRAMES;

	if (nOffset == 0)
	{
		for (int i = 0; i != m_vecChilds.size(); i++)
		{
			if (i != nIndex1)
				m_vecChilds[i]->SetRect(NULL, lpClipRect);
		}

		FRIEND(m_vecChilds[nIndex1])->CalcRect(CRect(m_rect), lpClipRect);
		return;
	}

	int nIndex2 = (nIndex1 + 1) % m_vecChilds.size();
	CRect rect1(m_rect), rect2;

	switch (m_dir)
	{
	case ToRight:
		std::swap(nIndex1, nIndex2);
		nOffset = FRAMES - nOffset;

	case ToLeft:
		rect1.OffsetRect(-rect1.Width() * nOffset / FRAMES, 0);
		rect2 = rect1;
		rect2.OffsetRect(rect2.Width(), 0);
		break;

	case ToBottom:
		std::swap(nIndex1, nIndex2);
		nOffset = FRAMES - nOffset;

	case ToTop:
		rect1.OffsetRect(0, -rect1.Height() * nOffset / FRAMES);
		rect2 = rect1;
		rect2.OffsetRect(0, rect2.Height());
		break;
	}

	for (int i = 0; i != m_vecChilds.size(); i++)
	{
		if (i != nIndex1 && i != nIndex2)
			m_vecChilds[i]->SetRect(NULL, lpClipRect);
	}

	FRIEND(m_vecChilds[nIndex1])->CalcRect(rect1, lpClipRect);
	FRIEND(m_vecChilds[nIndex2])->CalcRect(rect2, lpClipRect);
}

void CUICarousel::OnUITimer()
{
	if (m_nCurFrame == m_nCurIndex * FRAMES)
	{
		// 开始滚动动画
		m_nCurIndex = (m_nCurIndex + 1) % m_vecChilds.size();

		if (m_fnOnChange)
			m_fnOnChange();

		m_nStep = 1;
		m_uiTimer.Start(30);
		OnUITimer();
	}
	else
	{
		int nAllFrames = m_vecChilds.size() * FRAMES;
		m_nCurFrame = (m_nCurFrame + m_nStep + nAllFrames) % nAllFrames;
		InvalidateLayout();

		if (m_nCurFrame == m_nCurIndex * FRAMES)
		{
			// 滚动动画结束
			if (m_bRunning)
				m_uiTimer.Start(m_nElapse);
			else
				m_uiTimer.Stop();
		}
	}
}

void CUICarousel::Start()
{
	if (m_vecChilds.size() < 2)
	{
		ATLASSERT(0);
		return;
	}

	m_bRunning = true;

	if (m_nCurFrame == m_nCurIndex * FRAMES)
		m_uiTimer.Start(m_nElapse);
}

void CUICarousel::Stop()
{
	m_bRunning = false;

	if (m_nCurFrame == m_nCurIndex * FRAMES)
		m_uiTimer.Stop();
}

void CUICarousel::SetIndex(int nIndex)
{
	// 先计算帧数再修正 nIndex
	int nFrames = nIndex * FRAMES - m_nCurFrame;
	nIndex = (nIndex + m_vecChilds.size()) % m_vecChilds.size();

	if (m_nCurIndex == nIndex)
		return;

	m_nCurIndex = nIndex;

	if (m_fnOnChange)
		m_fnOnChange();

	if (nFrames == 0)
	{
		if (m_bRunning)
			m_uiTimer.Start(m_nElapse);
		else
			m_uiTimer.Stop();
		return;
	}

	if (nFrames < 0)
	{
		// 逆向滚动帧数多改为正向
		int nFrames2 = nFrames + m_vecChilds.size() * FRAMES;
		if (-nFrames > nFrames2)
			nFrames = nFrames2;
	}

	// 每帧滚动距离
	m_nStep = nFrames / FRAMES;

	if (nFrames % FRAMES)
	{
		// 向上取整
		if (nFrames < 0)
			m_nStep--;
		else
			m_nStep++;
	}

	if (int nMod = nFrames % m_nStep)
	{
		// 减掉余数，比如 m_nCurFrame=18，nIndex=4，则 nFrames=22，m_nStep=3，应 m_nCurFrame -= 2，保证 16 + 3 * 8 = 40
		if (nFrames < 0)
			m_nCurFrame += nMod - m_nStep;
		else
			m_nCurFrame -= m_nStep - nMod;
	}

	m_uiTimer.Start(30);
	OnUITimer();
}

void CUICarousel::OnLoad(const IUIXmlAttrs &attrs)
{
	__super::OnLoad(attrs);

	int nValue;
	if (attrs.GetInt(L"elapse", &nValue))
		SetElapse(nValue);

	if (attrs.GetInt(L"start"))
		Start();

	LPCWSTR lpStr = attrs.GetStr(L"dir");
	if (lpStr == NULL)
		return;

	struct
	{
		LPCWSTR lpStr;
		Dir     dir;
	} items[] = { { L"toLeft", ToLeft }, { L"toTop", ToTop }, { L"toRight", ToRight }, { L"toBottom", ToBottom } };

	for (auto item : items)
	{
		if (_wcsicmp(item.lpStr, lpStr) == 0)
		{
			SetDir(item.dir);
			return;
		}
	}

	ATLASSERT(0);
}
