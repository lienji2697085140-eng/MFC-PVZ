#include "GameElement.h"

IMPLEMENT_DYNCREATE(GameElement, Visible)

GameElement::GameElement(CRuntimeClass* msg) : Visible(msg) {}

bool GameElement::onClick(int cx, int cy) {
  if (cx >= leftX && cx <= leftX + width && cy >= topY && cy <= topY + height) {
    return true;
  }
  return false;
}

void GameElement::stateSwitch() {}