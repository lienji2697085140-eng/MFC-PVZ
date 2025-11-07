#include "stdafx.h"  // 确保stdafx.h在最前面
#include "PVZWinApp.h"
#include "Config.h"
#include "NormalZombie.h"
#include "PVZFrameWnd.h"
#include "PVZView.h"
#include "Plant.h"
#include "Sun.h"
#include "Yard.h"
#include "resource.h"
#include <ctime>

// 程序爆破点
PVZWinApp theApp;
PVZDoc* theDoc;

// 初始化静态成员变量
bool PVZWinApp::gameOver = false;
bool PVZWinApp::gamePaused = false;
int PVZWinApp::score = 0;
std::list<ScorePopup> PVZWinApp::scorePopups; // 新增：初始化分数弹出列表

void PVZWinApp::GameOver() {
    gameOver = true;
    gamePaused = true;
}

void PVZWinApp::AddScore(int points) {
    score += points;
#ifdef _DEBUG
    TRACE(_T("分数增加:+%d,当前分数:%d\n"), points, score);
#endif
}

// 新增：添加分数弹出效果函数
void PVZWinApp::AddScorePopup(int points, int x, int y) {
    ScorePopup popup;
    popup.points = points;
    popup.x = x;
    popup.y = y;
    popup.lifeTime = 0;
    popup.maxLifeTime = 100; // 持续100 tick（约1秒）
    popup.alpha = 1.0;
    scorePopups.push_back(popup);
}

void PVZWinApp::ResetGame() {
    gameOver = false;
    gamePaused = false;
    Visible::currentGameTick = 0;
    score = 0;
    scorePopups.clear(); // 新增：清空分数弹出效果

    // 清空游戏数据
    if (theDoc) {
        Yard& yard = theDoc->getYard();
        auto& sunList = theDoc->getSunList();
        auto& zombieList = yard.getZombieList();
        auto& ejectList = yard.getEjectList();

        // 清空阳光
        auto sunIter = sunList.begin();
        while (sunIter != sunList.end()) {
            sunIter = sunList.erase(sunIter);
        }

        // 清空僵尸
        for (int row = 0; row < 5; ++row) {
            auto zombieIter = zombieList[row].begin();
            while (zombieIter != zombieList[row].end()) {
                zombieIter = zombieList[row].erase(zombieIter);
            }
        }

        // 清空植物
        yard.foreach(yard.getPlantMatrix(), [](Yard::plant_iter& iter, int) {
            *iter = nullptr;
            });

        // 清空发射物
        for (int row = 0; row < 5; ++row) {
            auto ejectIter = ejectList[row].begin();
            while (ejectIter != ejectList[row].end()) {
                ejectIter = ejectList[row].erase(ejectIter);
            }
        }
    }
}

void PVZWinApp::mainLoop(HWND hWnd, UINT nMsg, UINT_PTR nIDEvent, DWORD dwTime) {
    // 显示刷新
    updateScreen();
}

void PVZWinApp::animationLoop(HWND hWnd, UINT nMsg, UINT_PTR nIDEvent, DWORD dwTime) {
    // 如果游戏暂停,直接返回
    if (gamePaused) {
        return;
    }

    // 更新元素动画进入下一帧
    loadNextFPS();

    // 新增：更新分数弹出效果
    auto popupIter = scorePopups.begin();
    while (popupIter != scorePopups.end()) {
        popupIter->lifeTime++;

        // 计算透明度（渐变消失）
        popupIter->alpha = 1.0 - (double)popupIter->lifeTime / popupIter->maxLifeTime;

        // 向上移动（漂浮效果）
        popupIter->y -= 1;

        if (popupIter->lifeTime >= popupIter->maxLifeTime) {
            popupIter = scorePopups.erase(popupIter);
        }
        else {
            ++popupIter;
        }
    }
}

