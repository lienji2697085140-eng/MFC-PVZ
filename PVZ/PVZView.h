#pragma once

#include "PVZDoc.h"
#include "PVZWinApp.h"  // 添加这行，包含游戏状态

class PVZView : public CView {
protected:
	DECLARE_DYNCREATE(PVZView)

public:
	virtual void OnDraw(CDC* pDC);

protected:
	// 新增：绘制鼠标坐标函数
	void DrawMouseCoordinates(CDC* cDC);
	// 新增：绘制游戏失败画面
	void DrawGameOverScreen(CDC* cDC);

protected:
	DECLARE_MESSAGE_MAP()

public:
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnSelect(UINT nFlags, CPoint point);
	afx_msg void OnPlace(UINT nFlags, CPoint point);
};