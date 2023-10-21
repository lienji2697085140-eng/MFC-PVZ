#include "SeedBank.h"

#include "Config.h"
#include "Seed.h"

IMPLEMENT_DYNCREATE(SeedBank, GameElement)

SeedBank::SeedBank() : GameElement(RUNTIME_CLASS(SeedBank)), sunlight(0) {}

std::vector<Seed>& SeedBank::getSeedElements() { return seedElements; }

void SeedBank::updateAllSeedState() {
  for (auto& seed : seedElements) {
    seed.update(*this);
  }
}

void SeedBank::foreachAllSeed(std::function<void(Seed&)> call,
                              CRuntimeClass* thePlant) {
  for (auto& seed : seedElements) {
    if (thePlant == seed.getPlantClass()) {
      call(seed);
    }
  }
}

void SeedBank::draw(HDC hDC, int xOffset, int yOffset) {
  // 更新所有种子状态
  // updateAllSeedState();

  CString sunlightStr;
  sunlightStr.Format("%d", sunlight);
  auto& dock = rcManage.getResource("SeedBank", currentState);
  // dock[0]->BitBlt(hDC, leftX, topY, 600, 100, 0, 0);
  dock[0]->AlphaBlend(hDC, leftX + SLIDING_WINDOW, topY);
  // dock[0]->AlphaBlend(hDC, leftX + SLIDING_WINDOW, topY, dock[0]->GetWidth()
  // * 1.25, dock[0]->GetHeight() * 1.25, 0, 0, dock[0]->GetWidth(),
  // dock[0]->GetHeight());
  for (auto& seed : seedElements) {
    seed.draw(hDC);
  }
  // 显示阳光量
  ::SetBkMode(hDC, TRANSPARENT);
  // CRect textArea = { 100, 60, 180, 80 };
  CRect textArea(leftX, topY + (int)(height * 0.7), leftX + (int)(width * 0.18),
                 topY + height);
  ::DrawText(hDC, sunlightStr, sunlightStr.GetLength(), &textArea,
             DT_SINGLELINE | DT_CENTER | DT_VCENTER);
  // ::TextOut(hDC, 130, 60, sunlightStr, sunlightStr.GetLength());
}

void SeedBank::orderSeed() {
  size_t id = 0;
  for (auto& seed : seedElements) {
    seed.setState(Seed::READY);
    seed.leftX = leftX + width * 0.17 + id * width / 9;
    seed.topY = topY + height * 0.1;
    ++id;
  }
}

int SeedBank::getSunlight() { return sunlight; }

void SeedBank::setSunlight(int s) { sunlight = s; }