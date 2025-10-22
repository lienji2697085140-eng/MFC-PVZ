#pragma once
#include "stdafx.h"
#include "GameElement.h"
#include "PVZDoc.h"

class Zombie : public GameEntity {
    DECLARE_DYNCREATE(Zombie)

public:
    enum State { STAND = 0x10, MOVE = 0x100, EAT = 0x1000, HURT = 0x10000, SLOWED = 0x100000 }; // ���SLOWED״̬

    enum MapState { StandDynamic = 0, MoveDynamic, EatDynamic };

private:
    double speed;
    double originalSpeed; // ����ԭʼ�ٶ�
    int healthPoint;
    int hurt;
    int slowTimer; // ���ټ�ʱ��

    std::shared_ptr<Plant> targetPlant{ nullptr };

public:
    Zombie(CRuntimeClass* classMsg = nullptr, int hp = 0, double speed = 0,
        int hurt = 0);

    // ���ƺ����͹��ܺ���
    virtual void draw(HDC, int xOffset = 0, int yOffset = 0);

    void update();

    void setSpeed(double s) {
        speed = s;
        if (originalSpeed == 0) originalSpeed = s; // ��һ������ʱ����ԭʼ�ٶ�
    }
    double getSpeed() const { return speed; }

    void setTargetPlant(std::shared_ptr<Plant> p) { targetPlant = p; }

    void stateSwitch(bool isHalf);

    void attack();

    // ��������
    void applySlowEffect(int duration = 300); // Ӧ�ü���Ч��
    void updateSlowEffect(); // ���¼���Ч��
};