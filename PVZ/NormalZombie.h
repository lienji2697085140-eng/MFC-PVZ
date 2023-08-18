#pragma once
#include "Zombie.h"

class NormalZombie : public Zombie {
    
    DECLARE_DYNCREATE(NormalZombie)

private:

    static constexpr int HP = 181;
    static constexpr double MOVESPEED = 0.0002;
    static constexpr int HURT = 20;

public:
    NormalZombie(CRuntimeClass* classMsg = RUNTIME_CLASS(NormalZombie), 
                 int hp = HP, 
                 double moveSpeed = MOVESPEED, 
                 int hurt = HURT);
};

