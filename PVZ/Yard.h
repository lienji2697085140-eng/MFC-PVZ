#pragma once
#include <memory>
#include <array>
#include <list>
#include <functional>
#include "GameElement.h"
#include "Visible.h"
#include "Ejects.h"
#include "Plant.h"

class Zombie;

class Yard : public GameElement {

    DECLARE_DYNCREATE(Yard)

public:
    // 状态
    enum MapState { ImgNoon, ImgNight };
    // enum State { NOON = 0b0, NIGHT = 0b1 };

public:
    using plant_element = std::shared_ptr<Plant>;
    using ejects_element = std::shared_ptr<Ejects>;
    using zombie_element = std::shared_ptr<Zombie>;
    using plants = std::array<std::array<plant_element, 9>, 5>;
    using ejects = std::array<std::list<ejects_element>, 5>;
    using zombies = std::array<std::list<zombie_element>, 5>;
    using zombie_iter = std::list<zombie_element>::iterator;
    using ejects_iter = std::list<ejects_element>::iterator;
    using plant_iter = std::array<plant_element, 9>::iterator;

public:

    Yard();

    plants& getPlantMatrix();

    ejects& getEjectList();

    zombies& getZombieList();

    // yard的绘制函数
    virtual void draw(HDC, int xOffset = 0, int yOffset = 0);

    bool onClick(int cx, int cy);

    POINT getPlantPos(int cx, int cy);

    bool isMovePlantArea(int cx, int cy);

    int getPlantWidth() { return (int)(width * 0.058); }

    int getPlantHeight() { return (int)(height * 0.17); }

    int getPlantLeftX() { return (int)(leftX + width * 0.17); }

    int getPlantTopY() { return (int)(topY + height * 0.12); }

    template<typename Ty>
    void foreach(Ty& container, std::function<void(typename Ty::value_type::iterator&, int)>);

    void crossedDelete();

    // 更新院子内所有的元素
    void update();

private:
    plants plantMatrix;
    ejects ejectList;
    zombies zombieList;
};

