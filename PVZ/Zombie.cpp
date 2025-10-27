#include "Zombie.h"
#include "GameElement.h"
#include "PVZDoc.h"
#include <vector>
#include <cstdlib>
#include <ctime>
#include <algorithm>

IMPLEMENT_DYNCREATE(Zombie, GameEntity)

extern PVZDoc* theDoc;

Zombie::Zombie(CRuntimeClass* classMsg, int hp, double moveSpeed, int h)
    : GameEntity(classMsg, hp),
    speed(moveSpeed),
    hurt(h),
    healthPoint(hp),
    originalSpeed(moveSpeed),
    slowTimer(0)
{
    std::srand(static_cast<unsigned>(std::time(nullptr))); // 初始化随机种子
}

void Zombie::draw(HDC hDC, int xOffset, int yOffset) {
    std::string className(classMsg->m_lpszClassName);
    auto& rc = rcManage.getResource(className, getMapState());

    if (isState(Zombie::HURT)) {
        cleanState(Zombie::HURT);
    }
    else {
        // 先绘制原始僵尸
        rc[animateTick]->Draw(hDC, (int)(leftX), (int)(topY));

        // 减速状态：添加冰霜粒子效果
        if (isState(Zombie::SLOWED)) {
            // 创建蓝色和白色画刷
            HBRUSH hBlueBrush = CreateSolidBrush(RGB(150, 200, 255));
            HBRUSH hWhiteBrush = CreateSolidBrush(RGB(230, 240, 255));
            HPEN hTransparentPen = (HPEN)GetStockObject(NULL_PEN);

            // 保存旧对象
            HBRUSH hOldBrush = (HBRUSH)SelectObject(hDC, hBlueBrush);
            HPEN hOldPen = (HPEN)SelectObject(hDC, hTransparentPen);

            int zombieWidth = getWidth();
            int zombieHeight = getHeight();
            int borderOffset = 5; // 距离边框的偏移量

            // 在僵尸周围生成25-35个小冰晶
            int iceCount = 25 + (std::rand() % 11); // 25到35个

            for (int i = 0; i < iceCount; ++i) {
                // 随机分布在边框周围
                int edge = std::rand() % 4; // 0=上,1=右,2=下,3=左
                int x = 0, y = 0;
                int size = 0;

                switch (edge) {
                case 0: // 上边
                    x = (int)leftX + borderOffset + (std::rand() % (zombieWidth - 2 * borderOffset));
                    y = (int)topY - 8 + (std::rand() % 6);
                    size = 3 + (std::rand() % 5); // 3-7像素
                    break;
                case 1: // 右边
                    x = (int)leftX + zombieWidth - borderOffset + (std::rand() % 6);
                    y = (int)topY + borderOffset + (std::rand() % (zombieHeight - 2 * borderOffset));
                    size = 4 + (std::rand() % 6); // 4-9像素
                    break;
                case 2: // 下边
                    x = (int)leftX + borderOffset + (std::rand() % (zombieWidth - 2 * borderOffset));
                    y = (int)topY + zombieHeight - borderOffset + (std::rand() % 6);
                    size = 3 + (std::rand() % 5); // 3-7像素
                    break;
                case 3: // 左边
                    x = (int)leftX - 8 + (std::rand() % 6);
                    y = (int)topY + borderOffset + (std::rand() % (zombieHeight - 2 * borderOffset));
                    size = 4 + (std::rand() % 6); // 4-9像素
                    break;
                }

                // 随机选择蓝色或白色
                if (std::rand() % 3 == 0) { // 1/3概率白色
                    SelectObject(hDC, hWhiteBrush);
                }
                else {
                    SelectObject(hDC, hBlueBrush);
                }

                // 绘制小冰晶（圆形或椭圆形）
                if (std::rand() % 2 == 0) {
                    // 圆形
                    Ellipse(hDC, x, y, x + size, y + size);
                }
                else {
                    // 椭圆形（随机拉伸）
                    int stretch = 1 + (std::rand() % 3);
                    Ellipse(hDC, x, y, x + size, y + size / stretch);
                }

                // 40%概率添加冰刺
                if (std::rand() % 100 < 40) {
                    MoveToEx(hDC, x + size / 2, y + size / 2, NULL);
                    int length = 3 + (std::rand() % 5);
                    LineTo(hDC,
                        x + size / 2 + (std::rand() % (length * 2) - length),
                        y + size / 2 + (std::rand() % (length * 2) - length));
                }
            }

            // 恢复旧对象并清理资源
            SelectObject(hDC, hOldBrush);
            SelectObject(hDC, hOldPen);
            DeleteObject(hBlueBrush);
            DeleteObject(hWhiteBrush);
        }
    }
}

void Zombie::update() {
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

    collisioArea = CRect(
        (int)(getLeftX() + getWidth() * 0.45),
        (int)(getTopY() + getWidth() * 0.25),
        (int)(getLeftX() + getWidth() * 0.6),
        (int)(getTopY() + getHeight() * 0.75)
    );
}

void Zombie::stateSwitch(bool isHalf) {
    if (isState(Zombie::EAT)) {
        attack();
    }
}

void Zombie::attack() {
    if (targetPlant) {
        targetPlant->damage(hurt);
        if (targetPlant->getHp() <= 0) {
            targetPlant.reset();
            setMapState(Zombie::MoveDynamic);
            setState(Zombie::MOVE);
            animateTick = 0;
        }
    }
}

void Zombie::applySlowEffect(int duration) {
    if (slowTimer < duration) {
        slowTimer = duration;
        speed = originalSpeed * 0.3;
        addState(Zombie::SLOWED);
    }
}

void Zombie::updateSlowEffect() {
    if (slowTimer > 0) {
        slowTimer--;
        if (slowTimer == 0) {
            speed = originalSpeed;
            cleanState(Zombie::SLOWED);
        }
    }
}
