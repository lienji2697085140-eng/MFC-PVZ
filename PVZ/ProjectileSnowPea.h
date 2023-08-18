#pragma once
#include "ProjectilePea.h"
class ProjectileSnowPea : public ProjectilePea
{
    DECLARE_DYNCREATE(ProjectileSnowPea)

public:
    // ÒÆ¶¯ËÙ¶È
    static constexpr int step = 3;

public:

    ProjectileSnowPea();
};

