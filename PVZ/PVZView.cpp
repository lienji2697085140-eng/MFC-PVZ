#include"stdafx.h"
#include"PVZView.h"
#include"Config.h"
#include"PVZDoc.h"
#include"Plant.h"
#include"Sun.h"
#include"Yard.h"
#include"Zombie.h"
#include"resource.h"

extern PVZDoc* theDoc;

IMPLEMENT_DYNCREATE(PVZView, CView)

BEGIN_MESSAGE_MAP(PVZView, CView)
    ON_WM_MOUSEMOVE()
    ON_WM_LBUTTONDOWN()
    ON_WM_RBUTTONDOWN()
    ON_WM_KEYDOWN()
END_MESSAGE_MAP()

void PVZView::OnDraw(CDC* pDC) {
    //如果游戏失败,显示全屏黑屏和游戏失败文字
    if (PVZWinApp::gameOver) {
        DrawGameOverScreen(pDC);
        return;
    }

    //如果游戏暂停，显示暂停画面
    if (PVZWinApp::gamePaused && !PVZWinApp::gameOver) {
        DrawPauseScreen(pDC);
        return;
    }

    //正常的游戏绘制代码
    Yard& yard = theDoc->getYard();
    SeedBank& sbank = theDoc->getSeedBank();
    Player& player = theDoc->getPlayer();
    PVZDoc::SunlightList& sunList = theDoc->getSunList();

    //准备双缓冲所需的数据
    CDC memDC;
    CBitmap buf;
    CFont font;
    memDC.CreateCompatibleDC(pDC);
    font.CreatePointFont(170, _T("微软雅黑"));
    CRect rect;
    GetClientRect(&rect);
    buf.CreateCompatibleBitmap(pDC, rect.Width(), rect.Height());
    memDC.SelectObject(&buf);
    memDC.SelectObject(&font);

    //绘制白色背景
    memDC.FillSolidRect(rect, RGB(255, 255, 255));

    //绘制院子上的元素
    yard.draw(memDC.m_hDC);

    //绘制植物选项卡
    sbank.draw(memDC.m_hDC);

    //绘制植物 - 修复绘图调用
    auto& plantMatrix = yard.getPlantMatrix();
    for (int row = 0; row < 5; ++row) {
        for (int col = 0; col < 9; ++col) {
            if (plantMatrix[row][col]) {
                plantMatrix[row][col]->draw(memDC.m_hDC);
            }
        }
    }

    //绘制僵尸
    auto& zombieList = yard.getZombieList();
    for (int row = 0; row < 5; ++row) {
        for (auto& zombie : zombieList[row]) {
            if (zombie) zombie->draw(memDC.m_hDC);
        }
    }

    //绘制投射物
    auto& ejectList = yard.getEjectList();
    for (int row = 0; row < 5; ++row) {
        for (auto& eject : ejectList[row]) {
            if (eject) eject->draw(memDC.m_hDC);
        }
    }

    //绘制选中的植物
    player.drawCurrentPlant(memDC.m_hDC, yard);

    //绘制太阳
    for (auto& sun : sunList) {
        sun->draw(memDC.m_hDC);
    }

    //结束双缓冲绘制
    pDC->BitBlt(0, 0, rect.Width(), rect.Height(), &memDC, 0, 0, SRCCOPY);

    //显示分数
    DrawScore(pDC);

    //新增:绘制分数弹出效果
    DrawScorePopups(pDC);

    //在鼠标位置显示坐标（根据跟踪状态决定是否显示）
    if (m_bTrackingCoordinates) {
        DrawMouseCoordinates(pDC);
    }

    //新增:绘制铲子按钮
    DrawShovelButton(pDC);

    buf.DeleteObject();
    memDC.DeleteDC();
    font.DeleteObject();
}

