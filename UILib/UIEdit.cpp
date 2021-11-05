#include "stdafx.h"
#include "UIEdit.h"

#ifndef ID_EDIT_COPY
#define ID_EDIT_DELETE			0xE120
#define ID_EDIT_COPY			0xE122
#define ID_EDIT_CUT				0xE123
#define ID_EDIT_PASTE			0xE125
#define ID_EDIT_SELECT_ALL		0xE12A
#define ID_EDIT_UNDO			0xE12B
#endif

CUIEdit::CUIEdit(CUIView *pParent) : CUIControl(pParent), m_hFont(GetDefaultFont()), m_color(0), m_nTabStop(1), m_chPassword(L'●'), m_bPassword(false), m_bReadonly(false)
	, m_bImeEnable(true), m_bFocus(false), m_nMaxLen(0), m_iStart(0), m_iEnd(0), m_nLeft(0), m_pUIMenu(NULL)
{
	CalcFontHeight();
	SetCursor(LoadCursor(NULL, IDC_IBEAM));
	m_undo.SetPosLen(-1);
}

CUIEdit::~CUIEdit()
{
	SetUIMenu(NULL);
}

void CUIEdit::SetFont(HFONT hFont)
{
	ATLASSERT(hFont);
	if (m_hFont != hFont && hFont)
	{
		m_hFont = hFont;
		CalcFontHeight();
		InvalidateRect();
	}
}

void CUIEdit::SetTextColor(COLORREF color)
{
	if (m_color != color)
	{
		m_color = color;
		InvalidateRect();
	}
}

void CUIEdit::SetText(LPCWSTR lpText)
{
	int nSize = (wcslen(lpText) + 1) * 2;
	LPWSTR szText = (LPWSTR)alloca(nSize);
	CopyMemory(szText, lpText, nSize);

	CheckMaxText(L"", szText);
	m_strText = szText;
	m_iStart = m_iEnd = m_nLeft = 0;
	InvalidateRect();

	m_undo.SetPosLen(-1);
	m_undo.m_strDiff.clear();

	OnTextChange();
	CalcCaretPos();
}

void CUIEdit::SetSel(int iStart, int iEnd)
{
	// 原选中区域
	CRect rect1;
	CalcSelRect(m_iStart, m_iEnd, rect1);

	// 重新选中
	m_iStart = min(m_strText.size(), (UINT)iStart);
	m_iEnd   = min(m_strText.size(), (UINT)iEnd);
	CalcCaretPos();

	if (!m_bFocus)
		return;

	// 现选中区域
	CRect rect2;
	CalcSelRect(m_iStart, m_iEnd, rect2);

	// 无效不重叠区域
	CRect rect;
	if (rect.IntersectRect(rect1, rect2))
	{
		std::swap(rect1.left, rect2.right);
		rect1.NormalizeRect();
		rect2.NormalizeRect();
	}

	InvalidateRect(rect1);
	InvalidateRect(rect2);
}

void CUIEdit::SetFocus()
{
	if (IsRealVisible() && IsRealEnabled())
		GetRootView()->SetFocus(this);
}

void CUIEdit::NextTabStop(bool bShift)
{
	// 找出所有可用的 Edit
	vector<CUIEdit *> vecEdits;
	vecEdits.reserve(4);

	function<void(CUIView *)> fnVisit = [&](CUIView *pView)
	{
		if (pView->IsRealVisible() && pView->IsEnabled())
		{
			CUIEdit *pEdit;
			if (FRIEND(pView)->IsPaint() == 2 && (pEdit = dynamic_cast<CUIEdit *>(pView)) && pEdit->m_nTabStop)
				vecEdits.push_back(pEdit);

			for (auto pItem : pView->GetChilds())
				fnVisit(pItem);
		}
	};

	fnVisit(GetRootView());

	int nCount = vecEdits.size();
	if (nCount < 2)
		return;

	// 根据 tabStop 稳定排序
	for (int i = 0; i < nCount - 1; i++)
	{
		for (int j = 0; j < nCount - 1 - i; j++)
		{
			if (vecEdits[j]->m_nTabStop > vecEdits[j + 1]->m_nTabStop)
				std::swap(vecEdits[j], vecEdits[j + 1]);
		}
	}

	// 切换到上/下一个 Edit
	for (int i = 0; i != nCount; i++)
	{
		if (vecEdits[i] == this)
		{
			auto pEdit = vecEdits[(bShift ? i - 1 + nCount : i + 1) % nCount];
			pEdit->SetSel(0, -1);
			GetRootView()->SetFocus(pEdit);
			break;
		}
	}
}

