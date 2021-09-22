#pragma once

#include "UIWnd.h"

// CUIAboutWnd

class CUIAboutWnd : public CUIWnd
{
	DECLARE_DYNAMIC(CUIAboutWnd)
public:
	CUIAboutWnd();
	virtual ~CUIAboutWnd();

	DECLARE_MESSAGE_MAP()
protected:
	virtual void OnLoadedUI(const IUILoadAttrs &loaded) override;
};
