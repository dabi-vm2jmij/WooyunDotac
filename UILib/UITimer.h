#pragma once

class UILIB_API CUITimer
{
public:
	CUITimer(std::function<void()> &&fnOnTimer);
	CUITimer(CUITimer &&_Right);
	~CUITimer();

	bool Set(UINT nElapse);
	void Kill();

private:
	void CALLBACK MyProc(UINT, UINT_PTR, DWORD);

	_stdcallthunk *m_pThunk;
	UINT_PTR       m_nTimerId;
	std::function<void()> m_fnOnTimer;

	CUITimer(const CUITimer &) = delete;
	CUITimer &operator=(const CUITimer &) = delete;
};
