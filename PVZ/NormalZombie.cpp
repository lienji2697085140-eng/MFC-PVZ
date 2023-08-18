#include "NormalZombie.h"

IMPLEMENT_DYNCREATE(NormalZombie, Zombie)

NormalZombie::NormalZombie(CRuntimeClass* classMsg, int hp, double moveSpeed, int hurt) : 
    Zombie(classMsg, hp, moveSpeed, hurt) {
}
