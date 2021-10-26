#pragma once

class UILIB_API CUITimer
{
public:
	CUITimer(function<void()> &&fnOnTimer);
	CUITimer(CUITimer &&_Right);
	~CUITimer();

	bool Start(UINT nElapse);
	void Stop();
	bool Running() const { return m_nTimerId != 0; }

private:
	void CALLBACK MyProc(UINT, UINT_PTR, DWORD);

	_stdcallthunk *m_pThunk;
	UINT_PTR       m_nTimerId;
	function<void()> m_fnOnTimer;

	CUITimer(const CUITimer &) = delete;
	CUITimer &operator=(const CUITimer &) = delete;
};
