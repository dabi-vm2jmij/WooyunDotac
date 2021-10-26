#pragma once

class UILIB_API CUIMenu
{
	friend class CUIMenuWnd;
public:
	CUIMenu();
	virtual ~CUIMenu();

	struct Item;

	void CreateFromMenu(HMENU hMenu);
	Item *GetItem(UINT nId = -1);
	UINT GetCount() const { return m_vecItems.size(); }
	UINT Popup(HWND hParent, int x1, int y1, int x2 = MAXINT16, int y2 = MAXINT16);		// x2��y2���˵�������Ļ���µ��ҡ��±߽�

protected:
	virtual CUIMenu *NewUIMenu() const { return new CUIMenu; }
	virtual bool IsDropShadow() const { return true; }
	virtual void GetMargins(LPRECT lpRect) const;
	virtual int  GetSpacing() const { return 0; }	// �Ӳ˵���ˮƽ���
	virtual BYTE GetWndAlpha() const { return 0; }	// 0����͸����1-255��͸����
	virtual UINT GetShowDelay() const;
	virtual void MeasureItem(UINT nIndex, LPSIZE lpSize) const;
	virtual void DrawBg(CUIDC &dc, LPCRECT lpRect);
	virtual void DrawItem(CUIDC &dc, LPCRECT lpRect, UINT nIndex, bool bSelected);

	vector<Item> m_vecItems;
};

struct CUIMenu::Item
{
	UINT    m_nId;
	wstring m_strText;
	CImagex m_imagex;
	bool    m_bCheck;
	bool    m_bEnable;
	int     m_nHeight;

	Item(UINT nId = 0, LPCWSTR lpText = NULL);
	void SetSubMenu(CUIMenu *pUIMenu);
	CUIMenu *GetSubMenu() const { return m_pSubMenu; }

private:
	CUIMenu *m_pSubMenu;
};