void CUIEdit::SetUIMenu(CUIMenu *pUIMenu)
{
	if (m_pUIMenu != pUIMenu)
	{
		if (m_pUIMenu)
			delete m_pUIMenu;

		m_pUIMenu = pUIMenu;
	}
}

bool CUIEdit::OnMessage(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg)
	{
	case WM_MOUSEMOVE:
		if (m_bLButtonDown)
		{
			OnMouseMove(lParam);
			return true;
		}
		break;

	case WM_LBUTTONDBLCLK:
		OnSelAll();
		return true;

	case WM_KEYDOWN:
		OnKeyDown(wParam);
		return true;

	case WM_CHAR:
		OnKeyChar(wParam);
		return true;
	}

	return __super::OnMessage(uMsg, wParam, lParam);
}

void CUIEdit::OnPaint(CUIDC &dc) const
{
	__super::OnPaint(dc);

	if (m_strText.empty())
		return;

	// 密码显示星号
	LPCWSTR lpText = m_strText.c_str();
	if (lpText[0] && m_bPassword)
	{
		int    nSize  = m_strText.size();
		LPWSTR lpData = (LPWSTR)alloca((nSize + 1) * 2);
		lpText = lpData;

		for (int i = 0; i != nSize; i++)
			lpData[i] = m_chPassword;

		lpData[nSize] = 0;
	}

	CRect rect(m_rect);
	rect.top = (rect.top + rect.bottom - m_nHeight) / 2;
	rect.bottom = rect.top + m_nHeight;
	rect.left -= m_nLeft;

	SelectObject(dc, m_hFont);
	::SetTextColor(dc, IsRealEnabled() ? m_color : GetSysColor(COLOR_GRAYTEXT));
	DrawTextW(dc, lpText, -1, rect, DT_NOPREFIX | DT_SINGLELINE);

	if (m_iStart != m_iEnd && m_bFocus)	// 有焦点时
	{
		CalcSelRect(m_iStart, m_iEnd, rect);
		dc.FillSolidRect(rect, GetSysColor(COLOR_HIGHLIGHT));
		::SetTextColor(dc, GetSysColor(COLOR_HIGHLIGHTTEXT));
		DrawTextW(dc, lpText + min(m_iStart, m_iEnd), abs(m_iStart - m_iEnd), rect, DT_NOPREFIX | DT_SINGLELINE);
	}

	// DrawText 后填充 alpha
	dc.FillAlpha(m_rect, 255);
}

void CUIEdit::OnEnable(bool bEnable)
{
	__super::OnEnable(bEnable);

	InvalidateRect();

	if (!bEnable)
	{
		auto pRootView = GetRootView();
		if (pRootView->GetFocus() == this)
			pRootView->SetFocus(NULL);
	}
}

void CUIEdit::OnVisible(bool bVisible)
{
	__super::OnVisible(bVisible);

	if (!bVisible)
	{
		auto pRootView = GetRootView();
		if (pRootView->GetFocus() == this)
			pRootView->SetFocus(NULL);
	}
}

void CUIEdit::OnMouseMove(CPoint point)
{
	int iEnd = m_iEnd;
	CalcClickPos(true, point);
	InvalidateSel(iEnd, m_iEnd);
	CalcCaretPos();
}

void CUIEdit::OnLButtonDown(CPoint point)
{
	bool bShift = GetKeyState(VK_SHIFT) < 0;
	GetRootView()->SetCapture(this);

	if (m_bFocus)
	{
		if (!bShift)
			InvalidateSel(m_iStart, m_iEnd);

		int iEnd = m_iEnd;
		CalcClickPos(bShift, point);

		if (bShift)
			InvalidateSel(iEnd, m_iEnd);

		CalcCaretPos();
	}
	else
	{
		CalcClickPos(bShift, point);
		GetRootView()->SetFocus(this);
	}
}

void CUIEdit::OnLButtonUp(CPoint point)
{
	GetRootView()->SetCapture(NULL);
}

void CUIEdit::OnRButtonDown(CPoint point)
{
	if (!m_bFocus)
		GetRootView()->SetFocus(this);
}

