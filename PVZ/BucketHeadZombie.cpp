#include "BucketHeadZombie.h"

IMPLEMENT_DYNCREATE(BucketHeadZombie, Zombie)

BucketHeadZombie::BucketHeadZombie(CRuntimeClass* classMsg, int hp, double moveSpeed, int hurt)
    : Zombie(classMsg, hp, moveSpeed, hurt) {
}