#pragma once
#include <afxwin.h>

class PVZWinApp : public CWinApp {
public:
    static void updateScreen();
    static void loadNextFPS();
    // static void sunGenerated();
    static void mainLoop(HWND, UINT, UINT_PTR, DWORD);
    static void attackLoop(HWND, UINT, UINT_PTR, DWORD);
    static void animationLoop(HWND, UINT, UINT_PTR, DWORD);
    static void gameTickLoop(HWND, UINT, UINT_PTR, DWORD);
public:
    virtual BOOL InitInstance();
};

