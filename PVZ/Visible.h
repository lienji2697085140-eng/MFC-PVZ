#pragma once
#include <afxwin.h>
#include <atlimage.h>
#include <map>
#include <vector>
#include <string>
#include "ResourceManage.h"
#include "Config.h"

#define DEBUG_BORDER

class Visible : public CObject {

    DECLARE_DYNCREATE(Visible)

public:

    enum MapState { Static = 0 };

protected:
    double leftX = 0, topY = 0;
    double width = 0, height = 0;
    int animateTick{ 0 };
    size_t mapState{ Static };

    CRuntimeClass* classMsg;

public:
    static ResourceManage rcManage;
    static size_t currentGameTick;

public:

    // Visible() = default;
    // Visible(int x = 0, int y = 0, int wd = 0, int hg = 0, CRuntimeClass* msg = nullptr);

    Visible(CRuntimeClass* msg = nullptr);

    virtual void draw(HDC, int xOffset = 0, int yOffset = 0);

    void nextAnimateTick() {
        auto& rc = rcManage.getResource(classMsg->m_lpszClassName, getMapState());
        ++animateTick;
        if (animateTick >= rc.size()) {
            stateSwitch(false);
            animateTick = 0;
            return;
        }
        if (animateTick == rc.size() / 2) {
            stateSwitch(true);
            return;
        }
    }

    double getLeftX() const { return leftX; }
    void setLeftX(double x) { leftX = x; }
    double getTopY() const { return topY; }
    void setTopY(double y) { topY = y; }
    double getWidth() const { return width; }
    double getHeight() const { return height; }
    int getAnimateTick() const { return animateTick; }

    size_t getMapState() { return mapState; }

    void setMapState(size_t state) { mapState = state; }

    ResourceManage& getRcManage() { return rcManage; }

    virtual void stateSwitch(bool isHalf);
};