void PVZView::DrawShovelButton(CDC* cDC) {
    CRect rect;
    GetClientRect(&rect);

    int shovelButtonWidth = 80;
    int shovelButtonHeight = 40;
    int shovelButtonX = rect.Width() - shovelButtonWidth - 20;
    int shovelButtonY = 10;

    //绘制铲子按钮背景
    CRect shovelRect(shovelButtonX, shovelButtonY,
        shovelButtonX + shovelButtonWidth,
        shovelButtonY + shovelButtonHeight);

    //根据铲子模式状态设置不同颜色
    if (m_bShovelMode) {
        cDC->FillSolidRect(shovelRect, RGB(255, 200, 200));  // 铲子模式时红色
    }
    else {
        cDC->FillSolidRect(shovelRect, RGB(200, 200, 200));  // 正常模式时灰色
    }

    //绘制按钮边框
    CPen pen(PS_SOLID, 2, RGB(100, 100, 100));
    CPen* oldPen = cDC->SelectObject(&pen);
    cDC->Rectangle(shovelRect);
    cDC->SelectObject(oldPen);

    //创建按钮字体
    CFont font;
    font.CreatePointFont(100, _T("微软雅黑"));
    CFont* oldFont = cDC->SelectObject(&font);
    COLORREF oldColor = cDC->SetTextColor(RGB(0, 0, 0));
    int oldBkMode = cDC->SetBkMode(TRANSPARENT);

    //绘制"铲子"文字
    CString shovelText = _T("铲子");
    CSize textSize = cDC->GetTextExtent(shovelText);
    int textX = shovelButtonX + (shovelButtonWidth - textSize.cx) / 2;
    int textY = shovelButtonY + (shovelButtonHeight - textSize.cy) / 2;
    cDC->TextOut(textX, textY, shovelText);

    //恢复原来的设置
    cDC->SelectObject(oldFont);
    cDC->SetTextColor(oldColor);
    cDC->SetBkMode(oldBkMode);
    font.DeleteObject();
}

bool PVZView::IsClickShovelButton(CPoint point) {
    CRect rect;
    GetClientRect(&rect);

    int shovelButtonWidth = 80;
    int shovelButtonHeight = 40;
    int shovelButtonX = rect.Width() - shovelButtonWidth - 20;
    int shovelButtonY = 10;

    CRect shovelRect(shovelButtonX, shovelButtonY,
        shovelButtonX + shovelButtonWidth,
        shovelButtonY + shovelButtonHeight);

    return shovelRect.PtInRect(point);
}

void PVZView::DrawPauseScreen(CDC* pDC) {
    CRect rect;
    GetClientRect(&rect);

    //直接绘制游戏画面
    Yard& yard = theDoc->getYard();
    SeedBank& sbank = theDoc->getSeedBank();
    Player& player = theDoc->getPlayer();
    PVZDoc::SunlightList& sunList = theDoc->getSunList();

    //准备双缓冲
    CDC memDC;
    CBitmap buf;
    CFont font;
    memDC.CreateCompatibleDC(pDC);
    font.CreatePointFont(170, _T("微软雅黑"));
    buf.CreateCompatibleBitmap(pDC, rect.Width(), rect.Height());
    memDC.SelectObject(&buf);
    memDC.SelectObject(&font);

    //绘制白色背景
    memDC.FillSolidRect(rect, RGB(255, 255, 255));

    //绘制院子上的元素
    yard.draw(memDC.m_hDC);

    //绘制植物选项卡
    sbank.draw(memDC.m_hDC);

    //绘制植物 - 修复绘图调用
    auto& plantMatrix = yard.getPlantMatrix();
    for (int row = 0; row < 5; ++row) {
        for (int col = 0; col < 9; ++col) {
            if (plantMatrix[row][col]) {
                plantMatrix[row][col]->draw(memDC.m_hDC);
            }
        }
    }

    //绘制僵尸
    auto& zombieList = yard.getZombieList();
    for (int row = 0; row < 5; ++row) {
        for (auto& zombie : zombieList[row]) {
            if (zombie) zombie->draw(memDC.m_hDC);
        }
    }

    //绘制投射物
    auto& ejectList = yard.getEjectList();
    for (int row = 0; row < 5; ++row) {
        for (auto& eject : ejectList[row]) {
            if (eject) eject->draw(memDC.m_hDC);
        }
    }

    //绘制选中的植物
    player.drawCurrentPlant(memDC.m_hDC, yard);

    //绘制太阳
    for (auto& sun : sunList) {
        sun->draw(memDC.m_hDC);
    }

    //绘制半透明黑色覆盖层
    CDC overlayDC;
    CBitmap overlayBmp;
    overlayDC.CreateCompatibleDC(pDC);
    overlayBmp.CreateCompatibleBitmap(pDC, rect.Width(), rect.Height());
    overlayDC.SelectObject(&overlayBmp);

    //填充半透明黑色
    overlayDC.FillSolidRect(rect, RGB(0, 0, 0));

    BLENDFUNCTION blend;
    blend.BlendOp = AC_SRC_OVER;
    blend.BlendFlags = 0;
    blend.SourceConstantAlpha = 128;
    blend.AlphaFormat = 0;

    //将半透明层混合到游戏画面上
    AlphaBlend(memDC.m_hDC, 0, 0, rect.Width(), rect.Height(),
        overlayDC.m_hDC, 0, 0, rect.Width(), rect.Height(), blend);

    //绘制暂停文字
    CFont pauseFont;
    pauseFont.CreatePointFont(400, _T("微软雅黑"));
    CFont* oldFont = memDC.SelectObject(&pauseFont);

    COLORREF oldColor = memDC.SetTextColor(RGB(255, 0, 0));
    int oldBkMode = memDC.SetBkMode(TRANSPARENT);

    CString pauseText = _T("游戏暂停");
    CSize textSize = memDC.GetTextExtent(pauseText);
    int x = (rect.Width() - textSize.cx) / 2;
    int y = (rect.Height() - textSize.cy) / 2;

    //绘制文字阴影效果
    memDC.SetTextColor(RGB(0, 0, 0));
    memDC.TextOut(x + 3, y + 3, pauseText);

    memDC.SetTextColor(RGB(255, 0, 0));
    memDC.TextOut(x, y, pauseText);

    //绘制提示文字
    CFont hintFont;
    hintFont.CreatePointFont(120, _T("微软雅黑"));
    memDC.SelectObject(&hintFont);

    CString hintText = _T("按空格键继续游戏");
    CSize hintSize = memDC.GetTextExtent(hintText);
    int hintX = (rect.Width() - hintSize.cx) / 2;
    int hintY = y + textSize.cy + 40;

    memDC.SetTextColor(RGB(255, 255, 0));
    memDC.TextOut(hintX, hintY, hintText);

    //将最终画面输出到屏幕
    pDC->BitBlt(0, 0, rect.Width(), rect.Height(), &memDC, 0, 0, SRCCOPY);

    //恢复设置
    memDC.SelectObject(oldFont);
    memDC.SetTextColor(oldColor);
    memDC.SetBkMode(oldBkMode);

    //清理资源
    pauseFont.DeleteObject();
    hintFont.DeleteObject();
    font.DeleteObject();
    buf.DeleteObject();
    overlayBmp.DeleteObject();
    memDC.DeleteDC();
    overlayDC.DeleteDC();
}