void CUIEdit::OnRButtonUp(CPoint point)
{
	if (m_pUIMenu == NULL)
	{
		m_pUIMenu = new CUIMenu;
		m_pUIMenu->GetItem(ID_EDIT_UNDO)->m_strText = L"撤销(&U)";
		m_pUIMenu->GetItem();
		m_pUIMenu->GetItem(ID_EDIT_CUT)->m_strText = L"剪切(&T)";
		m_pUIMenu->GetItem(ID_EDIT_COPY)->m_strText = L"复制(&C)";
		m_pUIMenu->GetItem(ID_EDIT_PASTE)->m_strText = L"粘贴(&P)";
		m_pUIMenu->GetItem(ID_EDIT_DELETE)->m_strText = L"删除(&D)";
		m_pUIMenu->GetItem();
		m_pUIMenu->GetItem(ID_EDIT_SELECT_ALL)->m_strText = L"全选(&A)";
	}

	m_pUIMenu->GetItem(ID_EDIT_UNDO)->m_bEnable = (m_undo.m_nLen || m_undo.m_strDiff.size()) && !m_bReadonly;
	m_pUIMenu->GetItem(ID_EDIT_CUT)->m_bEnable = m_iStart != m_iEnd && !m_bPassword && !m_bReadonly;
	m_pUIMenu->GetItem(ID_EDIT_COPY)->m_bEnable = m_iStart != m_iEnd && !m_bPassword;
	m_pUIMenu->GetItem(ID_EDIT_PASTE)->m_bEnable = !m_bReadonly && IsClipboardFormatAvailable(CF_UNICODETEXT);
	m_pUIMenu->GetItem(ID_EDIT_DELETE)->m_bEnable = m_iStart != m_iEnd && !m_bReadonly;
	m_pUIMenu->GetItem(ID_EDIT_SELECT_ALL)->m_bEnable = abs(m_iStart - m_iEnd) != m_strText.size();

	GetCursorPos(&point);
	UINT nCmdId = GetRootView()->PopupUIMenu(m_pUIMenu, point.x, point.y, MAXINT16, point.y);

	switch (nCmdId)
	{
	case ID_EDIT_UNDO:
		OnUndo();
		break;

	case ID_EDIT_CUT:
		OnCut();
		break;

	case ID_EDIT_COPY:
		OnCopy();
		break;

	case ID_EDIT_PASTE:
		OnPaste();
		break;

	case ID_EDIT_DELETE:
		DelStartEnd(m_iStart, m_iEnd);
		break;

	case ID_EDIT_SELECT_ALL:
		OnSelAll();
		break;
	}
}

void CUIEdit::OnSetFocus()
{
	if (m_bFocus)
		return;

	m_bFocus = true;
	InvalidateSel(m_iStart, m_iEnd);
	CalcCaretPos();
}

void CUIEdit::OnKillFocus()
{
	if (!m_bFocus)
		return;

	m_bFocus = false;
	InvalidateSel(m_iStart, m_iEnd);
	GetRootView()->SetCaretPos(CRect());
}

void CUIEdit::CalcFontHeight()
{
	TEXTMETRIC tm;
	GetTextMetrics(CUIComDC(m_hFont), &tm);
	m_nHeight = tm.tmHeight;
}

void CUIEdit::CalcClickPos(bool bShift, CPoint point)
{
	if ((m_iEnd = m_strText.size()) == 0)
	{
		m_iStart = 0;
		return;
	}

	CUIComDC dc(m_hFont);

	if (m_bPassword)
	{
		CRect rect;
		DrawTextW(dc, (LPCWSTR)&m_chPassword, 1, rect, DT_NOPREFIX | DT_SINGLELINE | DT_CALCRECT);

		int nWidth = point.x - m_rect.left + m_nLeft;
		int nCount = max(nWidth, 0) / rect.Width();

		if (nCount < m_iEnd)
			m_iEnd = nWidth % rect.Width() * 2 < rect.Width() ? nCount : nCount + 1;

		if (!bShift)
			m_iStart = m_iEnd;
		return;
	}

	// 二分法查找
	int nWidth = point.x - m_rect.left + m_nLeft;
	int nWidthL = 0, nCountL = 0, nCountR = m_iEnd;

	for (;;)
	{
		int nCountM = (nCountL + nCountR + 1) / 2;

		CRect rect;
		DrawTextW(dc, m_strText.c_str() + nCountL, nCountM - nCountL, rect, DT_NOPREFIX | DT_SINGLELINE | DT_CALCRECT);
		int nWidthM = nWidthL + rect.Width();

		if (nCountM == nCountR)
		{
			m_iEnd = nWidth * 2 < nWidthL + nWidthM ? nCountL : nCountR;
			break;
		}

		if (nWidthM == nWidth)
		{
			m_iEnd = nCountM;
			break;
		}

		if (nWidthM < nWidth)
		{
			nCountL = nCountM;
			nWidthL = nWidthM;
		}
		else
			nCountR = nCountM;
	}

	if (!bShift)
		m_iStart = m_iEnd;
}

