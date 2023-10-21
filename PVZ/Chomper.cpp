#include "Chomper.h"

#include "PVZDoc.h"
#include "Zombie.h"

IMPLEMENT_DYNCREATE(Chomper, Plant)

extern PVZDoc* theDoc;

Chomper::Chomper()
    : Plant(RUNTIME_CLASS(Chomper), Chomper::HP, Chomper::SKILL_MAX_TIME) {
  this->skillWaitTime = SKILL_MAX_TIME;
}

void Chomper::draw(HDC hDC, int xOffset, int yOffset) {
  Plant::draw(hDC, -(width * 0.05), -(height * 0.4));
}

bool Chomper::skillCheck() {
  // 处理吃僵尸的逻辑
  auto& rowZombie = theDoc->getYard().getZombieList()[plantPos.x];
  for (auto& zombie : rowZombie) {
    if (collisio(*zombie)) {
      // chewTarget = *zombie;
      chewTarget = zombie;
      return true;
    }
  }
  return false;
}

void Chomper::update() {
  Plant::update();
  if (isState(Plant::SKILL_SLEEP)) {
    ++chewTime;
    if (chewTime == CHEW_MAX_TIME) {
      setState(Plant::SKILL_OVER);
    }
  }
}

void Chomper::skill() {
  if (chewTarget) {
    chewTarget->damage(500);
    chewTarget = nullptr;
  }
}

void Chomper::setPosition() {
  collisioArea = CRect(leftX + width * 0.2, topY, leftX + width * 0.6,
                       topY + height * 0.5);
}

bool Chomper::collisio(const GameEntity& e) {
  CRect tgCheckArea{e.getcollisioArea()}, thisArea{collisioArea};
  return !(thisArea.top >= tgCheckArea.bottom ||
           thisArea.bottom <= tgCheckArea.top ||
           thisArea.left >= tgCheckArea.right ||
           thisArea.right + getWidth() * 0.2 <= tgCheckArea.left);
}

void Chomper::stateSwitch(bool isHalf) {
  if (isState(Plant::DEFAULT) && !isHalf) {
    setMapState(Chomper::DefaultDynamic);
    cleanState(Plant::DEFAULT);
    setState(0x0);
  }

  if (isState(Plant::SKILL_OVER) && !isHalf) {
    setMapState(Chomper::SwallowDynamic);
    setState(Plant::DEFAULT);
  }

  if (isState(Plant::SKILL)) {
    if (isHalf) {
      skill();  // 释放技能
      return;
    }
    skill();
    // 进入休眠状态
    chewTime = 0;
    setMapState(Chomper::ChewDynamic);
    setState(Plant::SKILL_SLEEP);
    return;
  }

  if (isState(Plant::SKILL_REDAY) && !isHalf) {
    // 进入就绪状态后首先切换贴图
    setMapState(Plant::SkillDynamic);
    setState(Plant::SKILL);
    return;
  }
}