#include "stdafx.h"  // 确保stdafx.h在最前面
#include "PVZView.h"
#include "Config.h"
#include "PVZDoc.h"
#include "Plant.h"
#include "Sun.h"
#include "Yard.h"
#include "Zombie.h"
#include "resource.h"

extern PVZDoc* theDoc;

IMPLEMENT_DYNCREATE(PVZView, CView)

BEGIN_MESSAGE_MAP(PVZView, CView)
    ON_WM_MOUSEMOVE()
    ON_WM_LBUTTONDOWN()
END_MESSAGE_MAP()

void PVZView::OnDraw(CDC* pDC) {
    // 如果游戏失败,显示全屏黑屏和游戏失败文字
    if (PVZWinApp::gameOver) {
        DrawGameOverScreen(pDC);
        return;
    }

    // 正常的游戏绘制代码
    Yard& yard = theDoc->getYard();
    SeedBank& sbank = theDoc->getSeedBank();
    Player& player = theDoc->getPlayer();
    PVZDoc::SunlightList& sunList = theDoc->getSunList();

    // 准备双缓冲所需的数据
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

    // 绘制白色背景
    memDC.FillSolidRect(rect, RGB(255, 255, 255));

    // 绘制院子上的元素
    yard.draw(memDC.m_hDC);
    // 绘制植物选项卡
    sbank.draw(memDC.m_hDC);
    // 绘制植物僵尸
    yard.foreach(yard.getPlantMatrix(), [&](Yard::plant_iter& iter, int) {
        if (*iter) (*iter)->draw(memDC.m_hDC);
        });
    yard.foreach(yard.getZombieList(), [&](Yard::zombie_iter& iter, int row) {
        if (*iter) (*iter)->draw(memDC.m_hDC);
        });
    yard.foreach(yard.getEjectList(), [&](Yard::ejects_iter& iter, int) {
        if (*iter) (*iter)->draw(memDC.m_hDC);
        });
    // 绘制选中的植物
    player.drawCurrentPlant(memDC.m_hDC, yard);
    // 绘制太阳
    for (auto& sun : sunList) {
        sun->draw(memDC.m_hDC);
    }

    // 结束双缓冲绘制
    pDC->BitBlt(0, 0, rect.Width(), rect.Height(), &memDC, 0, 0, SRCCOPY);

    // 显示分数
    DrawScore(pDC);
    // 新增：绘制分数弹出效果
    DrawScorePopups(pDC);
    // 在鼠标位置显示坐标
    DrawMouseCoordinates(pDC);

    buf.DeleteObject();
    memDC.DeleteDC();
    font.DeleteObject();
}

// 新增：绘制分数弹出效果函数实现
void PVZView::DrawScorePopups(CDC* cDC) {
    if (PVZWinApp::scorePopups.empty()) return;

    // 创建字体
    CFont font;
    font.CreatePointFont(120, _T("微软雅黑"));
    CFont* oldFont = cDC->SelectObject(&font);

    int oldBkMode = cDC->SetBkMode(TRANSPARENT);

    for (const auto& popup : PVZWinApp::scorePopups) {
        // 根据透明度设置颜色（从红色渐变为黄色）
        // 当alpha从1.0到0.0时，红色分量保持255，绿色分量从0增加到255
        int redValue = 255;
        int greenValue = (int)(255 * (1.0 - popup.alpha)); // 透明度越低，绿色分量越高
        int blueValue = 0;

        COLORREF textColor = RGB(redValue, greenValue, blueValue);

        COLORREF oldColor = cDC->SetTextColor(textColor);

        // 格式化分数文本
        CString scoreText;
        scoreText.Format(_T("+%d"), popup.points);

        // 在指定位置绘制分数（应用缩放因子）
        int screenX = (int)(popup.x * ZOOM_FACTOR);
        int screenY = (int)(popup.y * ZOOM_FACTOR);
        cDC->TextOut(screenX, screenY, scoreText);

        cDC->SetTextColor(oldColor);
    }

    // 恢复设置
    cDC->SelectObject(oldFont);
    cDC->SetBkMode(oldBkMode);
    font.DeleteObject();
}

void PVZView::DrawGameOverScreen(CDC* cDC) {
    // 绘制全屏黑屏
    CRect rect;
    GetClientRect(&rect);
    cDC->FillSolidRect(&rect, RGB(0, 0, 0));

    // 创建大号字体
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

    // 显示最终分数
    CString finalScoreText;
    finalScoreText.Format(_T("最终分数:%d"), PVZWinApp::score);
    CSize scoreTextSize = cDC->GetTextExtent(finalScoreText);
    int scoreX = (rect.Width() - scoreTextSize.cx) / 2;
    int scoreY = y + textSize.cy + 20;
    cDC->TextOut(scoreX, scoreY, finalScoreText);

    // 恢复原来的设置
    cDC->SelectObject(oldFont);
    cDC->SetTextColor(oldColor);
    cDC->SetBkMode(oldBkMode);
    font.DeleteObject();

    // 绘制重新开始按钮
    DrawRestartButton(cDC);
}

