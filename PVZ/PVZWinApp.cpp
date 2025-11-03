#include "PVZWinApp.h"
#include "Config.h"
#include "NormalZombie.h"
#include "PVZDoc.h"
#include "PVZFrameWnd.h"
#include "PVZView.h"
#include "Plant.h"
#include "Sun.h"
#include "Yard.h"
#include "Zombie.h"
#include "resource.h"

// 程序爆破点
PVZWinApp theApp;

PVZDoc* theDoc;

// 初始化静态成员变量
bool PVZWinApp::gameOver = false;
bool PVZWinApp::gamePaused = false;

void PVZWinApp::GameOver() {
    gameOver = true;
    gamePaused = true;
}

void PVZWinApp::ResetGame() {
    gameOver = false;
    gamePaused = false;
    Visible::currentGameTick = 0;

    // 清空游戏数据（避免使用clear()）
    if (theDoc) {
        Yard& yard = theDoc->getYard();
        auto& sunList = theDoc->getSunList();
        auto& zombieList = yard.getZombieList();
        auto& ejectList = yard.getEjectList();

        // 清空阳光 - 使用erase逐个删除
        auto sunIter = sunList.begin();
        while (sunIter != sunList.end()) {
            sunIter = sunList.erase(sunIter);
        }

        // 清空僵尸 - 对每行逐个删除
        for (int row = 0; row < 5; ++row) {
            auto zombieIter = zombieList[row].begin();
            while (zombieIter != zombieList[row].end()) {
                zombieIter = zombieList[row].erase(zombieIter);
            }
        }

        // 清空植物 - 设置为nullptr
        yard.foreach(yard.getPlantMatrix(), [](Yard::plant_iter& iter, int) {
            *iter = nullptr;
            });

        // 清空发射物 - 使用erase逐个删除
        for (int row = 0; row < 5; ++row) {
            auto ejectIter = ejectList[row].begin();
            while (ejectIter != ejectList[row].end()) {
                ejectIter = ejectList[row].erase(ejectIter);
            }
        }

        // 玩家状态会在游戏自然进行中重置，不需要调用reset()
        // SeedBank会在游戏重新开始时自动重置
    }
}

void PVZWinApp::mainLoop(HWND, UINT, UINT_PTR, DWORD) {
    // 显示刷新
    updateScreen();
}

void PVZWinApp::animationLoop(HWND, UINT, UINT_PTR, DWORD) {
    // 如果游戏暂停，直接返回
    if (gamePaused) {
        return;
    }
    // 更新元素动画 进入下一帧
    loadNextFPS();
}

void PVZWinApp::gameTickLoop(HWND, UINT, UINT_PTR, DWORD) {
    // 如果游戏结束或暂停，直接返回
    if (gameOver || gamePaused) {
        return;
    }

    Yard& yard = theDoc->getYard();
    auto& sunList = theDoc->getSunList();
    auto& ejectList = yard.getEjectList();
    auto& zombieList = yard.getZombieList();

    // 检测僵尸是否到达x=220的边框
    for (int row = 0; row < 5; ++row) {
        for (auto& zombie : zombieList[row]) {
            if (zombie->getLeftX() <= 220) {
                // 僵尸碰到边框，游戏失败
                GameOver();
                break;
            }
        }
        if (gameOver) break;
    }

    // 如果游戏结束，直接返回
    if (gameOver) {
        return;
    }

    auto iter = sunList.begin();
    PVZDoc::SunlightList::iterator pos = sunList.end();
    // 自然生成阳光
    for (; iter != sunList.end(); ++iter) {
        auto& sun = *iter;
        sun->update();
        if (sun->getLifeTime() <= 0) {
            pos = iter;
            iter = sunList.erase(decltype(iter){iter});
            if (iter == sunList.end()) {
                break;
            }
        }
    }

    yard.update();

    if (Visible::currentGameTick % 700 == 0) {
        // 生成阳光
        int stX = (int)(yard.getWidth() / 6 + rand() % (int)(yard.getWidth() * 0.8));
        int stY = 80 + rand() % 40;
        srand((unsigned int)(time(NULL)));
        int lifeTime = 2000 + rand() % 200;
        POINT finalPoint = { stX, rand() % (int)yard.getHeight() + 150 };
        auto sun(std::make_shared<Sun>());
        sun->setLeftX(stX);
        sun->setTopY(stY);
        sun->setFinalPoint(finalPoint);
        sun->setMapState(Sun::NoFlash);
        sunList.push_front(sun);
    }

    if (Visible::currentGameTick % 500 == 0) {
        // 生成僵尸
        auto zombie(std::make_shared<NormalZombie>(RUNTIME_CLASS(NormalZombie)));
        zombie->setMapState(Zombie::MoveDynamic);
        zombie->setLeftX(yard.getWidth() * 0.7);
        int row = rand() % 5;
        zombie->setTopY(yard.getPlantTopY() + row * yard.getPlantHeight() +
            yard.getPlantHeight() * (row < 3 ? 0.1 : 0) -
            (zombie->getHeight() * 0.35));
        zombie->addState(Zombie::MOVE);
        zombieList[row].push_back(zombie);
    }

    ++Visible::currentGameTick;

    // SeedBank更新
    theDoc->getSeedBank().updateAllSeedState();
}

