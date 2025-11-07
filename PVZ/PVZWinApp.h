#pragma once
#include"stdafx.h"
#include<afxwin.h>
#include <list>  // 添加list头文件
#include <string> // 添加string头文件

// 新增：分数弹出效果结构体
struct ScorePopup {
    int points;
    int x, y;
    int lifeTime; // 存活时间
    int maxLifeTime; // 最大存活时间
    double alpha; // 透明度
};

class PVZWinApp : public CWinApp {
public:
    static bool gameOver;
    static bool gamePaused;
    static int score; // 分数变量
    static std::list<ScorePopup> scorePopups; // 新增：分数弹出列表

    static void updateScreen();
    static void loadNextFPS();
    static void mainLoop(HWND, UINT, UINT_PTR, DWORD);
    static void animationLoop(HWND, UINT, UINT_PTR, DWORD);
    static void gameTickLoop(HWND, UINT, UINT_PTR, DWORD);

    // 游戏控制函数
    static void GameOver();
    static void ResetGame();
    static void AddScore(int points); // 加分函数
    static void AddScorePopup(int points, int x, int y); // 新增：添加分数弹出效果

public:
    virtual BOOL InitInstance();
};