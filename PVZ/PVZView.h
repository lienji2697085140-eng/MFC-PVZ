#pragma once
#include"PVZDoc.h"
#include"PVZWinApp.h"

class PVZView : public CView {
protected:
    DECLARE_DYNCREATE(PVZView)

public:
    virtual void OnDraw(CDC* pDC);

protected:
    //绘制游戏失败画面
    void DrawGameOverScreen(CDC* cDC);
    //绘制重新开始按钮
    void DrawRestartButton(CDC* cDC);
    //检查是否点击了重新开始按钮
    bool IsClickRestartButton(CPoint point);
    //在鼠标位置显示坐标
    void DrawMouseCoordinates(CDC* cDC);
    //显示分数
    void DrawScore(CDC* cDC);
    //新增:绘制分数弹出效果
    void DrawScorePopups(CDC* cDC);

protected:
    DECLARE_MESSAGE_MAP()

public:
    afx_msg void OnMouseMove(UINT nFlags, CPoint point);
    afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
    afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
    afx_msg void OnSelect(UINT nFlags, CPoint point);
    afx_msg void OnPlace(UINT nFlags, CPoint point);

private:
    bool m_bTrackingCoordinates = false;  // 修改：跟踪坐标状态
    CPoint m_lastMousePos;                // 新增：记录最后鼠标位置
};