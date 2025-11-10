#include "Squash.h"
#include "PVZDoc.h"
#include "Zombie.h"

IMPLEMENT_DYNCREATE(Squash, Plant)

extern PVZDoc* theDoc;

Squash::Squash()
  : Plant(RUNTIME_CLASS(Squash), Squash::HP) {}

void Squash::draw(HDC hDC, int xOffset, int yOffset) {
  Plant::draw(hDC, -(int)(width * 0.1), -(int)(height * 0.2));
}

void Squash::setPosition() {
  collisioArea =
    CRect((int)(leftX + width * 0.2), 
          (int)(topY), 
          (int)(leftX + width * 0.65), 
          (int)(topY + height * 0.2));
}

void Squash::skill() {
  auto& rowZombie = theDoc->getYard().getZombieList()[plantPos.x];
  for (auto& zombie : rowZombie) {
    if (collisio(*zombie)) {
      zombie->damage(500);
    }
  }
}

void Squash::update() {
  Plant::update();
  if (isState(Squash::JUMP)) {
    int val = (int)(sin(jumpParam += 0.2) * (width * 0.17));
    topY -= val;
    leftX += jumpStep;
    if (jumpParam > 5) {
      cleanState(Squash::JUMP);
      skill();
      this->healthPoint = 0;
      jumpParam = 1;
    }
  }
}

bool Squash::collisio(const GameEntity& e) {
  CRect tgCheckArea{ e.getcollisioArea() }, thisArea{ collisioArea };
  return !(thisArea.top >= tgCheckArea.bottom ||
    thisArea.bottom <= tgCheckArea.top ||
    thisArea.left - getWidth() * 0.5 >= tgCheckArea.right ||
    thisArea.right + getWidth() * 0.5 <= tgCheckArea.left);
}

bool Squash::skillCheck() {
  // 处理吃僵尸的逻辑
  auto& rowZombie = theDoc->getYard().getZombieList()[plantPos.x];
  for (auto& zombie : rowZombie) {
    if (collisio(*zombie) && !isState(Squash::JUMP)) {
      CRect zombieRect = zombie->getcollisioArea();
      int plantPos = (collisioArea.left + collisioArea.right) / 2;
      int zombiePos = (zombieRect.left + zombieRect.right) / 2;

      jumpStep = (zombiePos - plantPos) / 20;

      if (plantPos < zombiePos) {
        direction = Right;
      }
      else if (plantPos > zombiePos) {
        direction = Left;
      }
      else {
        direction = Mid;
      }
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
      // skill();  // 释放技能
      return;
    }
    setState(Squash::JUMP);
    return;
  }

  if (isState(Plant::SKILL_REDAY) && !isHalf) {
    // 进入就绪状态后首先切换贴图
    setMapState(Plant::SkillDynamic);
    if (direction == Right) {
      setMapState(Squash::LookRDynamic);
    }
    else if (direction == Right) {
      setMapState(Squash::LookLDynamic);
    }
    setState(Plant::SKILL);
    return;
  }
}