void PVZView::DrawScorePopups(CDC* cDC) {
    if (PVZWinApp::scorePopups.empty()) return;

    CFont font;
    font.CreatePointFont(120, _T("微软雅黑"));
    CFont* oldFont = cDC->SelectObject(&font);
    int oldBkMode = cDC->SetBkMode(TRANSPARENT);

    for (const auto& popup : PVZWinApp::scorePopups) {
        int redValue = 255;
        int greenValue = static_cast<int>(255 * (1.0 - popup.alpha));  // 修复类型转换
        int blueValue = 0;
        COLORREF textColor = RGB(redValue, greenValue, blueValue);
        COLORREF oldColor = cDC->SetTextColor(textColor);

        CString scoreText;
        scoreText.Format(_T("+%d"), popup.points);

        int screenX = static_cast<int>(popup.x * ZOOM_FACTOR);  // 修复类型转换
        int screenY = static_cast<int>(popup.y * ZOOM_FACTOR);  // 修复类型转换
        cDC->TextOut(screenX, screenY, scoreText);
        cDC->SetTextColor(oldColor);
    }

    cDC->SelectObject(oldFont);
    cDC->SetBkMode(oldBkMode);
    font.DeleteObject();
}

void PVZView::DrawGameOverScreen(CDC* cDC) {
    CRect rect;
    GetClientRect(&rect);
    cDC->FillSolidRect(&rect, RGB(0, 0, 0));

    CFont font;
    font.CreatePointFont(400, _T("微软雅黑"));
    CFont* oldFont = cDC->SelectObject(&font);
    COLORREF oldColor = cDC->SetTextColor(RGB(255, 0, 0));
    int oldBkMode = cDC->SetBkMode(TRANSPARENT);

    CString gameOverText = _T("游戏失败");
    CSize textSize = cDC->GetTextExtent(gameOverText);
    int x = (rect.Width() - textSize.cx) / 2;
    int y = (rect.Height() - textSize.cy) / 2 - 50;
    cDC->TextOut(x, y, gameOverText);

    CString finalScoreText;
    finalScoreText.Format(_T("最终分数:%d"), PVZWinApp::score);
    CSize scoreTextSize = cDC->GetTextExtent(finalScoreText);
    int scoreX = (rect.Width() - scoreTextSize.cx) / 2;
    int scoreY = y + textSize.cy + 20;
    cDC->TextOut(scoreX, scoreY, finalScoreText);

    cDC->SelectObject(oldFont);
    cDC->SetTextColor(oldColor);
    cDC->SetBkMode(oldBkMode);
    font.DeleteObject();

    DrawRestartButton(cDC);
}

