#include "Ejects.h"
#include "PVZDoc.h"
#include "Zombie.h"

extern PVZDoc* theDoc;

IMPLEMENT_DYNCREATE(Ejects, GameEntity)

Ejects::Ejects(CRuntimeClass* classMsg, int h)
    : GameEntity(classMsg), hurt(h) {
}

// 绘制
void Ejects::draw(HDC hDC, int xOffset, int yOffset) {
    // GameEntity::draw(hDC);
    auto& eject = rcManage.getResource(classMsg->m_lpszClassName, getMapState());
    if (animateTick >= eject.size() - 1) {
        if (isState(Ejects::BREAK)) addState(Ejects::DEL);
        animateTick = 0;
    }
    auto& shadow = rcManage.getResource("Ejects", Visible::Static).at(0);
    eject[animateTick]->Draw(hDC, (int)leftX, (int)topY);
    shadow->Draw(hDC, (int)leftX, (int)(topY + eject[animateTick]->GetWidth() * 1.7),
        shadow->GetWidth() / 2, shadow->GetHeight(), 0, 0,
        shadow->GetWidth() / 2, shadow->GetHeight());
}

void Ejects::update() {
    if (isState(Ejects::DEFAULT)) {
        move();
    }
    // 更新碰撞体位置
    collisioArea = CRect((int)getLeftX(), (int)getTopY(), (int)(getLeftX() + getWidth()),
        (int)(getTopY() + getHeight()));

    auto& zombieList = theDoc->getYard().getZombieList();
    auto collisioChk = [&](Yard::zombie_iter& zombieIter, int) {
        if (this->hurt > 0 && collisio(**zombieIter)) {
            // 先保存伤害值
            int currentHurt = this->hurt;

            setMapState(Ejects::ImgBreak);
            cleanState(Ejects::DEFAULT);
            addState(Ejects::BREAK);
            (*zombieIter)->addState(Zombie::HURT);
            (*zombieIter)->damage(currentHurt);

            // 新增：寒冰豌豆减速效果
            if (strcmp(classMsg->m_lpszClassName, "ProjectileSnowPea") == 0) {
                // 使用新的减速方法
                (*zombieIter)->applySlowEffect(300); // 减速持续300帧
            }

            setHurt(0);
        }
        };
    theDoc->getYard().foreach(zombieList, collisioChk);
}

void Ejects::move() {}