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

    // ����ʽˮƽ��ࣺÿ���㶹��ǰһ�Ÿ���ǰ
    for (int i = 0; i < 10; i++) {
        std::shared_ptr<Ejects> pea((Ejects*)ejectClass->CreateObject());
        pea->setState(Ejects::DEFAULT);

        // ����ʽˮƽλ�ã���һ���������������Զ
        int horizontalOffset = i * 30; // ÿ���㶹��ǰһ��Զ30����

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
