#include "Ejects.h"

#include "PVZDoc.h"
#include "Zombie.h"

extern PVZDoc* theDoc;

IMPLEMENT_DYNCREATE(Ejects, GameEntity)

Ejects::Ejects(CRuntimeClass* classMsg, int h)
    : GameEntity(classMsg), hurt(h) {}

// 绘制
void Ejects::draw(HDC hDC, int xOffset, int yOffset) {
  GameEntity::draw(hDC);
  auto& eject = rcManage.getResource(classMsg->m_lpszClassName, getMapState());
  if (animateTick >= eject.size() - 1) {
    if (isState(Ejects::BREAK)) addState(Ejects::DEL);
    animateTick = 0;
  }
  auto& shadow = rcManage.getResource("Ejects", Visible::Static).at(0);
  eject[animateTick]->Draw(hDC, (int)leftX, (int)topY);
  shadow->Draw(hDC, leftX, topY + eject[animateTick]->GetWidth() * 1.7,
               shadow->GetWidth() / 2, shadow->GetHeight(), 0, 0,
               shadow->GetWidth() / 2, shadow->GetHeight());
}

void Ejects::update() {
  if (isState(Ejects::DEFAULT)) {
    move();
  }
  // 更新碰撞体位置
  collisioArea = CRect(getLeftX(), getTopY(), getLeftX() + getWidth(),
                       getTopY() + getHeight());

  auto& zombieList = theDoc->getYard().getZombieList();
  auto collisioChk = [&](Yard::zombie_iter& zombieIter, int) {
    if (this->hurt > 0 && collisio(**zombieIter)) {
      setMapState(Ejects::ImgBreak);
      cleanState(Ejects::DEFAULT);
      addState(Ejects::BREAK);
      (*zombieIter)->addState(Zombie::HURT);
      (*zombieIter)->damage(getHurt());
      setHurt(0);
    }
  };
  theDoc->getYard().foreach (zombieList, collisioChk);
}

//
void Ejects::move() {}