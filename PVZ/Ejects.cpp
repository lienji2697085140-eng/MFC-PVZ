#include "Ejects.h"
#include "PVZDoc.h"

extern PVZDoc* theDoc;

IMPLEMENT_DYNCREATE(Ejects, GameElement)

Ejects::Ejects(CRuntimeClass* classMsg, int h) : GameElement(classMsg), hurt(h) { }

// 绘制
void Ejects::draw(HDC hDC) {
    // GameElement::draw(hDC);
    auto& eject = rcManage.getResource(classMsg->m_lpszClassName, getMapState());
    if (animateTick >= eject.size() - 1) {
        if (isState(Ejects::BREAK)) addState(Ejects::DEL);
        animateTick = 0;
    }
    auto& shadow = rcManage.getResource("Ejects", Visible::Static).at(0);
    eject[animateTick]->Draw(hDC, leftX, topY);
    shadow->Draw(hDC, leftX, topY + eject[animateTick]->GetWidth() * 1.7, shadow->GetWidth() / 2, shadow->GetHeight(), 0, 0, shadow->GetWidth() / 2, shadow->GetHeight());
}

void Ejects::update() {
    if (isState(Ejects::DEFAULT)) {
        move();
    }
    // 更新碰撞体位置
    collisioArea = CRect(getLeftX(), getTopY(), 
                getLeftX() + getWidth(), getTopY() + getHeight());
}

//
void Ejects::move() { 

}