#pragma once
#include "stdafx.h"
#include "Ejects.h"

class ProjectilePea : public Ejects {
  DECLARE_DYNCREATE(ProjectilePea)

public:
  // ÒÆ¶¯ËÙ¶È
  static constexpr int step = 7;
  static constexpr int hurt = 20;

public:
  ProjectilePea(CRuntimeClass* msg = RUNTIME_CLASS(ProjectilePea),
    int h = ProjectilePea::hurt);

  virtual void move();
};