int CUIEdit::CalcPosLeft(int iPos) const
{
	if (iPos == 0)
		return 0;

	CUIComDC dc(m_hFont);

	if (m_bPassword)
	{
		CRect rect;
		DrawTextW(dc, (LPCWSTR)&m_chPassword, 1, rect, DT_NOPREFIX | DT_SINGLELINE | DT_CALCRECT);
		return rect.Width() * (iPos > 0 ? iPos : m_strText.size());
	}

	CRect rect;
	DrawTextW(dc, m_strText.c_str(), iPos, rect, DT_NOPREFIX | DT_SINGLELINE | DT_CALCRECT);
	return rect.Width();
}

void CUIEdit::CalcSelRect(int iStart, int iEnd, LPRECT lpRect) const
{
	if (iStart == iEnd)
	{
		SetRectEmpty(lpRect);
		return;
	}

	CRect rect(CalcPosLeft(iStart), 0, CalcPosLeft(iEnd), m_nHeight);
	rect.NormalizeRect();
	rect.OffsetRect(m_rect.left - m_nLeft, (m_rect.top + m_rect.bottom - m_nHeight) / 2);
	*lpRect = rect;
}

void CUIEdit::InvalidateSel(int iStart, int iEnd)
{
	CRect rect;
	CalcSelRect(iStart, iEnd, rect);
	InvalidateRect(rect);
}

void CUIEdit::OnKeyDown(UINT nChar)
{
	switch (nChar)
	{
	case VK_TAB:
		NextTabStop(GetKeyState(VK_SHIFT) < 0);
		break;

	case VK_LEFT:
		OnKeyMove(-1);
		break;

	case VK_RIGHT:
		OnKeyMove(1);
		break;

	case VK_HOME:
		OnKeyMove(-2);
		break;

	case VK_END:
		OnKeyMove(2);
		break;

	case VK_BACK:
		if (m_iStart != m_iEnd)
		{
			DelStartEnd(m_iStart, m_iEnd);
		}
		else if (m_iEnd)
		{
			DelStartEnd(m_iEnd - 1, m_iEnd);
		}
		break;

	case VK_DELETE:
		if (m_iStart != m_iEnd)
		{
			DelStartEnd(m_iStart, m_iEnd);
		}
		else if (m_iEnd != m_strText.size())
		{
			DelStartEnd(m_iEnd, m_iEnd + 1);
		}
		break;

	default:
		if (GetKeyState(VK_CONTROL) < 0 && GetKeyState(VK_MENU) >= 0)
		{
			switch (nChar)
			{
			case 'Z':
				OnUndo();
				break;

			case 'X':
				OnCut();
				break;

			case 'C':
				OnCopy();
				break;

			case 'V':
				OnPaste();
				break;

			case 'A':
				OnSelAll();
				break;
			}
		}
	}
}

void CUIEdit::OnKeyMove(int nMove)
{
	int nSize = m_strText.size();
	if (nSize == 0)
		return;

	int iEnd = m_iEnd;

	if (nMove < 0 && m_iEnd)
	{
		if (nMove == -1)
			iEnd--;
		else
			iEnd = 0;
	}

	if (nMove > 0 && m_iEnd != nSize)
	{
		if (nMove == 1)
			iEnd++;
		else
			iEnd = nSize;
	}

	if (GetKeyState(VK_SHIFT) < 0)
	{
		InvalidateSel(iEnd, m_iEnd);
		m_iEnd = iEnd;
	}
	else
	{
		InvalidateSel(m_iStart, m_iEnd);
		m_iStart = m_iEnd = iEnd;
	}

	CalcCaretPos();
}

