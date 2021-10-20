#pragma once

class IUIXmlAttrs;

class UILIB_API CUIBase
{
	friend class CUIView;
	friend class CUIRootView;
	friend class CUILoader;
public:
	CUIBase(CUIView *pParent);
	virtual ~CUIBase();

	void SetId(const wstring &strId) { m_strId = strId; }
	void SetId(wstring &&strId) { m_strId = std::move(strId); }
	LPCWSTR GetId() const { return m_strId.c_str(); }
	void SetLeft(int nLeft, bool bAlign = false);
	void SetRight(int nRight, bool bAlign = false);
	void SetTop(int nTop, bool bAlign = false);
	void SetBottom(int nBottom, bool bAlign = false);
	void SetWidth(int nWidth);
	void SetHeight(int nHeight);
	void SetSize(CSize size);
	CSize GetSize() const { return m_size; }
	CRect GetRect() const { return m_rect; }
	CRect GetWindowRect() const;
	void InvalidateRect(LPCRECT lpRect);
	void SetBgColor(COLORREF color) { m_colorBg = color; }
	void SetEnable(bool bEnable);
	void SetVisible(bool bVisible);
	bool IsEnabled() const { return m_bEnable; }
	bool IsVisible() const { return m_bVisible; }
	bool IsRealEnabled() const;
	bool IsRealVisible() const;
	bool IsFullVisible() const;
	bool IsChild(const CUIBase *pItem) const;
	bool IsControl() const { return m_bControl; }
	bool IsMouseEnter() const { return m_bMouseEnter; }
	CUIView *GetParent() const;
	CUIRootView *GetRootView() const;

protected:
	struct UIHitTest
	{
		CPoint point;
		UINT   nCount;

		struct
		{
			CUIBase *pItem;
			bool     bEnable;
		} items[20];

		UIHitTest() : nCount(0) {}

		void Add(CUIBase *pItem)
		{
			if (nCount < _countof(items))
				items[nCount++] = { pItem, pItem->IsEnabled() };
		}

		auto begin() const -> decltype(items + 0)
		{
			return items;
		}

		auto end() const -> decltype(items + 0)
		{
			return items + nCount;
		}
	};

	virtual void OnLoaded(const IUIXmlAttrs &attrs);
	virtual bool OnHitTest(UIHitTest &hitTest) { return false; }
	virtual bool OnMessage(UINT uMsg, WPARAM wParam, LPARAM lParam) { return false; }
	virtual void OnPaint(CUIDC &dc) const {}
	virtual void OnEnable(bool bEnable) {}
	virtual void OnRectChange(LPCRECT lpOldRect, LPRECT lpClipRect) {}
	virtual void OnNeedLayout(LPRECT lpClipRect) {}
	virtual void SetRect(LPCRECT lpRect, LPRECT lpClipRect);
	virtual void CalcRect(LPRECT lpRect, LPRECT lpClipRect);
	virtual void OnMouseEnter() {}
	virtual void OnMouseLeave() {}
	static void CalcLeftRight(long &nLeft, long &nRight, long nOffsetLeft, long nOffsetRight, long nWidth);

	CUIView *m_pParent;
	CRect    m_offset;		// 上下左右的偏移
	CSize    m_size;		// 宽、高 == 0 为填充
	CRect    m_rect;		// 完整区域，包括不可见部分
	CRect    m_rcReal;		// 实际可见区域
	wstring  m_strId;
	COLORREF m_colorBg;
	bool     m_bEnable;
	bool     m_bVisible;
	bool     m_bControl;	// 是否 CUIControl
	bool     m_bMouseEnter;

private:
	void MySetRect(CRect &rect, CRect &rcReal, LPRECT lpClipRect);
	void MyPaint(CUIDC &dc) const;

	CUIBase(const CUIBase &) = delete;
	CUIBase &operator=(const CUIBase &) = delete;
};
