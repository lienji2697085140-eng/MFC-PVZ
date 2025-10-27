#include "Zombie.h"
#include "GameElement.h"
#include "PVZDoc.h"

IMPLEMENT_DYNCREATE(Zombie, GameEntity)

extern PVZDoc* theDoc;

Zombie::Zombie(CRuntimeClass* classMsg, int hp, double moveSpeed, int h)
    : GameEntity(classMsg, hp), speed(moveSpeed), hurt(h), healthPoint(hp),
    originalSpeed(moveSpeed), slowTimer(0) { // 初始化新成员
}

void Zombie::draw(HDC hDC, int xOffset, int yOffset) {
    std::string className(classMsg->m_lpszClassName);
    auto& rc = rcManage.getResource(className, getMapState());

    if (isState(Zombie::HURT)) {
        cleanState(Zombie::HURT);
        CImage img;
    }
    else {
        if (isState(Zombie::SLOWED)) {
            // 创建临时图像用于处理颜色
            CImage tempImage;
            tempImage.Create(rc[animateTick]->GetWidth(), rc[animateTick]->GetHeight(), 32);

            // 将原图像绘制到临时图像上
            HDC tempDC = tempImage.GetDC();
            rc[animateTick]->Draw(tempDC, 0, 0);

            // 处理像素颜色 - 转换为深蓝色调
            for (int y = 0; y < tempImage.GetHeight(); y++) {
                for (int x = 0; x < tempImage.GetWidth(); x++) {
                    COLORREF color = tempImage.GetPixel(x, y);
                    if (color != RGB(0, 0, 0)) { // 跳过纯黑色像素（通常是透明部分）
                        BYTE r = GetRValue(color);
                        BYTE g = GetGValue(color);
                        BYTE b = GetBValue(color);

                        // 增强蓝色，减弱红色和绿色
                        b = min(255, b + 100);
                        r = max(0, r - 50);
                        g = max(0, g - 50);

                        tempImage.SetPixel(x, y, RGB(r, g, b));
                    }
                }
            }
            tempImage.ReleaseDC();

            // 绘制处理后的图像
            tempImage.Draw(hDC, (int)(leftX), (int)(topY));
        }
        else {
            rc[animateTick]->Draw(hDC, (int)(leftX), (int)(topY));
        }
    }
}

void Zombie::update() {
    // 更新减速效果
    updateSlowEffect();

    auto& yard = theDoc->getYard();
    if (isState(Zombie::MOVE)) {
        leftX -= yard.getWidth() * speed;
    }
    auto& plants = theDoc->getYard().getPlantMatrix();
    if (!targetPlant) {
        theDoc->getYard().foreach(plants, [&](Yard::plant_iter& plantIter, int) {
            if (*plantIter && collisio(**plantIter)) {
                targetPlant = *plantIter;
                setMapState(Zombie::EatDynamic);
                setState(Zombie::EAT);
                animateTick = 0;
            }
            });
    }

    collisioArea =
        CRect((int)(getLeftX() + getWidth() * 0.45), (int)(getTopY() + getWidth() * 0.25),
            (int)(getLeftX() + getWidth() * 0.6), (int)(getTopY() + getHeight() * 0.75));
}

void Zombie::stateSwitch(bool isHalf) {
    if (isState(Zombie::EAT)) {
        attack();
    }
}

void Zombie::attack() {
    // 伤害植物
    if (targetPlant) targetPlant->damage(hurt);
    if (targetPlant->getHp() <= 0) {
        targetPlant.reset();
        setMapState(Zombie::MoveDynamic);
        setState(Zombie::MOVE);
        animateTick = 0;
    }
}

// 新增方法实现
void Zombie::applySlowEffect(int duration) {
    slowTimer = duration;
    speed = originalSpeed * 0.3; // 速度减半
    addState(Zombie::SLOWED); // 添加减速状态
}

void Zombie::updateSlowEffect() {
    if (slowTimer > 0) {
        slowTimer--;
        if (slowTimer == 0) {
            // 减速结束，恢复原速度
            speed = originalSpeed;
            cleanState(Zombie::SLOWED); // 清除减速状态
        }
    }
}
