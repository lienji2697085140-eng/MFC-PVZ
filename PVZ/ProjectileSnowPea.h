#pragma once
#include "stdafx.h"
#include "ProjectilePea.h"

class ProjectileSnowPea : public ProjectilePea
{
  DECLARE_DYNCREATE(ProjectileSnowPea)

public:
  // ŇĆśŻËŮśČ
  static constexpr int step = 3;
  static constexpr int hurt = 50;

public:

  ProjectileSnowPea();
};
