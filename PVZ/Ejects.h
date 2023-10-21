#pragma once
#include "GameEntity.h"

class Ejects : public GameEntity {
    DECLARE_DYNCREATE(Ejects)

public:
    enum State { DEFAULT = 0x1, BREAK = 0x10, DEL = 0x100 };
    enum MapState { ImgBreak = 1 };

public:
    int hurt;

public:
    Ejects(CRuntimeClass* classMsg = RUNTIME_CLASS(Ejects), int hurt = 0);

public:
    // »æÖÆ
    virtual void draw(HDC, int xOffset = 0, int yOffset = 0);

    void update();

    virtual void move();

    int getHurt() { return hurt; }

    void setHurt(int h) { hurt = h; }
};

