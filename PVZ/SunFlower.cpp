#include "SunFlower.h"

#include "PVZDoc.h"
#include "Sun.h"

IMPLEMENT_DYNCREATE(SunFlower, Plant)

extern PVZDoc* theDoc;

SunFlower::SunFlower()
    : Plant(RUNTIME_CLASS(SunFlower), SunFlower::HP,
            SunFlower::SKILL_MAX_TIME) {}

void SunFlower::draw(HDC hDC, int xOffset, int yOffset) {
  Plant::draw(hDC);
}

bool SunFlower::skillCheck() {
  if (skillWaitTime >= skillMaxWaitTime) {
    return true;
  }
  return false;
}

void SunFlower::skill() {
  auto& sunList = theDoc->getSunList();
  // auto sun = std::make_shared<Sun>(leftX, topY - 50, POINT{ (int)leftX,
  // (int)topY + rand() % 10 }, 1000);
  auto sun = std::make_shared<Sun>();
  sun->setLeftX(leftX);
  sun->setTopY(topY - 50);
  sun->setFinalPoint({(int)leftX, (int)topY + rand() % 10});
  sun->addState(Sun::JUMP);
  sun->setMapState(Sun::Flash);
  sunList.push_back(sun);
}

void SunFlower::stateSwitch(bool isHalf) { Plant::stateSwitch(isHalf); }