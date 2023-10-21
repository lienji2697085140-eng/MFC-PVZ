#pragma once
#include "Plant.h"

class SunFlower : public Plant {
  DECLARE_DYNCREATE(SunFlower)

 public:
  enum MapStatus { Dynamic = 1, DynamicBright };

 public:
  // 植物基本属性
  static constexpr int HP = 300;
  static constexpr int PLANT_TIME = 200;
  static constexpr int PRICE = 50;
  // 15秒产生一次阳光
  static constexpr int SKILL_MAX_TIME = 1500;

 public:
  SunFlower();

 public:
  virtual void stateSwitch(bool isHalf);

  virtual void draw(HDC hDC, int xOffset = 0, int yOffset = 0);

  virtual void skill();

  virtual bool skillCheck();
};
