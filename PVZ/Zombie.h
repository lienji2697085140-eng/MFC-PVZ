#pragma once
#include "stdafx.h"
#include "GameElement.h"
#include "PVZDoc.h"

class Zombie : public GameEntity {
  DECLARE_DYNCREATE(Zombie)

public:
  enum State { STAND = 0x10, MOVE = 0x100, EAT = 0x1000, HURT = 0x10000 };

  enum MapState { StandDynamic = 0, MoveDynamic, EatDynamic };

private:
  double speed;
  int healthPoint;
  int hurt;

  std::shared_ptr<Plant> targetPlant{ nullptr };

public:
  Zombie(CRuntimeClass* classMsg = nullptr, int hp = 0, double speed = 0,
    int hurt = 0);

  // 绘制函数和功能函数
  virtual void draw(HDC, int xOffset = 0, int yOffset = 0);

  void update();

  void setSpeed(double s) { speed = s; }

  void setTargetPlant(std::shared_ptr<Plant> p) { targetPlant = p; }

  void stateSwitch(bool isHalf);

  void attack();
};