void PVZView::DrawRestartButton(CDC* cDC) {
    CRect rect;
    GetClientRect(&rect);
    int buttonWidth = 200;
    int buttonHeight = 60;
    int buttonX = (rect.Width() - buttonWidth) / 2;
    int buttonY = rect.Height() - buttonHeight - 20;

    CRect buttonRect(buttonX, buttonY, buttonX + buttonWidth, buttonY + buttonHeight);
    cDC->FillSolidRect(buttonRect, RGB(128, 128, 128));

    CPen pen(PS_SOLID, 2, RGB(200, 200, 200));
    CPen* oldPen = cDC->SelectObject(&pen);
    cDC->Rectangle(buttonRect);
    cDC->SelectObject(oldPen);

    CFont font;
    font.CreatePointFont(200, _T("微软雅黑"));
    CFont* oldFont = cDC->SelectObject(&font);
    COLORREF oldColor = cDC->SetTextColor(RGB(0, 255, 0));
    int oldBkMode = cDC->SetBkMode(TRANSPARENT);

    CString restartText = _T("重新开始");
    CSize textSize = cDC->GetTextExtent(restartText);
    int textX = buttonX + (buttonWidth - textSize.cx) / 2;
    int textY = buttonY + (buttonHeight - textSize.cy) / 2;
    cDC->TextOut(textX, textY, restartText);

    cDC->SelectObject(oldFont);
    cDC->SetTextColor(oldColor);
    cDC->SetBkMode(oldBkMode);
    font.DeleteObject();
}

bool PVZView::IsClickRestartButton(CPoint point) {
    if (!PVZWinApp::gameOver) { return false; }

    CRect rect;
    GetClientRect(&rect);
    int buttonWidth = 200;
    int buttonHeight = 60;
    int buttonX = (rect.Width() - buttonWidth) / 2;
    int buttonY = rect.Height() - buttonHeight - 20;
    CRect buttonRect(buttonX, buttonY, buttonX + buttonWidth, buttonY + buttonHeight);
    return buttonRect.PtInRect(point);
}

void PVZView::DrawMouseCoordinates(CDC* cDC) {
    CPoint point = m_lastMousePos;

    int gameX = static_cast<int>(point.x / ZOOM_FACTOR);  // 修复类型转换
    int gameY = static_cast<int>(point.y / ZOOM_FACTOR);  // 修复类型转换

    CFont font;
    font.CreatePointFont(90, _T("微软雅黑"));

    CFont* oldFont = cDC->SelectObject(&font);
    COLORREF oldColor = cDC->SetTextColor(RGB(255, 255, 0));
    int oldBkMode = cDC->SetBkMode(TRANSPARENT);

    CString coordText;
    coordText.Format(_T("(%d,%d)"), gameX, gameY);

    cDC->TextOut(point.x + 15, point.y + 15, coordText);

    cDC->SelectObject(oldFont);
    cDC->SetTextColor(oldColor);
    cDC->SetBkMode(oldBkMode);
    font.DeleteObject();
}

void PVZView::DrawScore(CDC* cDC) {
    CFont font;
    font.CreatePointFont(120, _T("微软雅黑"));
    CFont* oldFont = cDC->SelectObject(&font);
    COLORREF oldColor = cDC->SetTextColor(RGB(255, 255, 0));
    int oldBkMode = cDC->SetBkMode(TRANSPARENT);

    CString scoreText;
    scoreText.Format(_T("分数:%d"), PVZWinApp::score);

    cDC->TextOut(10, 10, scoreText);

    cDC->SelectObject(oldFont);
    cDC->SetTextColor(oldColor);
    cDC->SetBkMode(oldBkMode);
    font.DeleteObject();
}

void PVZView::OnMouseMove(UINT nFlags, CPoint point) {
    if (PVZWinApp::gamePaused && !PVZWinApp::gameOver) {
        CView::OnMouseMove(nFlags, point);
        return;
    }

    if (m_pDocument) {
        Player& player = ((PVZDoc*)m_pDocument)->getPlayer();
        player.setPos({ static_cast<int>(point.x / ZOOM_FACTOR), static_cast<int>(point.y / ZOOM_FACTOR) });  // 修复类型转换

        if (m_bTrackingCoordinates) {
            m_lastMousePos = point;
            CRect rect(point.x, point.y, point.x + 100, point.y + 30);
            InvalidateRect(rect, FALSE);
        }
    }
    CView::OnMouseMove(nFlags, point);
}

