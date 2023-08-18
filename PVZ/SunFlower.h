#pragma once
#include "Plant.h"

class SunFlower : public Plant {

    DECLARE_DYNCREATE(SunFlower)

public:

    enum MapStatus { Dynamic = 1, DynamicBright };

public:
    // 植物基本属性
    static constexpr int HP = 300;
    static constexpr int PLANT_TIME = 200;
    static constexpr int PRICE = 50;
    static constexpr int SKILL_MAX_TIME = 200;

public:

    SunFlower();

public:
    virtual void draw(HDC hDC);

    virtual void skill(PVZDoc&);

    virtual bool skillCheck();
};

