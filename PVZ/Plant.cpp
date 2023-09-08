#include "Plant.h"
#include "Yard.h"
#include "PVZDoc.h"
#include "ProjectilePea.h"

IMPLEMENT_DYNCREATE(Plant, GameElement)

Plant::Plant(CRuntimeClass* classMsg, int hp, int maxTime) : 
    GameElement(classMsg, hp), skillMaxWaitTime(maxTime) { 
    // maxTime 默认值-1 代表技能不受时间控制
}

void Plant::draw(HDC hDC) {

    // GameElement::draw(hDC);

    Yard &yard = theDoc->getYard();
    // 获得类信息
    std::string theClass = GetRuntimeClass()->m_lpszClassName;
    // 根据类名加载对应类
    auto& currMaps = rcManage.getResource(theClass, getMapState());
    // auto& shade = rcManage.getResource("Plant", 0).at(0);
    currMaps.at(animateTick)->AlphaBlend(hDC, CPoint(leftX, topY), isState(Plant::VIRTUAL) ? 130 : 255);
}

void Plant::update() {
    if (skillWaitTime != -1)
        ++skillWaitTime;

    collisioArea = CRect(leftX + width * 0.3, topY, leftX + width * 0.8, topY + height);
}

void Plant::skill() {
    // 置空技能累计时间
    // skillWaitTime = 0;
}

bool Plant::skillCheck() {
    return false;
}

bool Plant::collisio(GameElement& e) {
    return false;
}