void PVZView::DrawRestartButton(CDC* cDC) {
    CRect rect;
    GetClientRect(&rect);
    int buttonWidth = 200;
    int buttonHeight = 60;
    int buttonX = (rect.Width() - buttonWidth) / 2;
    int buttonY = rect.Height() - buttonHeight - 20;

    // 绘制灰色按钮背景
    CRect buttonRect(buttonX, buttonY, buttonX + buttonWidth, buttonY + buttonHeight);
    cDC->FillSolidRect(buttonRect, RGB(128, 128, 128));

    // 绘制按钮边框
    CPen pen(PS_SOLID, 2, RGB(200, 200, 200));
    CPen* oldPen = cDC->SelectObject(&pen);
    cDC->Rectangle(buttonRect);
    cDC->SelectObject(oldPen);

    // 创建按钮字体
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

    // 恢复原来的设置
    cDC->SelectObject(oldFont);
    cDC->SetTextColor(oldColor);
    cDC->SetBkMode(oldBkMode);
    font.DeleteObject();
}

bool PVZView::IsClickRestartButton(CPoint point) {
    if (!PVZWinApp::gameOver) {
        return false;
    }

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
    // 获取鼠标位置
    CPoint point;
    GetCursorPos(&point);
    ScreenToClient(&point);

    // 转换为游戏内坐标(除以缩放因子)
    int gameX = (int)(point.x / ZOOM_FACTOR);
    int gameY = (int)(point.y / ZOOM_FACTOR);

    // 创建字体
    CFont font;
    font.CreatePointFont(90, _T("微软雅黑"));

    // 保存原来的设置
    CFont* oldFont = cDC->SelectObject(&font);
    COLORREF oldColor = cDC->SetTextColor(RGB(255, 255, 0));
    int oldBkMode = cDC->SetBkMode(TRANSPARENT);

    // 格式化坐标字符串
    CString coordText;
    coordText.Format(_T("(%d,%d)"), gameX, gameY);

    // 在鼠标位置旁边显示坐标
    cDC->TextOut(point.x + 15, point.y + 15, coordText);

    // 恢复原来的设置
    cDC->SelectObject(oldFont);
    cDC->SetTextColor(oldColor);
    cDC->SetBkMode(oldBkMode);
    font.DeleteObject();
}

void PVZView::DrawScore(CDC* cDC) {
    // 创建分数字体
    CFont font;
    font.CreatePointFont(120, _T("微软雅黑"));
    CFont* oldFont = cDC->SelectObject(&font);
    COLORREF oldColor = cDC->SetTextColor(RGB(255, 255, 0));
    int oldBkMode = cDC->SetBkMode(TRANSPARENT);

    // 格式化分数字符串
    CString scoreText;
    scoreText.Format(_T("分数:%d"), PVZWinApp::score);

    // 在左上角显示分数
    cDC->TextOut(10, 10, scoreText);

    // 恢复原来的设置
    cDC->SelectObject(oldFont);
    cDC->SetTextColor(oldColor);
    cDC->SetBkMode(oldBkMode);
    font.DeleteObject();
}

void PVZView::OnMouseMove(UINT nFlags, CPoint point) {
    if (m_pDocument) {
        Player& player = ((PVZDoc*)m_pDocument)->getPlayer();
        // 更新鼠标指针位置
        player.setPos({ int(point.x / ZOOM_FACTOR), int(point.y / ZOOM_FACTOR) });
        // 刷新显示，更新坐标位置
        Invalidate(FALSE);
    }
    CView::OnMouseMove(nFlags, point);
}

void PVZView::OnLButtonDown(UINT nFlags, CPoint point) {
    // 如果游戏失败，检查是否点击了重新开始按钮
    if (PVZWinApp::gameOver) {
        if (IsClickRestartButton(point)) {
            // 重置游戏
            PVZWinApp::ResetGame();
            Invalidate(TRUE);
            return;
        }
    }

    // 正常的游戏点击逻辑
    if (m_pDocument) {
        auto doc = (PVZDoc*)m_pDocument;
        SeedBank& sbank = doc->getSeedBank();
        Player& player = doc->getPlayer();
        Yard& yard = doc->getYard();
        auto& sunList = doc->getSunList();

        player.selectPlant(yard, sbank, point);
        player.collectSun(yard, sbank, sunList, point);

        // 点击时也刷新坐标显示
        Invalidate(FALSE);
    }
}

void PVZView::OnSelect(UINT nFlags, CPoint point) {}

void PVZView::OnPlace(UINT nFlags, CPoint point) {}