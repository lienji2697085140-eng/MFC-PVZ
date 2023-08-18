#include "SnowPeaShootter.h"

IMPLEMENT_DYNCREATE(SnowPeaShooter, PeaShooterSingle)

SnowPeaShooter::SnowPeaShooter() : 
    PeaShooterSingle(RUNTIME_CLASS(SnowPeaShooter), RUNTIME_CLASS(ProjectileSnowPea), HP) {
}