void PVZWinApp::gameTickLoop(HWND hWnd, UINT nMsg, UINT_PTR nIDEvent, DWORD dwTime) {
    // 如果游戏结束或暂停,直接返回
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
                // 僵尸碰到边框,游戏失败
                GameOver();
                break;
            }
            if (gameOver) break;
        }
    }

    // 如果游戏结束，直接返回
    if (gameOver) {
        return;
    }

    // 简单版本:通过僵尸数量变化来检测消灭情况
    static std::vector<int> lastZombieCounts(5, 0);
    for (int row = 0; row < 5; ++row) {
        int currentCount = (int)zombieList[row].size();  // 显式转换为int
        int lastCount = lastZombieCounts[row];

        if (currentCount < lastCount) {
            // 僵尸数量减少，说明有僵尸被消灭
            int zombiesKilled = lastCount - currentCount;
            AddScore(zombiesKilled * 10);

            // 新增：为每个被消灭的僵尸添加分数弹出效果
            for (int i = 0; i < zombiesKilled; i++) {
                // 在随机位置显示分数弹出（模拟在僵尸死亡位置）
                int popupX = 105;
                int popupY = 17;
                AddScorePopup(10, popupX, popupY);
            }

#ifdef _DEBUG
            TRACE(_T("第%d行消灭%d个僵尸，加%d分\n"), row, zombiesKilled, zombiesKilled * 10);
#endif
        }
        lastZombieCounts[row] = currentCount;
    }

    // 时间分数:每秒加1分(假设100 tick=1秒)
    if (Visible::currentGameTick % 100 == 0) {
        AddScore(1);
    }

    auto iter = sunList.begin();
    PVZDoc::SunlightList::iterator pos = sunList.end();

    // 自然生成阳光
    for (; iter != sunList.end(); ++iter) {
        auto& sun = *iter;
        sun->update();
        if (sun->getLifeTime() <= 0) {
            pos = iter;
            iter = sunList.erase(iter);
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
        int lifeTime = 2000 + rand() % 200;
        POINT finalPoint = { stX, rand() % (int)yard.getHeight() + 150 };
        auto sun = std::make_shared<Sun>();
        sun->setLeftX(stX);
        sun->setTopY(stY);
        sun->setFinalPoint(finalPoint);
        sun->setMapState(Sun::NoFlash);
        sunList.push_front(sun);
    }

    if (Visible::currentGameTick % 500 == 0) {
        // 生成僵尸
        auto zombie = std::make_shared<NormalZombie>();
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
    if (frame) {
        auto* view = frame->GetActiveView();
        if (view) {
            // 更新屏幕
            view->Invalidate(FALSE);
        }
    }
}

void PVZWinApp::loadNextFPS() {
    // 如果游戏暂停，直接返回
    if (gamePaused) {
        return;
    }

    Yard& yard = theDoc->getYard();

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
    // MFC SDI基础代码
    CSingleDocTemplate* pTemplate = new CSingleDocTemplate(
        IDR_MENU1, RUNTIME_CLASS(PVZDoc), RUNTIME_CLASS(PVZFrameWnd),
        RUNTIME_CLASS(PVZView));
    AddDocTemplate(pTemplate);
    OnFileNew();

    if (m_pMainWnd) {
        theDoc = (PVZDoc*)((CFrameWnd*)theApp.m_pMainWnd)->GetActiveDocument();
    }

    // 设置主循环计时器
    if (m_pMainWnd) {
        m_pMainWnd->SetTimer(MAIN_LOOP_TIMER, FLASH_TICK, PVZWinApp::mainLoop);
        // 设置动画计时器
        m_pMainWnd->SetTimer(ANIMATION_LOOP_TIMER, FPS, PVZWinApp::animationLoop);
        // 设置更新状态计时器
        m_pMainWnd->SetTimer(GAME_TICK_LOOP, GAME_TICK, PVZWinApp::gameTickLoop);
    }

    // 初始化主窗口
    if (m_pMainWnd) {
        auto& rc = Visible::rcManage.getResource("Yard", Yard::ImgNoon).at(0);
        m_pMainWnd->MoveWindow(0, 0, rc->GetWidth(),
            (int)(rc->GetHeight() + rc->GetHeight() * 0.1));
        m_pMainWnd->ShowWindow(SW_SHOW);
        m_pMainWnd->UpdateWindow();
    }

    // 初始化游戏状态
    gameOver = false;
    gamePaused = false;
    score = 0;
    scorePopups.clear();

    return TRUE;
}