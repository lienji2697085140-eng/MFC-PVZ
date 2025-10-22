#pragma once
#include "stdafx.h"
#include "GameElement.h"
#include "PVZDoc.h"

class Zombie : public GameEntity {
    DECLARE_DYNCREATE(Zombie)

public:
    enum State { STAND = 0x10, MOVE = 0x100, EAT = 0x1000, HURT = 0x10000, SLOWED = 0x100000 }; // 添加SLOWED状态

    enum MapState { StandDynamic = 0, MoveDynamic, EatDynamic };

private:
    double speed;
    double originalSpeed; // 保存原始速度
    int healthPoint;
    int hurt;
    int slowTimer; // 减速计时器

    std::shared_ptr<Plant> targetPlant{ nullptr };

public:
    Zombie(CRuntimeClass* classMsg = nullptr, int hp = 0, double speed = 0,
        int hurt = 0);

    // 绘制函数和功能函数
    virtual void draw(HDC, int xOffset = 0, int yOffset = 0);

    void update();

    void setSpeed(double s) {
        speed = s;
        if (originalSpeed == 0) originalSpeed = s; // 第一次设置时保存原始速度
    }
    double getSpeed() const { return speed; }

    void setTargetPlant(std::shared_ptr<Plant> p) { targetPlant = p; }

    void stateSwitch(bool isHalf);

    void attack();

    // 新增方法
    void applySlowEffect(int duration = 300); // 应用减速效果
    void updateSlowEffect(); // 更新减速效果
};