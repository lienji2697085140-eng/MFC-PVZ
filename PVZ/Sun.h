#pragma once
#include "GameElement.h"

class Sun : public GameElement {

    DECLARE_DYNCREATE(Sun)

public:

    enum { Flash = 1, NoFlash };

    enum { 
        SLOWDOWN = 0b1,
        JUMP = 0b10, 
        COLLECTED = 0b100,
        NOFLASH = 0b1000,
        FLASH = 0b10000
    };
private:
    int sunValue{ 25 };
    POINT finalPoint{ 0 };

public:
    double jumpParam = 1;
    double slope = 0;
    double intercept = 0;
    bool moveFlg = true;

public:

    Sun();

    void draw(HDC);

    //int getLifeTime();

    //void setLiftTime(int left);

    void update();

    int getSunValue();

    bool onClick(int cx, int cy);

    POINT getMoveParam();
    void setMoveParam(POINT);

    void setMoveSlope(double slope);
    double getMoveSlope();
};

