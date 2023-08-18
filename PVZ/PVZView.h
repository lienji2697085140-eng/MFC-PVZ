#pragma once
//#include <afxwin.h>
//#include "PVZDoc.h"
#include <afxwin.h>

class PVZView : public CView {
    DECLARE_DYNCREATE(PVZView)
    DECLARE_MESSAGE_MAP()
public:
    // ÷ÿ‘ÿOnDraw
    virtual void OnDraw(CDC*);
    afx_msg void OnMouseMove(UINT nFlags, CPoint point);
    afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
    void OnSelect(UINT nFlags, CPoint point);
    void OnPlace(UINT nFlags, CPoint point);
};

