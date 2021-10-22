#pragma once

// ��������������๤�ߡ���������

class CUIMoreTools : public CUIWindow
{
public:
	DECLARE_WND_CLASS_EX(NULL, CS_DBLCLKS | CS_DROPSHADOW, -1)

	CUIMoreTools();
	~CUIMoreTools();

	HWND Popup(const vector<CUIView *> &vecItems, HWND hParent, CPoint point);

private:
	virtual void OnKillFocus(HWND hNewWnd) override;
	virtual void OnDestroy() override;

	CUIView *m_pToolBar;
};
