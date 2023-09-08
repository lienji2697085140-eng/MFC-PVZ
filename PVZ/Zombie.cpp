#include "Zombie.h"
#include "GameElement.h"
#include "PVZDoc.h"

IMPLEMENT_DYNCREATE(Zombie, GameElement)

extern PVZDoc* theDoc;

Zombie::Zombie(CRuntimeClass* classMsg, int hp, double moveSpeed, int h) : 
    GameElement(classMsg, hp), speed(moveSpeed), hurt(h) { 
    // 初始化碰撞信息
    
}

void Zombie::draw(HDC hDC) {
    // GameElement::draw(hDC);
    std::string className(classMsg->m_lpszClassName);
    auto& rc = rcManage.getResource(className, getMapState());

    if (isState(Zombie::HURT)) {
        cleanState(Zombie::HURT);
        rc[animateTick]->AlphaBlend(hDC, CPoint(leftX, topY), 200, 0);
    }
    else {
        rc[animateTick]->Draw(hDC, leftX, topY);
    }
}

void Zombie::update() {
    auto& yard = theDoc->getYard();
    if (isState(Zombie::MOVE)) {
        leftX -= yard.getWidth() * speed;
    }
    collisioArea = CRect(getLeftX() + getWidth() * 0.45, getTopY() + getWidth() * 0.25,
        getLeftX() + getWidth() * 0.6, getTopY() + getHeight() * 0.75);
}

void Zombie::stateSwitch(bool isHalf) {
    if (isState(Zombie::EAT)) {
        attack();
        // setMapState(Zombie::EatDynamic);
    }
}

void Zombie::attack() {
    // 伤害植物
    if (targetPlant) targetPlant->damage(hurt);
}

bool Zombie::collisio(GameElement& e) {
    return GameElement::collisio(e);
}