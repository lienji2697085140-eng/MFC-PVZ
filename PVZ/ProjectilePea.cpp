#include "ProjectilePea.h"

IMPLEMENT_DYNCREATE(ProjectilePea, Ejects)

ProjectilePea::ProjectilePea(CRuntimeClass* msg, int hurt) : Ejects(msg, hurt) { }

void ProjectilePea::move() {
    leftX += ProjectilePea::step;
}
