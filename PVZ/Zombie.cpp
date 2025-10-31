#include "Zombie.h"
#include "GameElement.h"
#include "PVZDoc.h"
#include <vector>
#include <algorithm>

IMPLEMENT_DYNCREATE(Zombie, GameEntity)

extern PVZDoc* theDoc;

// ȫ�ֱ������洢�����ٽ�ʬ����ɫ������Ϣ
struct SlowEffectArea {
    CRect area;
    int duration;
    int timer;
};
static std::vector<SlowEffectArea> g_slowEffectAreas;
static const int SLOW_AREA_DURATION = 180; // ��ɫ�������180֡��3�룩

Zombie::Zombie(CRuntimeClass* classMsg, int hp, double moveSpeed, int h)
    : GameEntity(classMsg, hp), speed(moveSpeed), hurt(h), healthPoint(hp),
    originalSpeed(moveSpeed), slowTimer(0) {
}

void Zombie::draw(HDC hDC, int xOffset, int yOffset) {
    std::string className(classMsg->m_lpszClassName);
    auto& rc = rcManage.getResource(className, getMapState());

    if (isState(Zombie::HURT)) {
        cleanState(Zombie::HURT);
    }
    else {
        // ����ԭʼ��ʬ
        rc[animateTick]->Draw(hDC, (int)(leftX), (int)(topY));

        // ����״̬�������ɫЧ��
        if (isState(Zombie::SLOWED)) {
            // ������ɫ��͸�����ǲ�
            HDC memDC = CreateCompatibleDC(hDC);
            int width = rc[animateTick]->GetWidth();
            int height = rc[animateTick]->GetHeight();

            HBITMAP hBitmap = CreateCompatibleBitmap(hDC, width, height);
            HBITMAP hOldBitmap = (HBITMAP)SelectObject(memDC, hBitmap);

            BLENDFUNCTION blend = { AC_SRC_OVER, 0, 80, 0 };
            HBRUSH hBlueBrush = CreateSolidBrush(RGB(100, 150, 255));
            HBRUSH hOldBrush = (HBRUSH)SelectObject(memDC, hBlueBrush);

            SelectObject(memDC, GetStockObject(NULL_PEN));
            Rectangle(memDC, 0, 0, width, height);

            AlphaBlend(hDC, (int)leftX, (int)topY, width, height,
                memDC, 0, 0, width, height, blend);

            SelectObject(memDC, hOldBrush);
            DeleteObject(hBlueBrush);
            SelectObject(memDC, hOldBitmap);
            DeleteObject(hBitmap);
            DeleteDC(memDC);

            // ������ɫ����
            HPEN hBluePen = CreatePen(PS_SOLID, 3, RGB(0, 100, 255));
            HPEN hOldPen = (HPEN)SelectObject(hDC, hBluePen);
            HBRUSH hOldBrush2 = (HBRUSH)SelectObject(hDC, GetStockObject(NULL_BRUSH));

            // ����Ƚ�ʬ�Դ�
            CRect effectArea(
                (int)leftX - 10,
                (int)topY - 10,
                (int)(leftX + width + 10),
                (int)(topY + height + 10)
            );
            Rectangle(hDC, effectArea.left, effectArea.top, effectArea.right, effectArea.bottom);

            SelectObject(hDC, hOldBrush2);
            SelectObject(hDC, hOldPen);
            DeleteObject(hBluePen);
        }
    }

    // �������л�Ծ�ļ������򣨵����ã�
#ifdef _DEBUG
    for (const auto& effect : g_slowEffectAreas) {
        HPEN hDebugPen = CreatePen(PS_DASH, 2, RGB(0, 200, 255));
        HPEN hOldPen = (HPEN)SelectObject(hDC, hDebugPen);
        HBRUSH hOldBrush = (HBRUSH)SelectObject(hDC, GetStockObject(NULL_BRUSH));

        Rectangle(hDC, effect.area.left, effect.area.top,
            effect.area.right, effect.area.bottom);

        SelectObject(hDC, hOldBrush);
        SelectObject(hDC, hOldPen);
        DeleteObject(hDebugPen);
    }
#endif
}

void Zombie::update() {
    updateSlowEffect();

    auto& yard = theDoc->getYard();
    if (isState(Zombie::MOVE)) {
        leftX -= yard.getWidth() * speed;
    }

    // ����Ƿ�Ӵ����������ٽ�ʬ����ɫ����
    if (!isState(Zombie::SLOWED)) {
        CRect zombieRect((int)leftX, (int)topY,
            (int)(leftX + getWidth()),
            (int)(topY + getHeight()));

        for (const auto& effect : g_slowEffectAreas) {
            CRect intersect;
            if (intersect.IntersectRect(&zombieRect, &effect.area)) {
                applySlowEffect(SLOW_AREA_DURATION);
                break;
            }
        }
    }

    // ���¼�������
    for (auto it = g_slowEffectAreas.begin(); it != g_slowEffectAreas.end(); ) {
        it->timer--;
        if (it->timer <= 0) {
            it = g_slowEffectAreas.erase(it);
        }
        else {
            ++it;
        }
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
        speed = originalSpeed * 0.5; // �ٶȼ���
        addState(Zombie::SLOWED);

        // ������ɫ����Ч������
        SlowEffectArea newEffect;
        int width = getWidth();
        int height = getHeight();
        newEffect.area = CRect(
            (int)leftX - 15,
            (int)topY - 15,
            (int)(leftX + width + 15),
            (int)(topY + height + 15)
        );
        newEffect.duration = SLOW_AREA_DURATION;
        newEffect.timer = SLOW_AREA_DURATION;
        g_slowEffectAreas.push_back(newEffect);
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
