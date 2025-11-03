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

void PVZView::OnDraw(CDC* cDC) {
    // 获得院子的信息
    Yard& yard = theDoc->getYard();
    SeedBank& sbank = theDoc->getSeedBank();
    Player& player = theDoc->getPlayer();
    PVZDoc::SunlightList& sunList = theDoc->getSunList();

    // 准备双缓冲所需的数据
    CDC memDC;
    CBitmap buf;
    CFont font;
    memDC.CreateCompatibleDC(cDC);
    font.CreatePointFont(170, "微软雅黑");
    buf.CreateCompatibleBitmap(cDC, (int)(yard.getWidth()), (int)(yard.getHeight()));
    memDC.SelectObject(buf);
    memDC.SelectObject(font);

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
    cDC->AlphaBlend(0, 0, (int)(yard.getWidth() * ZOOM_FACTOR),
        (int)(yard.getHeight() * ZOOM_FACTOR), &memDC, 0, 0, (int)yard.getWidth(),
        (int)(yard.getHeight()), { AC_SRC_OVER, 0, 255, 0 });

    // 在鼠标位置显示坐标（在双缓冲绘制完成后添加）
    DrawMouseCoordinates(cDC);

    buf.DeleteObject();
    memDC.DeleteDC();
}

// 新增函数：在鼠标位置显示坐标
void PVZView::DrawMouseCoordinates(CDC* cDC) {
    // 获取鼠标位置
    CPoint point;
    GetCursorPos(&point);
    ScreenToClient(&point);

    // 转换为游戏内坐标（除以缩放因子）
    int gameX = (int)(point.x / ZOOM_FACTOR);
    int gameY = (int)(point.y / ZOOM_FACTOR);

    // 创建字体
    CFont font;
    font.CreatePointFont(90, "微软雅黑"); // 小一点的字体

    // 保存原来的设置
    CFont* oldFont = cDC->SelectObject(&font);
    COLORREF oldColor = cDC->SetTextColor(RGB(255, 255, 0)); // 黄色文字
    int oldBkMode = cDC->SetBkMode(TRANSPARENT);

    // 格式化坐标字符串
    CString coordText;
    coordText.Format(_T("(%d, %d)"), gameX, gameY);

    // 在鼠标位置旁边显示坐标（稍微偏移避免被鼠标挡住）
    cDC->TextOut(point.x + 15, point.y + 15, coordText);

    // 恢复原来的设置
    cDC->SelectObject(oldFont);
    cDC->SetTextColor(oldColor);
    cDC->SetBkMode(oldBkMode);
    font.DeleteObject();
}

void PVZView::OnMouseMove(UINT nFlags, CPoint point) {
    Player& player = ((PVZDoc*)m_pDocument)->getPlayer();
    // 更新鼠标指针位置
    player.setPos({ int(point.x / ZOOM_FACTOR), int(point.y / ZOOM_FACTOR) });

    // 刷新显示，更新坐标位置
    Invalidate(FALSE);

    CView::OnMouseMove(nFlags, point);
}

void PVZView::OnLButtonDown(UINT nFlags, CPoint point) {
    auto doc = ((PVZDoc*)m_pDocument);
    SeedBank& sbank = doc->getSeedBank();
    Player& player = doc->getPlayer();
    Yard& yard = doc->getYard();
    auto& sunList = doc->getSunList();

    player.selectPlant(yard, sbank, point);

    player.collectSun(yard, sbank, sunList, point);

    // 点击时也刷新坐标显示
    Invalidate(FALSE);
}

void PVZView::OnSelect(UINT nFlags, CPoint point) {}

void PVZView::OnPlace(UINT nFlags, CPoint point) {}
