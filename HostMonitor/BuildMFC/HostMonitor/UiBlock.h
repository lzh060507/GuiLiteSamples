
#pragma once

extern void* getUiOfHostMonitor(int display_id, int* width, int* height, bool force_update = false);
extern void sendTouch2HostMonitor(int x, int y, bool is_down, int display_id);

typedef struct tagMYBITMAPINFO {
	BITMAPINFOHEADER    bmiHeader;
	DWORD				biRedMask;
	DWORD				biGreenMask;
	DWORD				biBlueMask;
} MYBITMAPINFO;

class CUiBlock
{
public:
	CUiBlock(int index, int color_bytes);
	void renderUI(CRect& rect, CDC* pDC);

	void OnLButtonDown(UINT nFlags, CPoint point);
	void OnLButtonUp(UINT nFlags, CPoint point);
	void OnMouseMove(UINT nFlags, CPoint point);
private:
	CPoint pointMFC2GuiLite(CPoint point);

	int m_index;
	int m_color_bytes;
	CRect m_block_rect;
	int m_ui_width;
	int m_ui_height;
	
	MYBITMAPINFO m_ui_bm_info;
	CDC			m_memDC;
	CBitmap		m_blockBmp;

	bool m_is_mouse_down;
};
