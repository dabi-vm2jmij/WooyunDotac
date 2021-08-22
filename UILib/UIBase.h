#pragma once

class IUILoadAttrs;

class UILIB_API CUIBase
{
	friend class CUIView;
	friend class CUIRootView;
	friend class CUILoader;
public:
	CUIBase(CUIView *pParent);
	virtual ~CUIBase();

	void SetLeft(int nLeft, bool bClip = false);
	void SetRight(int nRight, bool bClip = false);
	void SetTop(int nTop, bool bClip = false);
	void SetBottom(int nBottom, bool bClip = false);
	void SetWidth(int nWidth) { m_size.cx = nWidth; }
	void SetHeight(int nHeight) { m_size.cy = nHeight; }
	void SetSize(CSize size) { m_size = size; }
	void GetSize(LPSIZE lpSize) const;
	void GetClientRect(LPRECT lpRect) const;
	void GetWindowRect(LPRECT lpRect) const;
	void InvalidateRect(LPCRECT lpRect);
	void SetEnabled(bool bEnabled);
	void SetVisible(bool bVisible, bool bUpdate = false);
	bool IsControl() const { return m_bControl; }
	bool IsEnabled() const { return m_bEnabled; }
	bool IsVisible() const { return m_bVisible; }
	bool IsRealEnabled() const;
	bool IsRealVisible() const;
	bool IsChild(const CUIBase *pItem) const;
	bool IsMouseEnter() const { return m_ppEnter != NULL; }
	void DoMouseEnter();
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
			bool     bEnabled;
		} items[64];

		UIHitTest() : nCount(0) {}

		void Add(CUIBase *pItem)
		{
			if (nCount < _countof(items))
				items[nCount++] = { pItem, pItem->IsEnabled() };
		}

		auto begin() const->decltype(items + 0)
		{
			return items;
		}

		auto end() const->decltype(items + 0)
		{
			return items + nCount;
		}
	};

	virtual void OnLoaded(const IUILoadAttrs &attrs);
	virtual bool OnHitTest(UIHitTest &hitTest) { return false; }
	virtual bool OnMessage(UINT uMsg, WPARAM wParam, LPARAM lParam) { return false; }
	virtual void OnPaint(CUIDC &dc) const {}
	virtual void OnEnabled(bool bEnabled) {}
	virtual void OnRectChanged(LPCRECT lpOldRect, LPRECT lpClipRect) {}
	virtual void CalcRect(LPRECT lpRect, LPRECT lpClipRect);
	virtual void OnMouseEnter() {}
	virtual void OnMouseLeave() {}
	virtual bool DoMouseLeave(bool bForce);
	void DoPaint(CUIDC &dc) const;

	CUIView  *m_pParent;
	CRect     m_offset;		// �����ƫ�ƣ����һ��⡢���»���
	CSize     m_size;		// �����С����Ϊ����Ӧ
	CRect     m_rect;		// �������򣬰������ɼ�����
	bool      m_bControl;	// �Ƿ� CUIControl
	bool      m_bEnabled;
	bool      m_bVisible;
	bool      m_bKeepEnter;	// ���� MouseEnter ״̬

private:
	CRect     m_rcReal;		// ʵ�ʿɼ�����
	CUIBase **m_ppEnter;	// �ο� CUIRootView::DoMouseEnter

	CUIBase(const CUIBase &) = delete;
	CUIBase &operator=(const CUIBase &) = delete;
};

class UILIB_API CUIBlank : public CUIBase
{
public:
	CUIBlank(CUIView *pParent) : CUIBase(pParent) {}
};