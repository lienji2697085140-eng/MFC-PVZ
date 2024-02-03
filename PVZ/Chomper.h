#pragma once
#include "stdafx.h"
#include "Plant.h"
#include "Yard.h"

class Chomper : public Plant {
  DECLARE_DYNCREATE(Chomper)

public:
  enum MapStatus { Dynamic = 1, EatDynamic, ChewDynamic, SwallowDynamic };

public:
  // 植物基本属性
  static constexpr int HP = 300;
  static constexpr int PLANT_TIME = 200;
  static constexpr int PRICE = 50;
  // 咀嚼多少秒
  static constexpr int SKILL_MAX_TIME = -1;
  static constexpr int CHEW_MAX_TIME = 1000;

public:
  Chomper();

public:
  virtual void stateSwitch(bool isHalf);

  virtual void draw(HDC hDC, int xOffset = 0, int yOffset = 0);

  virtual void skill();

  virtual void update();

  virtual bool skillCheck();

  virtual void setPosition();

  virtual bool collisio(const GameEntity&);

private:
  Yard::zombie_element chewTarget{};
  int chewTime{ 0 };
};
