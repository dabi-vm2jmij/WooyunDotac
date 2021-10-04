#include "stdafx.h"
#include "UITimer.h"

CUITimer::CUITimer(function<void()> &&fnOnTimer) : m_fnOnTimer(std::move(fnOnTimer)), m_pThunk(NULL), m_nTimerId(0)
{
}

CUITimer::CUITimer(CUITimer &&_Right) : m_pThunk(_Right.m_pThunk), m_nTimerId(_Right.m_nTimerId), m_fnOnTimer(std::move(_Right.m_fnOnTimer))
{
	_Right.m_pThunk = NULL;
	_Right.m_nTimerId = 0;
}

CUITimer::~CUITimer()
{
	Stop();
}

bool CUITimer::Start(UINT nElapse)
{
	if (m_pThunk == NULL && (m_pThunk = new _stdcallthunk) == NULL)
		return false;

	auto pfProc = &CUITimer::MyProc;
	m_pThunk->Init((DWORD &)pfProc, this);

	if (m_nTimerId = SetTimer(NULL, m_nTimerId, nElapse, (TIMERPROC)m_pThunk->GetCodeAddress()))
		return true;

	Stop();
	return false;
}

void CUITimer::Stop()
{
	if (m_nTimerId)
	{
		KillTimer(NULL, m_nTimerId);
		m_nTimerId = 0;
	}

	if (m_pThunk)
	{
		delete m_pThunk;
		m_pThunk = NULL;
	}
}

void CUITimer::MyProc(UINT, UINT_PTR, DWORD)
{
	m_fnOnTimer();
}
