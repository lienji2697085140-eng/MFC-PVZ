#include "Plant.h"

#include "PVZDoc.h"
#include "ProjectilePea.h"
#include "Yard.h"

IMPLEMENT_DYNCREATE(Plant, GameEntity)

Plant::Plant(CRuntimeClass* classMsg, int hp, int maxTime)
  : GameEntity(classMsg, hp), skillMaxWaitTime(maxTime) {
  // maxTime 默认值-1 代表技能不受时间控制
}

void Plant::draw(HDC hDC, int xOffset, int yOffset) {
  // GameEntity::draw(hDC);

  Yard& yard = theDoc->getYard();
  // 获得类信息
  std::string theClass = GetRuntimeClass()->m_lpszClassName;
  // 根据类名加载对应类
  auto& currMaps = rcManage.getResource(theClass, getMapState());
  currMaps.at(animateTick)
    ->AlphaBlend(hDC, CPoint((int)(leftX + xOffset), (int)(topY + yOffset)),
      isState(Plant::VIRTUAL) ? 130 : 255);
}

void Plant::setPosition() {
  collisioArea =
    CRect((int)(leftX + width * 0.3), (int)topY, (int)(leftX + width * 0.8), (int)(topY + height));
}

void Plant::update() {
  skillWaitTime++;  // 增加技能等待时间

  setPosition();

  // 接触触发的植物
  if (skillMaxWaitTime == -1 && skillCheck() && isOnlyState(0x0)) {
    addState(Plant::SKILL_REDAY);
    return;
  }
  // 非接触式的植物
  if (skillMaxWaitTime != -1 && skillWaitTime > skillMaxWaitTime) {
    skillWaitTime = 0;
    addState(Plant::SKILL_REDAY);
  }
}

void Plant::skill() {
  // 置空技能累计时间
  // skillWaitTime = 0;
}

// 默认返回true  需要子类重写后控制
bool Plant::skillCheck() { return true; }

void Plant::stateSwitch(bool isHalf) {
  if (isState(Plant::SKILL_REDAY) && !isHalf) {
    // 进入就绪状态后首先切换贴图
    setMapState(Plant::SkillDynamic);
    // 下一轮后进入技能
    addState(Plant::SKILL);
    // 清除就绪状态
    cleanState(Plant::SKILL_REDAY);
    return;
  }

  if (isState(Plant::SKILL)) {
    if (isHalf) {
      skill();  // 释放技能
      return;
    }
    setMapState(Plant::DefaultDynamic);
    cleanState(Plant::SKILL);
    return;
  }
}