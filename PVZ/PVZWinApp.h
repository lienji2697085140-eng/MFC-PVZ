#pragma once
#include "stdafx.h"
#include <afxwin.h>
#include <list>
#include <string>

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
    static int resetScoreCounter;
    static int isTestMode1;

    static void updateScreen();
    static void loadNextFPS();
    static void mainLoop(HWND, UINT, UINT_PTR, DWORD);
    static void animationLoop(HWND, UINT, UINT_PTR, DWORD);
    static void gameTickLoop(HWND, UINT, UINT_PTR, DWORD);
    static void GameOver();
    static void ResetGame();
    static void AddScore(int points);
    static void AddScorePopup(int points, int x, int y);

    virtual BOOL InitInstance();
};