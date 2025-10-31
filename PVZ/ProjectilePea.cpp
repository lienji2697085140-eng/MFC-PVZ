#include "ProjectilePea.h"

IMPLEMENT_DYNCREATE(ProjectilePea, Ejects)

ProjectilePea::ProjectilePea(CRuntimeClass* msg, int hurt)
    : Ejects(msg, hurt), penetrationCount(3), hitCount(0) {
}

void ProjectilePea::move() {
    leftX += ProjectilePea::step;
}
