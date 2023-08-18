#pragma once
#include <afxwin.h>
#include "Plant.h"
#include "ProjectilePea.h"

// 动态创建机制

class PeaShooterSingle : public Plant {

    DECLARE_DYNCREATE(PeaShooterSingle)

public:

    // 设置贴图状态
    enum MapStatus { Dynamic = 1, DynamicLaunch };

public:
    static constexpr int HP = 300;
    static constexpr int PLANTTIME = 150;
    static constexpr int PRICE = 100;
    // static constexpr int SKILL_MAX_TIME = 50;

private:
    CRuntimeClass* ejectClass;

public:

    PeaShooterSingle(CRuntimeClass* classMsg = RUNTIME_CLASS(PeaShooterSingle), 
                     CRuntimeClass* eject = RUNTIME_CLASS(ProjectilePea), int hp = HP);

public:
    virtual void draw(HDC hDC);

    virtual void skill();

    virtual bool skillCheck();

    virtual void stateSwitch(bool isHalf);
};

