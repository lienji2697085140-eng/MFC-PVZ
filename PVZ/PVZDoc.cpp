#include "PVZDoc.h"
#include "SunFlower.h"
#include "PeaShooterSingle.h"
#include "SnowPeaShootter.h"
#include "Seed.h"
#include "ProjectilePea.h"

IMPLEMENT_DYNCREATE(PVZDoc, CDocument)

PVZDoc::PVZDoc(): yard(), sbank()  {
    sbank.setLeftX(yard.getWidth() / 6);
    sbank.setTopY(0);
    // Ìí¼ÓÖÖ×Ó
    std::vector<Seed> &seedElements = sbank.getSeedElements();
    seedElements.emplace_back(RUNTIME_CLASS(SunFlower), 
                              SunFlower::PRICE - 100, 
                              SunFlower::PLANT_TIME);
    seedElements.emplace_back(RUNTIME_CLASS(PeaShooterSingle), 
                              PeaShooterSingle::PRICE - 100, 
                              SunFlower::PLANT_TIME);
    seedElements.emplace_back(RUNTIME_CLASS(SnowPeaShooter), 
                              SnowPeaShooter::PRICE - 200, 
                              SunFlower::PLANT_TIME);

    sbank.orderSeed();
}

Yard& PVZDoc::getYard() {
    return yard;
}

SeedBank& PVZDoc::getSeedBank() {
    return sbank;
}

Player& PVZDoc::getPlayer() {
    return player;
}

PVZDoc::SunlightList& PVZDoc::getSunList() {
    return sunList;
}


