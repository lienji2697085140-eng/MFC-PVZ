#include "ProjectileSnowPea.h"

IMPLEMENT_DYNCREATE(ProjectileSnowPea, ProjectilePea)

ProjectileSnowPea::ProjectileSnowPea()
    : ProjectilePea(RUNTIME_CLASS(ProjectileSnowPea)) {}