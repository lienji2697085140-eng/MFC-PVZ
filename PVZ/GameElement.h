#pragma once
#include "Visible.h"

class GameElement : public Visible {

DECLARE_DYNCREATE_(GameElement)

protected:
    size_t currentState = 0x0;
    CRect collisioArea { 0 };
    int healthPoint;

public:

    GameElement(CRuntimeClass* msg = nullptr, int hp = 0);

    // ¶ÔÓ¦×´Ì¬
    size_t getCurrentState() { return currentState; }
    void setState(size_t state) { currentState = state; }
    void addState(size_t state) { currentState |= state; }
    void cleanState(size_t state) { if (isState(state)) currentState ^= state; }
    bool isState(size_t state) { return (currentState & state) == state; }
    bool isOnlyState(size_t state) { return currentState == state; }

    // Åö×²¼ì²â
    virtual bool collisio(const GameElement&);

    // µã»÷¼ì²â
    virtual bool onClick(int cx, int cy);

    virtual void draw(HDC hDC);

    virtual void stateSwitch();

    int getHp() { return healthPoint; }

    void damage(int h) { healthPoint -= h; };
};

