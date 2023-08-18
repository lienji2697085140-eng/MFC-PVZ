#pragma once
//#include "Visible.h"
//#include "Seed.h"
#include "GameElement.h"
#include <functional>

class Seed;

class SeedBank : public GameElement {

    DECLARE_DYNCREATE(SeedBank)

public:

    SeedBank();

    std::vector<Seed>& getSeedElements();

    void updateAllSeedState();

    int getSunlight();

    void setSunlight(int);

    void orderSeed();

    // 利用可调用对象遍历列表
    void foreachAllSeed(std::function<void(Seed&)>, CRuntimeClass* thePlant);

    virtual void draw(HDC);

private:
    std::vector<Seed> seedElements;
    
    int sunlight;
};

