#pragma once
#include "stdafx.h"
#include "Visible.h"


class GameElement : public Visible {
  DECLARE_DYNCREATE(GameElement)

protected:
  size_t currentState = 0x0;

public:
  GameElement(CRuntimeClass* msg = nullptr);

  // ¶ÔÓ¦×´Ì¬
  size_t getCurrentState() { return currentState; }
  void setState(size_t state) { currentState = state; }
  void addState(size_t state) { currentState |= state; }
  void cleanState(size_t state) {
    if (isState(state)) currentState ^= state;
  }
  bool isState(size_t state) { return (currentState & state) == state; }
  bool isOnlyState(size_t state) { return currentState == state; }

  // µã»÷¼ì²â
  virtual bool onClick(int cx, int cy);

  virtual void stateSwitch();
};
