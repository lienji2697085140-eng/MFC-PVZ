#pragma once
#include "GameEntity.h"

class Sun : public GameElement {
  DECLARE_DYNCREATE(Sun)

 public:
  enum { Flash = 1, NoFlash };

  enum {
    SLOWDOWN = 0b1,
    JUMP = 0b10,
    COLLECTED = 0b100,
    NOFLASH = 0b1000,
    FLASH = 0b10000
  };

  const static int LIFE_TIME = 400;

 private:
  int sunValue{25};
  int lifeTime;
  CPoint finalPoint{0};

 public:
  double jumpParam = 1;
  double xStep = 0;
  double yStep = 0;
  int distance = 0;
  int moveSpeed = 0;
  bool moveFlg = true;

 public:
  Sun();

  void draw(HDC, int xOffset = 0, int yOffset = 0);

  void update();

  int getSunValue();

  bool onClick(int cx, int cy);

  int getLifeTime() { return lifeTime; }

  void setFinalPoint(CPoint fp) { finalPoint = fp; }

  POINT getMoveParam();
  void setMoveParam(POINT);

  void setMoveSlope(double slope);
  double getMoveSlope();
};
