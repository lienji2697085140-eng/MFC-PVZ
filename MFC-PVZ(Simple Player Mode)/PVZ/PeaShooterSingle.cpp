#include "PeaShooterSingle.h"

#include "PVZDoc.h"
#include "ProjectilePea.h"

IMPLEMENT_DYNCREATE(PeaShooterSingle, Plant)

PeaShooterSingle::PeaShooterSingle(CRuntimeClass* theClass,
  CRuntimeClass* eject, int hp)
  : Plant(theClass, hp), ejectClass(eject) {}

void PeaShooterSingle::draw(HDC hDC, int xOffset, int yOffset) {
  Plant::draw(hDC);
}

bool PeaShooterSingle::skillCheck() {
  auto& rowZombie = theDoc->getYard().getZombieList()[plantPos.x];
  if (!rowZombie.empty()) return true;
  return false;
}

void PeaShooterSingle::skill() {
  // 设置状态
  // 设置技能贴图
  auto& yard = theDoc->getYard();
  // 获取植物的行
  int row = plantPos.x;
  // 添加对应
  auto& ejects = yard.getEjectList();
  std::shared_ptr<Ejects> pea((Ejects*)ejectClass->CreateObject());
  pea->setState(Ejects::DEFAULT);
  pea->setLeftX(leftX + width * 0.9);
  pea->setTopY(topY + height * 0.2);
  ejects[row].push_front(pea);
}

void PeaShooterSingle::stateSwitch(bool isHalf) {
  if (isState(Plant::SKILL)) {
    if (isHalf) {
      skill();  // 释放技能
      return;
    }
    setMapState(Plant::DefaultDynamic);
    addState(Plant::SKILL_SLEEP);  // 进入休眠状态
    cleanState(Plant::SKILL);
    setState(0x0);
    return;
  }

  if (isState(Plant::SKILL_REDAY) && !isHalf) {
    // 进入就绪状态后首先切换贴图
    setMapState(Plant::SkillDynamic);
    // 下一轮后进入技能
    addState(Plant::SKILL);
    // 清除就绪状态
    cleanState(Plant::SKILL_REDAY);
    return;
  }
}