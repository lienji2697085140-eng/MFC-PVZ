#pragma once
#include "stdafx.h"
#include <afxwin.h>

class PVZWinApp : public CWinApp {
public:
    static bool gameOver;
    static bool gamePaused;

    static void updateScreen();
    static void loadNextFPS();
    static void mainLoop(HWND, UINT, UINT_PTR, DWORD);
    static void animationLoop(HWND, UINT, UINT_PTR, DWORD);
    static void gameTickLoop(HWND, UINT, UINT_PTR, DWORD);

    // ÓÎÏ·¿ØÖÆº¯Êý
    static void GameOver();
    static void ResetGame();

public:
    virtual BOOL InitInstance();
};