void PVZView::OnLButtonDown(UINT nFlags, CPoint point) {
    //如果游戏暂停，不处理鼠标点击
    if (PVZWinApp::gamePaused && !PVZWinApp::gameOver) {
        CView::OnLButtonDown(nFlags, point);
        return;
    }

    //如果游戏失败，检查是否点击了重新开始按钮
    if (PVZWinApp::gameOver) {
        if (IsClickRestartButton(point)) {
            PVZWinApp::ResetGame();
            Invalidate(TRUE);
            return;
        }
    }

    //检查是否点击了铲子按钮
    if (IsClickShovelButton(point)) {
        m_bShovelMode = !m_bShovelMode;
        Invalidate(FALSE);
        return;
    }

    //铲子模式：移除植物
    if (m_bShovelMode && m_pDocument) {
        auto doc = (PVZDoc*)m_pDocument;
        Yard& yard = doc->getYard();

        //将点击位置转换为游戏坐标
        int gameX = static_cast<int>(point.x / ZOOM_FACTOR);  // 修复类型转换
        int gameY = static_cast<int>(point.y / ZOOM_FACTOR);  // 修复类型转换

        //直接遍历植物矩阵来检测点击
        bool plantRemoved = false;
        auto& plantMatrix = yard.getPlantMatrix();

        for (int row = 0; row < 5; ++row) {
            for (int col = 0; col < 9; ++col) {
                auto& plant = plantMatrix[row][col];
                if (plant) {
                    int plantLeftX = plant->getLeftX();
                    int plantTopY = plant->getTopY();
                    int plantRightX = plantLeftX + plant->getWidth();
                    int plantBottomY = plantTopY + plant->getHeight();

                    //检查点击位置是否在植物范围内
                    if (gameX >= plantLeftX && gameX <= plantRightX &&
                        gameY >= plantTopY && gameY <= plantBottomY) {
                        //移除植物
                        plant = nullptr;
                        plantRemoved = true;

#ifdef _DEBUG
                        TRACE(_T("移除植物: 行%d, 列%d\n"), row, col);
#endif

                        break;
                    }
                }
            }
            if (plantRemoved) break;
        }

        if (plantRemoved) {
            //退出铲子模式
            m_bShovelMode = false;
            Invalidate(FALSE);
        }
        return;
    }

    //正常的游戏点击逻辑
    if (m_pDocument) {
        auto doc = (PVZDoc*)m_pDocument;
        SeedBank& sbank = doc->getSeedBank();
        Player& player = doc->getPlayer();
        Yard& yard = doc->getYard();
        auto& sunList = doc->getSunList();

        player.selectPlant(yard, sbank, point);
        player.collectSun(yard, sbank, sunList, point);

        Invalidate(FALSE);
    }
}

void PVZView::OnRButtonDown(UINT nFlags, CPoint point) {
    if (PVZWinApp::gamePaused && !PVZWinApp::gameOver) {
        CView::OnRButtonDown(nFlags, point);
        return;
    }

    //右键点击退出铲子模式
    if (m_bShovelMode) {
        m_bShovelMode = false;
        Invalidate(FALSE);
        return;
    }

    m_bTrackingCoordinates = !m_bTrackingCoordinates;

    if (m_bTrackingCoordinates) {
        m_lastMousePos = point;
    }

    Invalidate(FALSE);

    CView::OnRButtonDown(nFlags, point);
}

void PVZView::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags) {
    if (nChar == VK_SPACE && !PVZWinApp::gameOver) {
        PVZWinApp::gamePaused = !PVZWinApp::gamePaused;

#ifdef _DEBUG
        TRACE(_T("游戏%s\n"), PVZWinApp::gamePaused ? _T("暂停") : _T("继续"));
#endif

        Invalidate(TRUE);
    }

    // ESC键退出铲子模式
    if (nChar == VK_ESCAPE && m_bShovelMode) {
        m_bShovelMode = false;
        Invalidate(FALSE);
    }

    CView::OnKeyDown(nChar, nRepCnt, nFlags);
}

void PVZView::OnSelect(UINT nFlags, CPoint point) {}
void PVZView::OnPlace(UINT nFlags, CPoint point) {}