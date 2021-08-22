#pragma once

// 快速 IStream，可以直接从数据得到一个 IStream，而不用 GlobalAlloc 申请+拷贝内存
// 还可以直接在栈上得到一个 IStream，比如：&CUIStream(lpData, dwSize)

class UILIB_API CUIStream : public IStream
{
public:
	CUIStream(LPCVOID lpData, DWORD dwSize);
	virtual ~CUIStream();

	static CUIStream *FromData(LPCVOID lpData, DWORD dwSize, bool bNeedFree = false);
	static CUIStream *FromFile(LPCWSTR lpFileName);

	LPCSTR Data() const { return m_lpData; }
	DWORD  Size() const { return m_dwSize; }

	HRESULT STDMETHODCALLTYPE QueryInterface(REFIID riid, void **ppvObject);
	ULONG   STDMETHODCALLTYPE AddRef();
	ULONG   STDMETHODCALLTYPE Release();
	HRESULT STDMETHODCALLTYPE Read(void *pv, ULONG cb, ULONG *pcbRead);
	HRESULT STDMETHODCALLTYPE Write(const void *pv, ULONG cb, ULONG *pcbWritten);
	HRESULT STDMETHODCALLTYPE Seek(LARGE_INTEGER dlibMove, DWORD dwOrigin, ULARGE_INTEGER *plibNewPosition);
	HRESULT STDMETHODCALLTYPE SetSize(ULARGE_INTEGER libNewSize);
	HRESULT STDMETHODCALLTYPE CopyTo(IStream *pstm, ULARGE_INTEGER cb, ULARGE_INTEGER *pcbRead, ULARGE_INTEGER *pcbWritten);
	HRESULT STDMETHODCALLTYPE Commit(DWORD grfCommitFlags);
	HRESULT STDMETHODCALLTYPE Revert();
	HRESULT STDMETHODCALLTYPE LockRegion(ULARGE_INTEGER libOffset, ULARGE_INTEGER cb, DWORD dwLockType);
	HRESULT STDMETHODCALLTYPE UnlockRegion(ULARGE_INTEGER libOffset, ULARGE_INTEGER cb, DWORD dwLockType);
	HRESULT STDMETHODCALLTYPE Stat(STATSTG *pstatstg, DWORD grfStatFlag);
	HRESULT STDMETHODCALLTYPE Clone(IStream **ppstm);

private:
	bool  Reserve(DWORD dwCapacity);

	void *operator new(size_t);
	void  operator delete(void *);

	ULONG m_nRefCount;
	DWORD m_dwCurPos;
	DWORD m_dwCapacity;
	DWORD m_dwSize;
	LPSTR m_lpData;
	UINT  m_nSkinId;

	CUIStream(const CUIStream &) = delete;
	CUIStream &operator=(const CUIStream &) = delete;
};
