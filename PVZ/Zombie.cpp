#include "Zombie.h"
#include "GameElement.h"
#include "PVZDoc.h"

IMPLEMENT_DYNCREATE(Zombie, GameEntity)

extern PVZDoc* theDoc;

Zombie::Zombie(CRuntimeClass* classMsg, int hp, double moveSpeed, int h)
    : GameEntity(classMsg, hp), speed(moveSpeed), hurt(h), healthPoint(hp),
    originalSpeed(moveSpeed), slowTimer(0) { // ��ʼ���³�Ա
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
            // ������ʱͼ�����ڴ�����ɫ
            CImage tempImage;
            tempImage.Create(rc[animateTick]->GetWidth(), rc[animateTick]->GetHeight(), 32);

            // ��ԭͼ����Ƶ���ʱͼ����
            HDC tempDC = tempImage.GetDC();
            rc[animateTick]->Draw(tempDC, 0, 0);

            // ����������ɫ - ת��Ϊ����ɫ��
            for (int y = 0; y < tempImage.GetHeight(); y++) {
                for (int x = 0; x < tempImage.GetWidth(); x++) {
                    COLORREF color = tempImage.GetPixel(x, y);
                    if (color != RGB(0, 0, 0)) { // ��������ɫ���أ�ͨ����͸�����֣�
                        BYTE r = GetRValue(color);
                        BYTE g = GetGValue(color);
                        BYTE b = GetBValue(color);

                        // ��ǿ��ɫ��������ɫ����ɫ
                        b = min(255, b + 100);
                        r = max(0, r - 50);
                        g = max(0, g - 50);

                        tempImage.SetPixel(x, y, RGB(r, g, b));
                    }
                }
            }
            tempImage.ReleaseDC();

            // ���ƴ�����ͼ��
            tempImage.Draw(hDC, (int)(leftX), (int)(topY));
        }
        else {
            rc[animateTick]->Draw(hDC, (int)(leftX), (int)(topY));
        }
    }
}

void Zombie::update() {
    // ���¼���Ч��
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
    // �˺�ֲ��
    if (targetPlant) targetPlant->damage(hurt);
    if (targetPlant->getHp() <= 0) {
        targetPlant.reset();
        setMapState(Zombie::MoveDynamic);
        setState(Zombie::MOVE);
        animateTick = 0;
    }
}

// ��������ʵ��
void Zombie::applySlowEffect(int duration) {
    slowTimer = duration;
    speed = originalSpeed * 0.3; // �ٶȼ���
    addState(Zombie::SLOWED); // ��Ӽ���״̬
}

void Zombie::updateSlowEffect() {
    if (slowTimer > 0) {
        slowTimer--;
        if (slowTimer == 0) {
            // ���ٽ������ָ�ԭ�ٶ�
            speed = originalSpeed;
            cleanState(Zombie::SLOWED); // �������״̬
        }
    }
}
