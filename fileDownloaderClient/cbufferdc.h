
#ifndef C_BUFFER_DC_H__
#define C_BUFFER_DC_H__

class CBufferDC :
	public CPaintDC
{
	DECLARE_DYNAMIC(CBufferDC)

private:
	HDC m_hOutputDC;
	HDC m_hAttributeDC;
	HDC m_hMemoryDC;

	HBITMAP  m_hPaintBitmap;
	HBITMAP  m_hOldBitmap;

	RECT m_ClientRect;

	BOOL m_bBoundsUpdated;

public:
	CBufferDC(CWnd *pWnd);
	~CBufferDC();

private:
	void Flush();

public:
	UINT SetBoundsRect(LPCRECT lpRectBounds, UINT flags);
	virtual BOOL RestoreDC(int nSavedDC);
};

#endif

