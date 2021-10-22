#pragma once

class CDemoWnd2 : public CUIWindow
{
public:
	CDemoWnd2();
	~CDemoWnd2();

private:
	virtual void OnLoadUI(const IUIXmlAttrs &attrs) override;
	virtual void OnCreate() override;
	virtual void OnGetMinMaxInfo(MINMAXINFO *lpMMI) override;
	virtual void OnSize(UINT nType, CSize size);
};
