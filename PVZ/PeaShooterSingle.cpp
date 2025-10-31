#include "PeaShooterSingle.h"
#include "PVZDoc.h"
#include "ProjectilePea.h"

IMPLEMENT_DYNCREATE(PeaShooterSingle, Plant)

PeaShooterSingle::PeaShooterSingle(CRuntimeClass* theClass,
    CRuntimeClass* eject, int hp)
    : Plant(theClass, hp), ejectClass(eject) {
}

void PeaShooterSingle::draw(HDC hDC, int xOffset, int yOffset) {
    Plant::draw(hDC);
}

bool PeaShooterSingle::skillCheck() {
    auto& rowZombie = theDoc->getYard().getZombieList()[plantPos.x];
    return !rowZombie.empty();
}

void PeaShooterSingle::skill() {
    auto& yard = theDoc->getYard();
    int row = plantPos.x;
    auto& ejects = yard.getEjectList();

    // 阶梯式水平间距：每颗豌豆比前一颗更靠前
    for (int i = 0; i < 10; i++) {
        std::shared_ptr<Ejects> pea((Ejects*)ejectClass->CreateObject());
        pea->setState(Ejects::DEFAULT);

        // 阶梯式水平位置：第一颗最近，第三颗最远
        int horizontalOffset = i * 30; // 每颗豌豆比前一颗远30像素

        pea->setLeftX(leftX + width * 0.9 + horizontalOffset);
        pea->setTopY(topY /*+ height * 0.2 + (i * 3) - 3*/);

        ejects[row].push_front(pea);
    }
}

void PeaShooterSingle::stateSwitch(bool isHalf) {
    if (isState(Plant::SKILL)) {
        if (isHalf) {
            skill();
            return;
        }
        setMapState(Plant::DefaultDynamic);
        addState(Plant::SKILL_SLEEP);
        cleanState(Plant::SKILL);
        setState(0x0);
        return;
    }

    if (isState(Plant::SKILL_REDAY) && !isHalf) {
        setMapState(Plant::SkillDynamic);
        addState(Plant::SKILL);
        cleanState(Plant::SKILL_REDAY);
        return;
    }
}
