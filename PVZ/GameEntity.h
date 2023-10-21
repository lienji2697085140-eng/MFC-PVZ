#pragma once
#include "GameElement.h"

class GameEntity : public GameElement {
  DECLARE_DYNCREATE(GameEntity)

 public:
  GameEntity(CRuntimeClass* msg = nullptr, int hp = 0);

 protected:
  int healthPoint;
  CRect collisioArea{0};

 public:
  virtual bool collisio(const GameEntity& e);

  int getHp() { return healthPoint; }

  void damage(int hurt) { healthPoint -= hurt; }

  virtual void draw(HDC hDC, int xOffset = 0, int yOffset = 0);

  CRect getcollisioArea() const { return collisioArea; }

  // int getRow() { topY }
};