#pragma once
#include "stdafx.h"
#include "Plant.h"

class Wallnut : public Plant {
  DECLARE_DYNCREATE(Wallnut)

public:
  enum MapStatus { FristDynamic = 1, SecondDynamic, ThirdDynamic };

public:
  // 植物基本属性
  static constexpr int HP = 3000;
  static constexpr int PLANT_TIME = 200;
  static constexpr int PRICE = 50;

public:
  Wallnut();

public:
  virtual void stateSwitch(bool isHalf);

  virtual void draw(HDC hDC, int xOffset = 0, int yOffset = 0);

  virtual void update();
};
