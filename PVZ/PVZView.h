#pragma once

#include "PVZDoc.h"

class PVZView : public CView {
protected:
	DECLARE_DYNCREATE(PVZView)

public:
	virtual void OnDraw(CDC* pDC);

protected:
	// 新增：绘制鼠标坐标函数
	void DrawMouseCoordinates(CDC* cDC);

protected:
	DECLARE_MESSAGE_MAP()

public:
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnSelect(UINT nFlags, CPoint point);
	afx_msg void OnPlace(UINT nFlags, CPoint point);
};
