#pragma once
#include "stdafx.h"
#include "GameEntity.h"

class Seed;

class PVZDoc;

extern PVZDoc* theDoc;

class Plant : public GameEntity {
  DECLARE_DYNCREATE(Plant)

    enum MapState { DefaultDynamic = 1, SkillDynamic };

  enum State {
    DEFAULT = 0x10,
    SKILL = 0x100,
    VIRTUAL = 0x1000,
    SKILL_REDAY = 0x10000,
    SKILL_SLEEP = 0x100000,
    SKILL_OVER = 0x1000000
  };

  friend class Seed;

protected:
  int skillMaxWaitTime;   // 重置时间
  int skillWaitTime = 0;  // 技能的冷却时间 为-1代表不启用技能
  CPoint plantPos{ 0 };

public:
  Plant(CRuntimeClass* classMsg = nullptr, int hp = 0, int skillMaxTime = -1);

  CPoint getPlantPos() const { return plantPos; }
  void setPlantPos(CPoint pos) { plantPos = pos; }

  // 绘制函数和功能函数
  virtual void draw(HDC, int xOffset = 0, int yOffset = 0);

  virtual void skill();

  virtual void update();

  virtual bool skillCheck();

  virtual void stateSwitch(bool isHalf);

  virtual void setPosition();
};
