#include "Wallnut.h"

#include "PVZDoc.h"

IMPLEMENT_DYNCREATE(Wallnut, Plant)

extern PVZDoc* theDoc;

Wallnut::Wallnut()
    : Plant(RUNTIME_CLASS(Wallnut), Wallnut::HP) {}

void Wallnut::draw(HDC hDC, int xOffset, int yOffset) {
  Plant::draw(hDC);
}

void Wallnut::stateSwitch(bool isHalf) { Plant::stateSwitch(isHalf); }

void Wallnut::update() {
  if (this->getHp() < 1000) {
    setMapState(Wallnut::ThirdDynamic);
  } else if (this->getHp() < 2000) {
    setMapState(Wallnut::SecondDynamic);
  } else {
    setMapState(Wallnut::FristDynamic);
  }
  Plant::update();
}