void PVZWinApp::updateScreen() {
    // 获得框架类
    CFrameWnd* frame = (CFrameWnd*)theApp.m_pMainWnd;
    auto* view = frame->GetActiveView();
    // 更新屏幕
    view->Invalidate(FALSE);
}

void PVZWinApp::loadNextFPS() {
    // 如果游戏暂停，直接返回
    if (gamePaused) {
        return;
    }

    Yard& yard = theDoc->getYard();
    SeedBank& sbank = theDoc->getSeedBank();

    yard.foreach(yard.getPlantMatrix(), [](Yard::plant_iter& iter, int) {
        if (*iter) (*iter)->nextAnimateTick();
        });

    yard.foreach(yard.getZombieList(), [](Yard::zombie_iter& iter, int row) {
        (*iter)->nextAnimateTick();
        });

    yard.foreach(yard.getEjectList(), [](Yard::ejects_iter& iter, int) {
        (*iter)->nextAnimateTick();
        });

    auto& sunList = theDoc->getSunList();
    for (auto& sun : sunList) {
        sun->nextAnimateTick();
    }
}

BOOL PVZWinApp::InitInstance() {
    // MFCSdi基础代码
    CSingleDocTemplate* pTemplate = new CSingleDocTemplate(
        IDR_MENU1, RUNTIME_CLASS(PVZDoc), RUNTIME_CLASS(PVZFrameWnd),
        RUNTIME_CLASS(PVZView));
    AddDocTemplate(pTemplate);
    OnFileNew();

    theDoc = (PVZDoc*)((CFrameWnd*)theApp.m_pMainWnd)->GetActiveDocument();

    // 设置主循环计时器
    m_pMainWnd->SetTimer(MAIN_LOOP_TIMER, FLASH_TICK, PVZWinApp::mainLoop);
    // 设置动画计时器
    m_pMainWnd->SetTimer(ANIMATION_LOOP_TIMER, FPS, PVZWinApp::animationLoop);
    // 设置更新状态计时器
    m_pMainWnd->SetTimer(GAME_TICK_LOOP, GAME_TICK, PVZWinApp::gameTickLoop);

    auto& rc = Visible::rcManage.getResource("Yard", Yard::ImgNoon).at(0);
    // 初始化主窗口
    m_pMainWnd->MoveWindow(0, 0, rc->GetWidth(),
        (int)(rc->GetHeight() + rc->GetHeight() * 0.1));
    m_pMainWnd->ShowWindow(SW_SHOW);
    m_pMainWnd->UpdateWindow();

    // 初始化游戏状态
    gameOver = false;
    gamePaused = false;

    return TRUE;
}