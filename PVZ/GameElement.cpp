#include "GameElement.h"

IMPLEMENT_DYNCREATE(GameElement, Visible)

GameElement::GameElement(CRuntimeClass* msg, int hp) :
    Visible(msg), healthPoint(hp) {

}

void GameElement::draw(HDC hDC) {
    ::Rectangle(hDC, collisioArea.left, collisioArea.top,
        collisioArea.right, collisioArea.bottom);
    CString str;
    str.Format("Hp:%d", healthPoint);
    ::TextOut(hDC, leftX + getWidth() * 0.3, topY, str, str.GetLength());
}

bool GameElement::collisio(const GameElement& e) {
    // Åö×²¼ì²â
    CRect tgCheckArea { e.collisioArea },
          thisArea{ collisioArea };
    if (thisArea.right > tgCheckArea.left &&
        thisArea.left < tgCheckArea.right) {
        return true;
    }
    return false;
}

bool GameElement::onClick(int cx, int cy) {
    if (cx >= leftX && cx <= leftX + width
        && cy >= topY && cy <= topY + height) {
        return true;
    }
    return false;
}

void GameElement::stateSwitch() {

}