#pragma once
#include"stdafx.h"
#include<afxwin.h>
#include <list>
#include <string>

// 分数弹出效果结构体
struct ScorePopup {
    int points;
    int x, y;
    int lifeTime;
    int maxLifeTime;
    double alpha;
};

class PVZWinApp : public CWinApp {
public:
    static bool gameOver;
    static bool gamePaused;
    static int score;
    static std::list<ScorePopup> scorePopups;
    static int resetScoreCounter;  // 只添加分数重置计数器
    /*// PVZWinApp.cpp 三模式切换核心逻辑
bool PVZWinApp::isTestMode = false; // 全局模式标志

void PVZWinApp::toggleGameMode() {
    isTestMode = !isTestMode;
    ResetGame(); // 切换模式时重置游戏
    // 根据模式调整游戏参数
    if (isTestMode) {
        // 测试模式：植物冷却减半、启用调试显示
        Plant::setCoolDownRatio(0.5); 
        PVZView::enableDebugDisplay(true);
    } else {
        // 玩家模式：恢复默认参数、禁用调试显示
        Plant::setCoolDownRatio(1.0);
        PVZView::enableDebugDisplay(false);
    }
    // 更新界面模式提示
    if (theDoc && theDoc->getYard().getModeLabel()) {
        CString modeText = isTestMode ? _T("当前模式：Test Mode") : _T("当前模式：Player Mode");
        theDoc->getYard().getModeLabel()->SetWindowText(modeText);
    }
}

// PVZView.cpp 快捷键触发模式切换
void PVZView::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags) {
    // F1键切换模式
    if (nChar == VK_F1) {
        PVZWinApp::toggleGameMode();
        Invalidate(TRUE);
        return;
    }
    // 其他按键逻辑（空格暂停、ESC退出铲子模式等）
    // ...
}*/
    static void updateScreen();
    static void loadNextFPS();
    static void mainLoop(HWND, UINT, UINT_PTR, DWORD);
    static void animationLoop(HWND, UINT, UINT_PTR, DWORD);
    static void gameTickLoop(HWND, UINT, UINT_PTR, DWORD);

    static void GameOver();
    static void ResetGame();
    static void AddScore(int points);
    static void AddScorePopup(int points, int x, int y);

public:
    virtual BOOL InitInstance();
};