void CUIEdit::DelStartEnd(int iStart, int iEnd, bool bChange)
{
	if (iStart == iEnd || m_bReadonly)
		return;

	if (iStart > iEnd)
		std::swap(iStart, iEnd);

	wstring strDiff = m_strText.substr(iStart, iEnd - iStart);
	if (m_undo.m_nPos == iStart && m_undo.m_nLen == 0 && iEnd <= (int)m_strText.size())
	{
		m_undo.m_strDiff.append(strDiff);
	}
	else if (m_undo.m_nPos == iEnd)
	{
		m_undo.SetPosLen(iStart);
		m_undo.m_strDiff.insert(0, strDiff);
	}
	else
	{
		m_undo.SetPosLen(iStart);
		m_undo.m_strDiff = strDiff;
	}

	InvalidateSel(iStart, -1);
	m_strText.erase(iStart, iEnd - iStart);
	m_iStart = m_iEnd = iStart;

	if (bChange)
	{
		OnTextChange();
		CalcCaretPos();
	}
}

void CUIEdit::OnKeyChar(UINT nChar)
{
	if (nChar < ' ')
		return;

#ifndef _UNICODE
	if (nChar > 127)
	{
		int nTemp = (nChar << 8) | (nChar >> 8);
		MultiByteToWideChar(CP_ACP, 0, (LPCSTR)&nTemp, 2, (LPWSTR)&nChar, 1);
	}
#endif

	Insert((LPWSTR)&nChar);
}

void CUIEdit::OnUndo()
{
	if (m_undo.m_nLen == 0 && m_undo.m_strDiff.empty() || m_bReadonly)
		return;

	InvalidateSel(m_iStart, m_iEnd);
	m_iStart = m_undo.m_nPos;
	wstring strDiff = m_strText.substr(m_iStart, m_undo.m_nLen);

	InvalidateSel(m_iStart, -1);
	m_strText.erase(m_iStart, m_undo.m_nLen);
	m_strText.insert(m_iStart, m_undo.m_strDiff);
	InvalidateSel(m_iStart, -1);

	m_iEnd = m_iStart + (m_undo.m_nLen = m_undo.m_strDiff.size());
	m_undo.m_strDiff = strDiff;

	OnTextChange();
	CalcCaretPos();
}

void CUIEdit::OnCut()
{
	if (m_iStart == m_iEnd || m_bPassword || m_bReadonly)
		return;

	OnCopy();
	DelStartEnd(m_iStart, m_iEnd);
}

void CUIEdit::OnCopy()
{
	if (m_iStart == m_iEnd || m_bPassword || !OpenClipboard(NULL))
		return;

	int iStart = min(m_iStart, m_iEnd);
	int iEnd   = m_iStart ^ m_iEnd ^ iStart;
	int nSize  = iEnd - iStart;

	HGLOBAL	hGlobal = GlobalAlloc(GMEM_MOVEABLE, (nSize + 1) * 2);
	wcsncpy_s((LPWSTR)GlobalLock(hGlobal), nSize + 1, m_strText.c_str() + iStart, nSize);
	GlobalUnlock(hGlobal);

	EmptyClipboard();
	SetClipboardData(CF_UNICODETEXT, hGlobal);
	CloseClipboard();
}

void CUIEdit::OnPaste()
{
	if (m_bReadonly || !OpenClipboard(NULL))
		return;

	if (HANDLE hGlobal = GetClipboardData(CF_UNICODETEXT))
	{
		LPWSTR lpSrc = (LPWSTR)GlobalLock(hGlobal);
		LPWSTR lpDst = (LPWSTR)alloca((wcslen(lpSrc) + 1) * 2), lp = lpDst;

		while (int ch = *lpSrc++)
		{
			if (ch >= ' ' && (!m_bPassword || ch <= 127))
				*lp++ = ch;
		}

		*lp = 0;
		GlobalUnlock(hGlobal);
		CloseClipboard();
		Insert(lpDst);
	}
	else
		CloseClipboard();
}

void CUIEdit::OnSelAll()
{
	if (m_iStart > m_iEnd)
		std::swap(m_iStart, m_iEnd);

	InvalidateSel(0, m_iStart);
	InvalidateSel(m_iEnd, -1);

	m_iStart = 0;
	m_iEnd   = m_strText.size();
	CalcCaretPos();
}

