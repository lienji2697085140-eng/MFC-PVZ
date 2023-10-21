#pragma once
#include <afxwin.h>
#include <memory>
#include <list>
#include <string>

class Yard;

class SeedBank;

class Sun;

class Plant;

class Player {
private:
    CPoint pos;
    int sun;
    std::shared_ptr<Plant> currentPlant;

public:

    // 标记点的位置
    CPoint& getPos();
    void setPos(CPoint);

    // 当前选择的植物
    void setCurrentPlant(std::shared_ptr<Plant>);
    std::shared_ptr<Plant> getCurrentPlant();
    
    // 选择植物 放置植物
    void selectPlant(Yard&, SeedBank&, CPoint&);
    void placementPlant(Yard&, SeedBank& ,int, int);
    void plantOver(SeedBank& sbank, CRuntimeClass* theClass, size_t state);
    void collectSun(Yard&, SeedBank&, std::list<std::shared_ptr<Sun>>&, CPoint&);
    void drawCurrentPlant(HDC hDC, Yard&);
};

