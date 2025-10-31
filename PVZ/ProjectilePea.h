#pragma once
#include "stdafx.h"
#include "Ejects.h"

class ProjectilePea : public Ejects {
    DECLARE_DYNCREATE(ProjectilePea)

public:
    static constexpr int step = 7;
    static constexpr int hurt = 20;

private:
    int penetrationCount;
    int hitCount; // 记录击中次数

public:
    ProjectilePea(CRuntimeClass* msg = RUNTIME_CLASS(ProjectilePea),
        int h = ProjectilePea::hurt);

    virtual void move();

    void setPenetration(int count) {
        if (count > 0) penetrationCount = count;
    }
    int getPenetration() const { return penetrationCount; }
    int getHitCount() const { return hitCount; }
    bool isActive() const { return penetrationCount > 0; }
};