void CUIEdit::Insert(LPWSTR lpText)
{
	if (m_bReadonly)
		return;

	if (m_fnFilter && lpText[0])
	{
		m_fnFilter(lpText);

		if (lpText[0] == 0)
			return;
	}

	if (m_iStart != m_iEnd)
		DelStartEnd(m_iStart, m_iEnd, false);

	if (m_undo.m_nPos + m_undo.m_nLen != m_iEnd)
	{
		m_undo.SetPosLen(m_iEnd);
		m_undo.m_strDiff.clear();
	}

	CheckMaxText(m_strText.c_str(), lpText);
	m_strText.insert(m_iEnd, lpText);
	InvalidateSel(m_iEnd, -1);
	m_iStart = m_iEnd += wcslen(lpText);
	m_undo.m_nLen = m_iEnd - m_undo.m_nPos;

	OnTextChange();
	CalcCaretPos();
}

void CUIEdit::CalcCaretPos()
{
	if (!m_bFocus || m_rect.IsRectEmpty())
		return;

	CPoint point(m_rect.left + CalcPosLeft(m_iEnd) - m_nLeft, (m_rect.top + m_rect.bottom - m_nHeight) / 2);

	if (point.x < m_rect.left)
	{
		// 光标超出左边界，整体右移
		while (point.x < m_rect.left)
		{
			int nSize = m_rect.Width() / 4;
			point.x += nSize;
			m_nLeft -= nSize;
		}

		InvalidateRect();
	}
	else if (point.x >= m_rect.right)
	{
		// 光标超出右边界，整体左移
		while (point.x >= m_rect.right)
		{
			int nSize = m_rect.Width() / 4;
			point.x -= nSize;
			m_nLeft += nSize;
		}

		InvalidateRect();
	}

	if (m_nLeft > 0)
	{
		// 移除文字右空白
		int nMore = m_rect.Width() + m_nLeft - CalcPosLeft(-1) - 1;
		if (nMore > 0)
		{
			point.x += nMore;
			m_nLeft -= nMore;
			InvalidateRect();
		}
	}

	if (m_nLeft < 0)
	{
		// 移除文字左空白
		point.x += m_nLeft;
		m_nLeft = 0;
		InvalidateRect();
	}

	bool bCaret = point.x >= m_rcReal.left && point.x < m_rcReal.right;
	GetRootView()->SetCaretPos(CRect(point, CSize(bCaret, m_nHeight)));
}

void CUIEdit::CheckMaxText(LPCWSTR lpExist, LPWSTR lpText) const
{
	int nMaxLen = (WORD)m_nMaxLen;
	if (nMaxLen == 0)
		return;

	// 字符数
	if (m_nMaxLen >> 16 == 0)
	{
		int nSize = nMaxLen - wcslen(lpExist);
		if (nSize < (int)wcslen(lpText))
			lpText[max(nSize, 0)] = 0;
		return;
	}

	// 字节数
	while (WORD ch = *lpExist++)
	{
		if (ch > 127)
			nMaxLen -= 2;
		else
			nMaxLen--;
	}

	while (WORD ch = *lpText)
	{
		if (ch > 127)
			nMaxLen -= 2;
		else
			nMaxLen--;

		if (nMaxLen < 0)
			*lpText = 0;
		else
			lpText++;
	}
}

void CUIEdit::OnLoad(const IUIXmlAttrs &attrs)
{
	__super::OnLoad(attrs);

	LPCWSTR lpStr;
	if (lpStr = attrs.GetStr(L"font"))
		SetFont(attrs.GetFont(lpStr));

	if (lpStr = attrs.GetStr(L"color"))
	{
		COLORREF color = 0;
		ATLVERIFY(StrToColor(lpStr, color));
		SetTextColor(color);
	}

	int nValue;
	if (attrs.GetInt(L"tabStop", &nValue))
		SetTabStop(nValue);

	if (attrs.GetInt(L"maxBytes", &nValue))
		SetMaxBytes(nValue);
	else if (attrs.GetInt(L"maxChars", &nValue))
		SetMaxChars(nValue);

	if (attrs.GetInt(L"password", &nValue) && nValue)
		SetPassword(true);

	if (attrs.GetInt(L"readonly", &nValue) && nValue)
		SetReadonly(true);

	if (attrs.GetInt(L"imeEnabled", &nValue) && nValue == 0)
		SetImeEnabled(false);

	if (lpStr = attrs.GetStr(L"text"))
		SetText(lpStr);
}
