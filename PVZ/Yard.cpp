#include "Yard.h"
#include "Zombie.h"
#include "Config.h"

IMPLEMENT_DYNCREATE(Yard, GameElement)

Yard::Yard() : GameElement(RUNTIME_CLASS(Yard)) {
}

Yard::plants& Yard::getPlantMatrix() {
    return plantMatrix;
}

Yard::ejects& Yard::getEjectList() {
    return ejectList;
}

Yard::zombies& Yard::getZombieList() {
    return zombieList;
}

void Yard::draw(HDC hDC) {
    // 绘制背景
    auto &vec = rcManage.getResource("Yard", currentState);
    auto &img = vec[0];
    img->AlphaBlend(hDC, 0, 0, width, height, 0, 0, img->GetWidth(), img->GetHeight());

    // 绘制院子内的元素
    foreach(plantMatrix, [&](plant_iter& iter, int) { if (*iter) (*iter)->draw(hDC); });
    foreach(zombieList, [&](zombie_iter& iter, int row) { if (*iter) (*iter)->draw(hDC); });
    foreach(ejectList, [&](ejects_iter& iter, int) { if (*iter) (*iter)->draw(hDC); });   
}

bool Yard::isMovePlantArea(int cx, int cy) {
    if (cx >= leftX + width * 0.18 && cx <= width * 0.7
        && cy >= topY + height * 0.12 && cy <= height * 0.95) {
        return true;
    }
    return false;
}

bool Yard::onClick(int cx, int cy) {
    return isMovePlantArea(cx, cy);
}

POINT Yard::getPlantPos(int cx, int cy) {
    int lx = leftX + width * 0.17;
    int ty = topY + height * 0.12;
    int w = width * 0.06;
    int h = height * 0.17;
    for (int row = 0; row < 5; ++row) {
        for (int col = 0; col < 9; ++col) {
            RECT rect = { lx + w * col, ty + h * row, lx + w * col + w, ty + h * row + h };
            if (cx >= rect.left && cx <= rect.right
                && cy >= rect.top && cy <= rect.bottom) {
                return POINT{ row, col };
            }
        }
    }
    return POINT{ -1, -1 };
}

void Yard::crossedDelete() {

    foreach(plantMatrix,
        [&](plant_iter& iter, int row) {
            if (*iter && (*iter)->getHp() <= 0) {
                auto& plant = (*iter);
                plant.reset();
            }
        });

    foreach(zombieList,
        [&](zombie_iter& iter, int row) {
            if ((*iter)->getLeftX() < getWidth() * 0.1 || (*iter)->getHp() <= 0)
            iter = zombieList[row].erase(decltype(iter){iter});
        });

    foreach(ejectList, 
        [&](ejects_iter& iter, int row) {
            if ((*iter)->getLeftX() > getWidth() * 0.8)
                iter = ejectList[row].erase(decltype(iter){iter});
        });
}

template<typename Ty>
void Yard::foreach(Ty& container, 
        std::function<void(typename Ty::value_type::iterator&, int)> call) {
    for (int row = 0; row < container.size(); ++row) {
        for (auto iter = container[row].begin();
            iter != container[row].end(); ++iter) {
            call(iter, row);
            if (iter == container[row].end()) break;
        }
    }
}

void Yard::attackUpdate() {
    foreach(plantMatrix, [&](plant_iter& plantIter, int row) {
        if (*plantIter && (*plantIter)->skillCheck()) {
            (*plantIter)->addState(Plant::SKILL_REDAY);
        }
    });

    foreach(ejectList, [&](ejects_iter& ejectIter, int ejectRow) {
        foreach(zombieList, [&](zombie_iter& zombieIter, int zombieRow) {
        if (ejectRow == zombieRow && (*ejectIter)->collisio(*(*zombieIter))) {
            (*ejectIter)->setMapState(Ejects::ImgBreak);
            (*ejectIter)->cleanState(Ejects::DEFAULT);
            (*ejectIter)->addState(Ejects::BREAK);
            (*zombieIter)->addState(Zombie::HURT);
            (*zombieIter)->damage((*ejectIter)->getHurt());
            (*ejectIter)->setHurt(0);
        }
        });
    });

    
    foreach(zombieList, [&](zombie_iter& zombieIter, int zombieRow) {
        foreach(plantMatrix, [&](plant_iter& plantIter, int plantRow) {
        if (plantRow == zombieRow) {
            if (*plantIter && (*zombieIter)->collisio(*(*plantIter))) {
                (*zombieIter)->setState(Zombie::EAT);
                (*zombieIter)->setMapState(Zombie::EatDynamic);
                (*zombieIter)->setTargetPlant(*plantIter);
            }
        }
        });
    });

}


void Yard::update() {
    
    // 越界删除
    crossedDelete();

    // 处理植物攻击
    attackUpdate();

    // 更新植物状态 技能
    foreach(plantMatrix, [](plant_iter& iter, int) { if (*iter) (*iter)->update(); });
    foreach(zombieList, [](zombie_iter& iter, int) { (*iter)->update(); });
    foreach(ejectList, [&](ejects_iter& iter, int row) {
        (*iter)->update();
        if ((*iter)->isState(Ejects::DEL)) iter = ejectList[row].erase(iter);
    });

}
