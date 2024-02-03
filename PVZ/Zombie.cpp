#include "Zombie.h"

#include "GameElement.h"
#include "PVZDoc.h"

IMPLEMENT_DYNCREATE(Zombie, GameEntity)

extern PVZDoc* theDoc;

Zombie::Zombie(CRuntimeClass* classMsg, int hp, double moveSpeed, int h)
  : GameEntity(classMsg, hp), speed(moveSpeed), hurt(h), healthPoint(hp) {
  // 初始化碰撞信息
}

void Zombie::draw(HDC hDC, int xOffset, int yOffset) {
  std::string className(classMsg->m_lpszClassName);
  auto& rc = rcManage.getResource(className, getMapState());

  if (isState(Zombie::HURT)) {
    cleanState(Zombie::HURT);
    CImage img;
  }
  else {
    rc[animateTick]->Draw(hDC, (int)(leftX), (int)(topY));
  }
}

void Zombie::update() {
  auto& yard = theDoc->getYard();
  if (isState(Zombie::MOVE)) {
    leftX -= yard.getWidth() * speed;
  }
  auto& plants = theDoc->getYard().getPlantMatrix();
  if (!targetPlant) {
    theDoc->getYard().foreach(plants, [&](Yard::plant_iter& plantIter, int) {
      if (*plantIter && collisio(**plantIter)) {
        targetPlant = *plantIter;
        setMapState(Zombie::EatDynamic);
        setState(Zombie::EAT);
        animateTick = 0;
      }
      });
  }

  collisioArea =
    CRect((int)(getLeftX() + getWidth() * 0.45), (int)(getTopY() + getWidth() * 0.25),
      (int)(getLeftX() + getWidth() * 0.6), (int)(getTopY() + getHeight() * 0.75));
}

void Zombie::stateSwitch(bool isHalf) {
  if (isState(Zombie::EAT)) {
    attack();
  }
}

void Zombie::attack() {
  // 伤害植物
  if (targetPlant) targetPlant->damage(hurt);
  if (targetPlant->getHp() <= 0) {
    targetPlant.reset();
    setMapState(Zombie::MoveDynamic);
    setState(Zombie::MOVE);
    animateTick = 0;
  }
}