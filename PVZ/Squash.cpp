#include "Squash.h"
#include "PVZDoc.h"
#include "Zombie.h"

IMPLEMENT_DYNCREATE(Squash, Plant)

extern PVZDoc* theDoc;

Squash::Squash()
    : Plant(RUNTIME_CLASS(Squash), Squash::HP) {}

void Squash::draw(HDC hDC, int xOffset, int yOffset) {
  Plant::draw(hDC, 0, -(height * 0.2));
}

void Squash::update() {
  Plant::update();
  if (isState(Squash::JUMP)) {
    int val = sin(jumpParam += 0.15) * (width * 0.15);
    topY -= val;
    leftX += 4;
    if (jumpParam > 5 /*val < - 6*/) {
      cleanState(Squash::JUMP);
      jumpParam = 1;
    }
  }
}

bool Squash::collisio(const GameEntity& e) {
  CRect tgCheckArea{e.getcollisioArea()}, thisArea{collisioArea};
  return !(thisArea.top >= tgCheckArea.bottom ||
           thisArea.bottom <= tgCheckArea.top ||
           thisArea.left - getWidth() * 0.5 >= tgCheckArea.right ||
           thisArea.right + getWidth() * 0.5 <= tgCheckArea.left);
}

bool Squash::skillCheck() {
  // 处理吃僵尸的逻辑
  auto& rowZombie = theDoc->getYard().getZombieList()[plantPos.x];
  for (auto& zombie : rowZombie) {
    if (collisio(*zombie)) {
      if (getcollisioArea().right <= zombie->getcollisioArea().left) {
        direction = Right;
      } else if (getcollisioArea().left >= zombie->getcollisioArea().right) {
        direction = Left;
      } else {
        direction = Mid;
      }
      CPoint p1(leftX, topY), p2(zombie->getLeftX(), topY),
          p3((leftX + zombie->getLeftX()) / 2,
               topY + getWidth() * 0.5);
      /*a = (p3.x * (p2.y - p1.y) - p2.x * (p3.y - p1.y) + p1.x * (p3.y - p2.y)) /
          ((p2.x - p1.x) * (p3.x - p1.x) * (p3.x - p2.x));
      b = (p2.y - p1.y - a * (p2.x * p2.x - p1.x ^ p2.x)) / (p2.x - p1.x);
      c = p1.y - a * p1.x * p1.x - b * p1.x;*/

      return true;
    }
  }
  return false;
}

void Squash::stateSwitch(bool isHalf) {
  if (isState(Plant::SKILL)) {
    setMapState(Squash::JumpDynamic);
    if (isHalf) {
      skill();  // 释放技能
      return;
    }
    setState(Squash::JUMP);
    //skill();
    //// 进入休眠状态
    //setMapState(Chomper::ChewDynamic);
    //setState(Plant::SKILL_SLEEP);
    return;
  }

  if (isState(Plant::SKILL_REDAY) && !isHalf) {
    // 进入就绪状态后首先切换贴图
    setMapState(Plant::SkillDynamic);
    if (direction == Right) {
      setMapState(Squash::LookRDynamic);
      
    } else if (direction == Right) {
      setMapState(Squash::LookLDynamic);
    }
    setState(Plant::SKILL);
    return;
  }
}
