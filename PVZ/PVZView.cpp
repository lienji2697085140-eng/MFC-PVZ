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
END_MESSAGE_MAP()

void PVZView::OnDraw(CDC* cDC) {
    //如果游戏失败，显示全屏黑屏和游戏失败文字
    if (PVZWinApp::gameOver) {
        DrawGameOverScreen(cDC);
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
    memDC.CreateCompatibleDC(cDC);
    font.CreatePointFont(170, "微软雅黑");
    buf.CreateCompatibleBitmap(cDC, (int)(yard.getWidth()), (int)(yard.getHeight()));
    memDC.SelectObject(buf);
    memDC.SelectObject(font);

    //绘制院子上的元素
    yard.draw(memDC.m_hDC);
    //绘制植物选项卡
    sbank.draw(memDC.m_hDC);
    //绘制植物僵尸
    yard.foreach(yard.getPlantMatrix(), [&](Yard::plant_iter& iter, int) {
        if (*iter) (*iter)->draw(memDC.m_hDC);
        });
    yard.foreach(yard.getZombieList(), [&](Yard::zombie_iter& iter, int row) {
        if (*iter) (*iter)->draw(memDC.m_hDC);
        });
    yard.foreach(yard.getEjectList(), [&](Yard::ejects_iter& iter, int) {
        if (*iter) (*iter)->draw(memDC.m_hDC);
        });

    //绘制选中的植物
    player.drawCurrentPlant(memDC.m_hDC, yard);
    //绘制太阳
    for (auto& sun : sunList) {
        sun->draw(memDC.m_hDC);
    }
    //结束双缓冲绘制
    cDC->AlphaBlend(0, 0, (int)(yard.getWidth() * ZOOM_FACTOR),
        (int)(yard.getHeight() * ZOOM_FACTOR), &memDC, 0, 0, (int)yard.getWidth(),
        (int)(yard.getHeight()), { AC_SRC_OVER, 0, 255, 0 });

    //显示分数
    DrawScore(cDC);

    //在鼠标位置显示坐标
    DrawMouseCoordinates(cDC);

    buf.DeleteObject();
    memDC.DeleteDC();
}

void PVZView::DrawGameOverScreen(CDC* cDC) {
    //绘制全屏黑屏
    CRect rect;
    GetClientRect(&rect);
    cDC->FillSolidRect(&rect, RGB(0, 0, 0));

    //创建大号字体
    CFont font;
    font.CreatePointFont(400, "微软雅黑");

    CFont* oldFont = cDC->SelectObject(&font);
    COLORREF oldColor = cDC->SetTextColor(RGB(255, 0, 0));
    int oldBkMode = cDC->SetBkMode(TRANSPARENT);

    CString gameOverText = _T("游戏失败");
    CSize textSize = cDC->GetTextExtent(gameOverText);
    int x = (rect.Width() - textSize.cx) / 2;
    int y = (rect.Height() - textSize.cy) / 2 - 50;

    cDC->TextOut(x, y, gameOverText);

    //显示最终分数
    CString finalScoreText;
    finalScoreText.Format(_T("最终分数: %d"), PVZWinApp::score);
    CSize scoreTextSize = cDC->GetTextExtent(finalScoreText);
    int scoreX = (rect.Width() - scoreTextSize.cx) / 2;
    int scoreY = y + textSize.cy + 20;
    cDC->TextOut(scoreX, scoreY, finalScoreText);

    //恢复原来的设置
    cDC->SelectObject(oldFont);
    cDC->SetTextColor(oldColor);
    cDC->SetBkMode(oldBkMode);
    font.DeleteObject();

    //绘制重新开始按钮
    DrawRestartButton(cDC);
}

void PVZView::DrawRestartButton(CDC* cDC) {
    CRect rect;
    GetClientRect(&rect);

    int buttonWidth = 200;
    int buttonHeight = 60;
    int buttonX = (rect.Width() - buttonWidth) / 2;
    int buttonY = rect.Height() - buttonHeight - 20; // 距离底部20像素

    //绘制灰色按钮背景
    CRect buttonRect(buttonX, buttonY, buttonX + buttonWidth, buttonY + buttonHeight);
    cDC->FillSolidRect(buttonRect, RGB(128, 128, 128));

    //绘制按钮边框
    CPen pen(PS_SOLID, 2, RGB(200, 200, 200));
    CPen* oldPen = cDC->SelectObject(&pen);
    cDC->Rectangle(buttonRect);
    cDC->SelectObject(oldPen);

    //创建按钮字体
    CFont font;
    font.CreatePointFont(200, "微软雅黑");

    CFont* oldFont = cDC->SelectObject(&font);
    COLORREF oldColor = cDC->SetTextColor(RGB(0, 255, 0));
    int oldBkMode = cDC->SetBkMode(TRANSPARENT);

    CString restartText = _T("重新开始");

    //计算文字在按钮中的位置（居中）
    CSize textSize = cDC->GetTextExtent(restartText);
    int textX = buttonX + (buttonWidth - textSize.cx) / 2;
    int textY = buttonY + (buttonHeight - textSize.cy) / 2;

    cDC->TextOut(textX, textY, restartText);

    //恢复原来的设置
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

    //按钮位置（与绘制时一致）
    int buttonWidth = 200;
    int buttonHeight = 60;
    int buttonX = (rect.Width() - buttonWidth) / 2;
    int buttonY = rect.Height() - buttonHeight - 20; // 距离底部20像素

    CRect buttonRect(buttonX, buttonY, buttonX + buttonWidth, buttonY + buttonHeight);

    return buttonRect.PtInRect(point);
}

void PVZView::DrawMouseCoordinates(CDC* cDC) {
    //获取鼠标位置
    CPoint point;
    GetCursorPos(&point);
    ScreenToClient(&point);

    //转换为游戏内坐标（除以缩放因子）
    int gameX = (int)(point.x / ZOOM_FACTOR);
    int gameY = (int)(point.y / ZOOM_FACTOR);

    //创建字体
    CFont font;
    font.CreatePointFont(90, "微软雅黑");

    //保存原来的设置
    CFont* oldFont = cDC->SelectObject(&font);
    COLORREF oldColor = cDC->SetTextColor(RGB(255, 255, 0));
    int oldBkMode = cDC->SetBkMode(TRANSPARENT);

    //格式化坐标字符串
    CString coordText;
    coordText.Format(_T("(%d, %d)"), gameX, gameY);

    //在鼠标位置旁边显示坐标（稍微偏移避免被鼠标挡住）
    cDC->TextOut(point.x + 15, point.y + 15, coordText);

    //恢复原来的设置
    cDC->SelectObject(oldFont);
    cDC->SetTextColor(oldColor);
    cDC->SetBkMode(oldBkMode);
    font.DeleteObject();
}

void PVZView::DrawScore(CDC* cDC) {
    //创建分数字体
    CFont font;
    font.CreatePointFont(120, "微软雅黑");

    CFont* oldFont = cDC->SelectObject(&font);
    COLORREF oldColor = cDC->SetTextColor(RGB(255, 255, 0));
    int oldBkMode = cDC->SetBkMode(TRANSPARENT);

    //格式化分数字符串
    CString scoreText;
    scoreText.Format(_T("分数: %d"), PVZWinApp::score);

    //在左上角显示分数
    cDC->TextOut(10, 10, scoreText);

    //恢复原来的设置
    cDC->SelectObject(oldFont);
    cDC->SetTextColor(oldColor);
    cDC->SetBkMode(oldBkMode);
    font.DeleteObject();
}

void PVZView::OnMouseMove(UINT nFlags, CPoint point) {
    Player& player = ((PVZDoc*)m_pDocument)->getPlayer();
    //更新鼠标指针位置
    player.setPos({ int(point.x / ZOOM_FACTOR), int(point.y / ZOOM_FACTOR) });

    //刷新显示，更新坐标位置
    Invalidate(FALSE);

    CView::OnMouseMove(nFlags, point);
}

void PVZView::OnLButtonDown(UINT nFlags, CPoint point) {
    //如果游戏失败，检查是否点击了重新开始按钮
    if (PVZWinApp::gameOver) {
        if (IsClickRestartButton(point)) {
            //重置游戏
            PVZWinApp::ResetGame();
            Invalidate(TRUE); //刷新屏幕
            return;
        }
    }

    //正常的游戏点击逻辑
    auto doc = ((PVZDoc*)m_pDocument);
    SeedBank& sbank = doc->getSeedBank();
    Player& player = doc->getPlayer();
    Yard& yard = doc->getYard();
    auto& sunList = doc->getSunList();

    player.selectPlant(yard, sbank, point);
    player.collectSun(yard, sbank, sunList, point);

    //点击时也刷新坐标显示
    Invalidate(FALSE);
}

void PVZView::OnSelect(UINT nFlags, CPoint point) {}

void PVZView::OnPlace(UINT nFlags, CPoint point) {}