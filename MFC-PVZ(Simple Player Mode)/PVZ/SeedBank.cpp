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
  dock[0]->AlphaBlend(hDC, (int)(leftX + SLIDING_WINDOW), (int)(topY));
  for (auto& seed : seedElements) {
    seed.draw(hDC);
  }
  // 显示阳光量
  ::SetBkMode(hDC, TRANSPARENT);
  CRect textArea((int)(leftX), (int)(topY) + (int)(height * 0.7), (int)(leftX) + (int)(width * 0.18),
    (int)(topY + height));
  ::DrawText(hDC, sunlightStr, sunlightStr.GetLength(), &textArea,
    DT_SINGLELINE | DT_CENTER | DT_VCENTER);
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