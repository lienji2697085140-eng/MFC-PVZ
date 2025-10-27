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
    std::srand(static_cast<unsigned>(std::time(nullptr))); // ��ʼ���������
}

void Zombie::draw(HDC hDC, int xOffset, int yOffset) {
    std::string className(classMsg->m_lpszClassName);
    auto& rc = rcManage.getResource(className, getMapState());

    if (isState(Zombie::HURT)) {
        cleanState(Zombie::HURT);
    }
    else {
        // �Ȼ���ԭʼ��ʬ
        rc[animateTick]->Draw(hDC, (int)(leftX), (int)(topY));

        // ����״̬����ӱ�˪����Ч��
        if (isState(Zombie::SLOWED)) {
            // ������ɫ�Ͱ�ɫ��ˢ
            HBRUSH hBlueBrush = CreateSolidBrush(RGB(150, 200, 255));
            HBRUSH hWhiteBrush = CreateSolidBrush(RGB(230, 240, 255));
            HPEN hTransparentPen = (HPEN)GetStockObject(NULL_PEN);

            // ����ɶ���
            HBRUSH hOldBrush = (HBRUSH)SelectObject(hDC, hBlueBrush);
            HPEN hOldPen = (HPEN)SelectObject(hDC, hTransparentPen);

            int zombieWidth = getWidth();
            int zombieHeight = getHeight();
            int borderOffset = 5; // ����߿��ƫ����

            // �ڽ�ʬ��Χ����25-35��С����
            int iceCount = 25 + (std::rand() % 11); // 25��35��

            for (int i = 0; i < iceCount; ++i) {
                // ����ֲ��ڱ߿���Χ
                int edge = std::rand() % 4; // 0=��,1=��,2=��,3=��
                int x = 0, y = 0;
                int size = 0;

                switch (edge) {
                case 0: // �ϱ�
                    x = (int)leftX + borderOffset + (std::rand() % (zombieWidth - 2 * borderOffset));
                    y = (int)topY - 8 + (std::rand() % 6);
                    size = 3 + (std::rand() % 5); // 3-7����
                    break;
                case 1: // �ұ�
                    x = (int)leftX + zombieWidth - borderOffset + (std::rand() % 6);
                    y = (int)topY + borderOffset + (std::rand() % (zombieHeight - 2 * borderOffset));
                    size = 4 + (std::rand() % 6); // 4-9����
                    break;
                case 2: // �±�
                    x = (int)leftX + borderOffset + (std::rand() % (zombieWidth - 2 * borderOffset));
                    y = (int)topY + zombieHeight - borderOffset + (std::rand() % 6);
                    size = 3 + (std::rand() % 5); // 3-7����
                    break;
                case 3: // ���
                    x = (int)leftX - 8 + (std::rand() % 6);
                    y = (int)topY + borderOffset + (std::rand() % (zombieHeight - 2 * borderOffset));
                    size = 4 + (std::rand() % 6); // 4-9����
                    break;
                }

                // ���ѡ����ɫ���ɫ
                if (std::rand() % 3 == 0) { // 1/3���ʰ�ɫ
                    SelectObject(hDC, hWhiteBrush);
                }
                else {
                    SelectObject(hDC, hBlueBrush);
                }

                // ����С������Բ�λ���Բ�Σ�
                if (std::rand() % 2 == 0) {
                    // Բ��
                    Ellipse(hDC, x, y, x + size, y + size);
                }
                else {
                    // ��Բ�Σ�������죩
                    int stretch = 1 + (std::rand() % 3);
                    Ellipse(hDC, x, y, x + size, y + size / stretch);
                }

                // 40%������ӱ���
                if (std::rand() % 100 < 40) {
                    MoveToEx(hDC, x + size / 2, y + size / 2, NULL);
                    int length = 3 + (std::rand() % 5);
                    LineTo(hDC,
                        x + size / 2 + (std::rand() % (length * 2) - length),
                        y + size / 2 + (std::rand() % (length * 2) - length));
                }
            }

            // �ָ��ɶ���������Դ
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
