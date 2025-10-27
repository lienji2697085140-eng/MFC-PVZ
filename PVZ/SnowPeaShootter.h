#pragma once
#include "stdafx.h"
#include "PeaShooterSingle.h"
#include "ProjectileSnowPea.h"

class SnowPeaShooter : public PeaShooterSingle {
  DECLARE_DYNCREATE(SnowPeaShooter)
public:

  // ������ͼ״̬
  enum MapStatus { Dynamic = 1, DynamicLaunch };

public:
  static constexpr int HP = 300;
  static constexpr int PLANTTIME = 150;
  static constexpr int PRICE = 175;
  // static constexpr int skillMaxTime = 50;

public:
  SnowPeaShooter();
};
