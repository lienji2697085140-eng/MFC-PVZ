#include "PVZDoc.h"
#include "SunFlower.h"
#include "Sun.h"

IMPLEMENT_DYNCREATE(SunFlower, Plant)

SunFlower::SunFlower() : 
    Plant(RUNTIME_CLASS(SunFlower), SunFlower::HP, SunFlower::SKILL_MAX_TIME) { }

void SunFlower::draw(HDC hDC) { 
    Plant::draw(hDC);
}

bool SunFlower::skillCheck() {
    if (skillWaitTime >= skillMaxWaitTime) {
        return true;
    }
    return false;
}

void SunFlower::skill(PVZDoc& doc) {
    /*auto &sunList = doc.getSunList();
    auto sun = std::make_shared<Sun>(leftX, topY - 50, POINT{ (int)leftX, (int)topY + rand() % 10 }, 1000);
    sun->addState(Sun::JUMP);
    sunList.push_back(sun);
    setMapState(SunFlower::DynamicBright);*/
}
