#include "GameEntity.h"

IMPLEMENT_DYNCREATE(GameEntity, GameElement)

GameEntity::GameEntity(CRuntimeClass* msg, int hp)
  : GameElement(msg), healthPoint(hp) {}

void GameEntity::draw(HDC hDC, int xOffset, int yOffset) {
  ::Rectangle(hDC, collisioArea.left, collisioArea.top, collisioArea.right,
    collisioArea.bottom);
  CString str;
  str.Format("Hp:%d", healthPoint);
  ::TextOut(hDC, (int)(leftX + getWidth() * 0.3), (int)topY, str, str.GetLength());
}

bool GameEntity::collisio(const GameEntity& e) {
  // Åö×²¼ì²â
  CRect tgCheckArea{ e.collisioArea }, thisArea{ collisioArea };
  return !(thisArea.top >= tgCheckArea.bottom ||
    thisArea.bottom <= tgCheckArea.top ||
    thisArea.left >= tgCheckArea.right ||
    thisArea.right <= tgCheckArea.left);
}