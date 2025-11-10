#include "Seed.h"

#include "Plant.h"
#include "SeedBank.h"
#include "Chomper.h"
#include "Squash.h"

IMPLEMENT_DYNCREATE(Seed, GameElement)

// 175 8 是偏移量
Seed::Seed(CRuntimeClass* plant, int seedPrice, int time)
  : GameElement(RUNTIME_CLASS(Seed)),
  plantClass(plant),
  price(seedPrice),
  maxWaitingTime(time) {}

Seed::Seed(const Seed& rhs)
  : GameElement(rhs.classMsg),
  plantClass(rhs.plantClass),
  price(rhs.price),
  maxWaitingTime(rhs.maxWaitingTime) {}

int Seed::getPrice() { return price; }

int Seed::getWaitingTime() { return waitingTime; }

void Seed::update(SeedBank& bank) {
  if (waitingTime) --waitingTime;
  // 解除冷却时间

  // 阳光不足 进入禁用状态
  if (bank.getSunlight() < price) addState(Seed::FORBID);
  if (bank.getSunlight() >= price) cleanState(Seed::FORBID);
  // if (waitingTime == 0) cleanState(Seed::WAIT);
}

void Seed::wait() { waitingTime = maxWaitingTime; }

void Seed::draw(HDC hDC, int xOffset, int yOffset) {
  CRuntimeClass* theClass = getPlantClass();
  auto& icons =
    rcManage.getResource(plantClass->m_lpszClassName, Visible::Static);
  auto& seedBg = rcManage.getResource("Seed", Visible::Static);
  // 绘制植物图标 边框 价格
  // seedBg[0].getImg().BitBlt(hDC, leftX, topY, 50, 70, 100, 0);
  // seedBg[0]->BitBlt(hDC, leftX, topY, 50, 70, 100, 0);
  int cardWidth = (int)(width / 9);
  int cardHeight = (int)(height);
  double sizeZoom = 0.55;
  if (getPlantClass() == RUNTIME_CLASS(Chomper)) {
    yOffset -= (int)(icons[0]->GetHeight() * 0.17);
    sizeZoom = 0.5;
  }
  if (getPlantClass() == RUNTIME_CLASS(Squash)) {
    yOffset -= (int)(icons[0]->GetHeight() * 0.07);
    sizeZoom = 0.5;
  }
  seedBg[0]->AlphaBlend(hDC, 
    (int)(leftX), (int)(topY), 
    (int)(cardWidth), (int)(height), 
    (int)(cardWidth * 2), 0,
    (int)(cardWidth), (int)(seedBg[0]->GetHeight()));
  icons[0]->Draw(hDC, (int)(leftX + cardWidth * 0.05 + xOffset),
    (int)(topY + height * 0.1 + yOffset), (int)(icons[0]->GetWidth() * sizeZoom),
    (int)(icons[0]->GetHeight() * sizeZoom));
  CString priceStr;
  priceStr.Format("%d", price);

  ::SetBkMode(hDC, TRANSPARENT);
  CRect textArea((int)(leftX), (int)(topY + cardHeight * 0.7), (int)(leftX + cardWidth),
    (int)(topY + cardHeight));
  ::DrawText(hDC, priceStr, priceStr.GetLength(), &textArea,
    DT_SINGLELINE | DT_CENTER | DT_VCENTER);

  // ::TextOut(hDC, leftX + cardHeight * 0.7, topY + 51, priceStr,
  // priceStr.GetLength());
  // ::DrawText(hDC, priceStr, )
  // 根据Seed状态绘制阴影
  if (isState(Seed::FORBID))   // 此阴影为阳光不足时触发
    seedBg[1]->AlphaBlend(hDC, (int)leftX, (int)topY, (int)cardWidth, (int)cardHeight, 0, 0,
      seedBg[1]->GetWidth(), seedBg[1]->GetHeight(), 100);
  if (isState(Seed::SELECTED)) // 此阴影为选中时触发
    seedBg[1]->AlphaBlend(hDC, (int)leftX, (int)topY, (int)cardWidth, (int)cardHeight, 0, 0,
      seedBg[1]->GetWidth(), seedBg[1]->GetHeight(), 100);
  // 此阴影为冷却时间触发
  int height = (int)(((double)waitingTime / maxWaitingTime) * cardHeight);
  seedBg[1]->AlphaBlend(hDC, (int)leftX, (int)topY, cardWidth, height, 0, 0,
    seedBg[1]->GetWidth(), seedBg[1]->GetHeight(), 100);

  // Visible::draw(hDC);
}

bool Seed::onClick(int cx, int cy) {
  if (cx >= leftX && cx <= leftX + width / 9 && cy >= topY &&
    cy <= topY + height) {
    return true;
  }
  return false;
}

CRuntimeClass* Seed::getPlantClass() { return plantClass; }