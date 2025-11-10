#include "PVZDoc.h"

#include "Chomper.h"
#include "PeaShooterSingle.h"
#include "ProjectilePea.h"
#include "Seed.h"
#include "SnowPeaShootter.h"
#include "Squash.h"
#include "SunFlower.h"
#include "Wallnut.h"

IMPLEMENT_DYNCREATE(PVZDoc, CDocument)

PVZDoc::PVZDoc() : yard(), sbank() {
  sbank.setLeftX(yard.getWidth() / 6);
  sbank.setTopY(0);
  // Ìí¼ÓÖÖ×Ó
  std::vector<Seed>& seedElements = sbank.getSeedElements();
  seedElements.emplace_back(RUNTIME_CLASS(SunFlower), SunFlower::PRICE, SunFlower::PLANT_TIME);
  seedElements.emplace_back(RUNTIME_CLASS(PeaShooterSingle), PeaShooterSingle::PRICE,
    SunFlower::PLANT_TIME);
  seedElements.emplace_back(RUNTIME_CLASS(SnowPeaShooter), SnowPeaShooter::PRICE,
    SunFlower::PLANT_TIME);
  seedElements.emplace_back(RUNTIME_CLASS(Wallnut), Wallnut::PRICE,
    SunFlower::PLANT_TIME);
  seedElements.emplace_back(RUNTIME_CLASS(Chomper), Chomper::PRICE, SunFlower::PLANT_TIME);
  seedElements.emplace_back(RUNTIME_CLASS(Squash), 0, SunFlower::PLANT_TIME);
  sbank.orderSeed();
}

Yard& PVZDoc::getYard() { return yard; }

SeedBank& PVZDoc::getSeedBank() { return sbank; }

Player& PVZDoc::getPlayer() { return player; }

PVZDoc::SunlightList& PVZDoc::getSunList() { return sunList; }