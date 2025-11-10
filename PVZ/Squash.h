#pragma once
#include "stdafx.h"
#include "Plant.h"
class Squash : public Plant {
  DECLARE_DYNCREATE(Squash)

public:
  enum MapStatus { Dynamic = 1, LookLDynamic, LookRDynamic, JumpDynamic };
  enum State {
    JUMP = 0x10000000
  };
  enum JumpDirection { Left, Right, Mid };
public:
  // 植物基本属性
  static constexpr int HP = 300;
  static constexpr int PLANT_TIME = 200;
  static constexpr int PRICE = 0;
  // 15秒产生一次阳光
  static constexpr int SKILL_MAX_TIME = -1;

public:
  Squash();

  virtual void update();

  bool skillCheck();

  bool collisio(const GameEntity& e);

  virtual void skill();

  virtual void setPosition();

  virtual void draw(HDC hDC, int xOffset, int yOffset);

  void stateSwitch(bool isHalf);

private:
  JumpDirection direction = JumpDirection::Left;
  double jumpParam = 1;
  double jumpStep = 0;
};
