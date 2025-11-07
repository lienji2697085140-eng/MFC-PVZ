#pragma once
#include "stdafx.h"
#include <afxwin.h>

class PVZWinApp : public CWinApp {
public:
    static bool gameOver;
    static bool gamePaused;
    static int score; // 分数变量

    static void updateScreen();
    static void loadNextFPS();
    static void mainLoop(HWND, UINT, UINT_PTR, DWORD);
    static void animationLoop(HWND, UINT, UINT_PTR, DWORD);
    static void gameTickLoop(HWND, UINT, UINT_PTR, DWORD);

    // 游戏控制函数
    static void GameOver();
    static void ResetGame();
    static void AddScore(int points); // 加分函数

public:
    virtual BOOL InitInstance();
};