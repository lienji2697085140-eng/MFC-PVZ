#include "stdafx.h"
#include "Zombie.h"

IMPLEMENT_DYNCREATE(Zombie, GameEntity)

extern PVZDoc* theDoc;

namespace {
    struct IceEffect {
        CRect area;
        int timer;
        int angle;
        int colorPhase; // ��ɫ�׶Σ�0-2��ÿ��һ���׶�
    };

    std::vector<IceEffect> g_iceEffects;
    const int ICE_TIME = 720; // 3�루60֡/�� �� 3�룩
}

Zombie::Zombie(CRuntimeClass* classMsg, int hp, double moveSpeed, int h)
    : GameEntity(classMsg, hp), speed(moveSpeed), hurt(h), healthPoint(hp),
    originalSpeed(moveSpeed), slowTimer(0) {
}

// ����ʱ���ȡ��ɫ����������ǳ�����䣩
COLORREF GetIceColor(int colorPhase, int frameInPhase) {
    // ÿ��60֡��ÿ����ɫ�׶�60֡
    float progress = frameInPhase / 60.0f; // ��ǰ�׶��ڵĽ��ȣ�0.0-1.0��

    // ������ɫ�׶Σ����� -> ���� -> ǳ��
    COLORREF colors[3] = {
        RGB(0, 50, 150),   // ��������1�룩
        RGB(50, 100, 200),  // ��������2�룩
        RGB(100, 150, 255)  // ǳ������3�룩
    };

    if (colorPhase >= 2) {
        return colors[2]; // ���һ�뱣��ǳ��ɫ
    }

    // ��ɫ��ֵ
    COLORREF startColor = colors[colorPhase];
    COLORREF endColor = colors[colorPhase + 1];

    int r1 = GetRValue(startColor);
    int g1 = GetGValue(startColor);
    int b1 = GetBValue(startColor);

    int r2 = GetRValue(endColor);
    int g2 = GetGValue(endColor);
    int b2 = GetBValue(endColor);

    int r = r1 + (int)((r2 - r1) * progress);
    int g = g1 + (int)((g2 - g1) * progress);
    int b = b1 + (int)((b2 - b1) * progress);

    return RGB(r, g, b);
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
    }

    // �������б���Ч��
    for (auto& effect : g_iceEffects) {
        effect.angle = (effect.angle + 6) % 360; // ÿ֡��ת6��

        // ������ɫ�׶κ�֡��
        int totalFrames = ICE_TIME - effect.timer; // �Ѳ��ŵ�֡��
        effect.colorPhase = totalFrames / 60; // ÿ��һ���׶Σ�0-2��
        int frameInPhase = totalFrames % 60;  // ��ǰ�׶��ڵ�֡��

        COLORREF iceColor = GetIceColor(effect.colorPhase, frameInPhase);

        // ������ת��С����
        HPEN hPen = CreatePen(PS_SOLID, 2, iceColor);
        HPEN hOldPen = (HPEN)SelectObject(hDC, hPen);
        HBRUSH hOldBrush = (HBRUSH)SelectObject(hDC, GetStockObject(NULL_BRUSH));

        int centerX = (effect.area.left + effect.area.right) / 2;
        int centerY = (effect.area.top + effect.area.bottom) / 2;
        int size = 6; // С������С

        // ����8��С����Χ�ƽ�ʬ��ת
        for (int i = 0; i < 8; i++) {
            double angle = (effect.angle + i * 45) * 3.14159 / 180.0;
            int offsetX = (int)(effect.area.Width() * 0.35 * cos(angle));
            int offsetY = (int)(effect.area.Height() * 0.35 * sin(angle));

            int x = centerX + offsetX - size / 2;
            int y = centerY + offsetY - size / 2;

            // ����С���α���
            POINT points[5];
            points[0] = { x + size / 2, y };
            points[1] = { x + size, y + size / 2 };
            points[2] = { x + size / 2, y + size };
            points[3] = { x, y + size / 2 };
            points[4] = points[0];

            Polyline(hDC, points, 5);
        }

        SelectObject(hDC, hOldPen);
        DeleteObject(hPen);
    }
}

void Zombie::update() {
    // ����Ч������
    if (slowTimer > 0) {
        slowTimer--;
        if (slowTimer == 0) {
            speed = originalSpeed;
            cleanState(Zombie::SLOWED);
        }
    }

    // �ƶ��߼�
    auto& yard = theDoc->getYard();
    if (isState(Zombie::MOVE)) {
        leftX -= yard.getWidth() * speed;
    }

    // ����Ƿ�Ӵ����������ٽ�ʬ�ı���Ч������
    if (!isState(Zombie::SLOWED)) {
        CRect zombieRect((int)leftX, (int)topY,
            (int)(leftX + getWidth()),
            (int)(topY + getHeight()));

        for (const auto& effect : g_iceEffects) {
            CRect intersect;
            if (intersect.IntersectRect(&zombieRect, &effect.area)) {
                applySlowEffect(ICE_TIME);
                break;
            }
        }
    }

    // ���±���Ч��
    for (auto it = g_iceEffects.begin(); it != g_iceEffects.end(); ) {
        it->timer--;
        if (it->timer <= 0) {
            it = g_iceEffects.erase(it);
        }
        else {
            ++it;
        }
    }

    // ֲ����ײ���
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
        speed = originalSpeed * 0.3; // �ٶȼ���
        addState(Zombie::SLOWED);

        // ��������Ч������
        IceEffect newEffect;
        int width = getWidth();
        int height = getHeight();

        newEffect.area = CRect(
            (int)leftX - 30, (int)topY - 30,
            (int)(leftX + width + 30), (int)(topY + height + 30)
        );
        newEffect.timer = duration;
        newEffect.angle = 0;
        newEffect.colorPhase = 0; // �ӵ�0�׶ο�ʼ
        g_iceEffects.push_back(newEffect);
    }
}
