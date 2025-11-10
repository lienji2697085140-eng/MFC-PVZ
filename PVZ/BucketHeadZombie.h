#pragma once
#include "stdafx.h"
#include "Zombie.h"

class BucketHeadZombie : public Zombie {
    DECLARE_DYNCREATE(BucketHeadZombie)

private:
    static constexpr int HP = 800;  // 铁桶僵尸有更高的生命值
    static constexpr double MOVESPEED = 0.0003;  // 比普通僵尸稍快
    static constexpr int HURT = 60;  // 伤害值与普通僵尸相同

public:
    BucketHeadZombie(CRuntimeClass* classMsg = RUNTIME_CLASS(BucketHeadZombie),
        int hp = HP,
        double moveSpeed = MOVESPEED,
        int hurt = HURT);
};