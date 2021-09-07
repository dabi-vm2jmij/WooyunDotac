#pragma once

class IUILoadAttrs
{
private:
	class CUIViewPtr
	{
	public:
		CUIViewPtr(CUIView *pView) : m_pView(pView) {}

		template<typename T>
		operator T *() const
		{
			T *pItem = dynamic_cast<T *>(m_pView);
			ATLASSERT(pItem);
			return pItem;
		}

	private:
		CUIView *m_pView;
	};

public:
	virtual LPCWSTR  GetStr(LPCWSTR lpName) const = 0;
	virtual bool     GetInt(LPCWSTR lpName, int *pnValue) const = 0;
	virtual HFONT    GetFont(LPCWSTR lpszId) const = 0;
	virtual CUIView *GetView(LPCWSTR lpszId) const = 0;

	int GetInt(LPCWSTR lpName, int nDefault = 0) const
	{
		int nValue;
		return GetInt(lpName, &nValue) ? nValue : nDefault;
	}

	CUIViewPtr operator[](LPCWSTR lpszId) const
	{
		return GetView(lpszId);
	}
};
