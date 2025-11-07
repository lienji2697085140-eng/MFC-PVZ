#include "stdafx.h"
#include "PVZWinApp.h"
#include "Config.h"
#include "NormalZombie.h"
#include "BucketHeadZombie.h"
#include "PVZFrameWnd.h"
#include "PVZView.h"
#include "Plant.h"
#include "Sun.h"
#include "Yard.h"
#include "Zombie.h"
#include "resource.h"
#include <ctime>

PVZWinApp theApp;
PVZDoc* theDoc;

// 静态成员变量定义
bool PVZWinApp::gameOver = false;
bool PVZWinApp::gamePaused = false;
int PVZWinApp::score = 0;
std::list<ScorePopup> PVZWinApp::scorePopups;
int PVZWinApp::resetScoreCounter = -1;  // -1表示不需要重置分数

void PVZWinApp::GameOver() {
    gameOver = true;
    gamePaused = true;
}

void PVZWinApp::AddScore(int points) {
    // 如果正在重置分数，忽略加分
    if (resetScoreCounter >= 0) {
        return;
    }
    score += points;
#ifdef _DEBUG
    TRACE(_T("分数增加:+%d,当前分数:%d\n"), points, score);
#endif
}

void PVZWinApp::AddScorePopup(int points, int x, int y) {
    // 如果正在重置分数，忽略飘字
    if (resetScoreCounter >= 0) {
        return;
    }

    // 先增加实际分数
    AddScore(points);

    // 再显示飘字效果
    ScorePopup popup;
    popup.points = points;
    popup.x = x - 50;
    popup.y = y + 100;
    popup.lifeTime = 0;
    popup.maxLifeTime = 10;
    popup.alpha = 1.0;
    scorePopups.push_back(popup);
}

void PVZWinApp::ResetGame() {
    // 立即重置游戏状态
    gameOver = false;
    gamePaused = false;
    Visible::currentGameTick = 0;

    // 立即清空其他内容
    scorePopups.clear();

    if (theDoc) {
        Yard& yard = theDoc->getYard();
        auto& sunList = theDoc->getSunList();
        auto& zombieList = yard.getZombieList();
        auto& ejectList = yard.getEjectList();

        // 清空阳光列表
        sunList.clear();

        // 清空僵尸列表
        for (int row = 0; row < 5; ++row) {
            zombieList[row].clear();
        }

        // 清空植物矩阵
        yard.foreach(yard.getPlantMatrix(), [](Yard::plant_iter& iter, int) {
            *iter = nullptr;
            });

        // 清空投射物列表
        for (int row = 0; row < 5; ++row) {
            ejectList[row].clear();
        }
    }

    // 设置分数重置计数器（等待3帧后重置分数）
    resetScoreCounter = 3;

#ifdef _DEBUG
    TRACE(_T("游戏重置开始，分数将在3帧后重置为0\n"));
#endif
}

void PVZWinApp::mainLoop(HWND hWnd, UINT nMsg, UINT_PTR nIDEvent, DWORD dwTime) {
    updateScreen();
}

void PVZWinApp::animationLoop(HWND hWnd, UINT nMsg, UINT_PTR nIDEvent, DWORD dwTime) {
    if (gamePaused) {
        return;
    }

    loadNextFPS();

    // 更新分数飘字效果
    auto popupIter = scorePopups.begin();
    while (popupIter != scorePopups.end()) {
        popupIter->lifeTime++;
        popupIter->alpha = 1.0 - (double)popupIter->lifeTime / popupIter->maxLifeTime;
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
    // 处理分数重置逻辑
    if (resetScoreCounter > 0) {
        resetScoreCounter--;
#ifdef _DEBUG
        if (resetScoreCounter == 0) {
            TRACE(_T("分数重置倒计时: 即将重置分数为0\n"));
        }
#endif
    }
    else if (resetScoreCounter == 0) {
        score = 0;  // 实际重置分数
        resetScoreCounter = -1;  // 重置完成
#ifdef _DEBUG
        TRACE(_T("分数已重置为0\n"));
#endif
    }

    if (gameOver || gamePaused) return;

    Yard& yard = theDoc->getYard();
    auto& sunList = theDoc->getSunList();
    auto& ejectList = yard.getEjectList();
    auto& zombieList = yard.getZombieList();

    // 检测僵尸是否到达房子
    for (int row = 0; row < 5; ++row) {
        for (auto& zombie : zombieList[row]) {
            if (zombie->getLeftX() <= 220) {
                GameOver();
                break;
            }
        }
        if (gameOver) break;
    }
    if (gameOver) return;

    // 精确检测僵尸死亡位置
    static std::vector<std::list<std::shared_ptr<Zombie>>> lastZombieLists(5);
    for (int row = 0; row < 5; ++row) {
        for (auto& lastZombie : lastZombieLists[row]) {
            bool zombieStillExists = false;
            for (auto& currentZombie : zombieList[row]) {
                if (currentZombie == lastZombie) {
                    zombieStillExists = true;
                    break;
                }
            }
            if (!zombieStillExists) {
                int popupX = lastZombie->getLeftX() + lastZombie->getWidth() / 2;
                int popupY = lastZombie->getTopY();

                // 根据僵尸类型决定分数
                int points = 10;  // 默认普通僵尸10分
                if (dynamic_cast<BucketHeadZombie*>(lastZombie.get()) != nullptr) {
                    points = 30;  // 铁桶僵尸30分
                }

                AddScorePopup(points, popupX, popupY);
            }
        }
        lastZombieLists[row] = zombieList[row];
    }

    // 每100 tick自动加1分
    if (Visible::currentGameTick % 100 == 0) {
        AddScore(1);
    }

    // 更新阳光状态
    auto iter = sunList.begin();
    for (; iter != sunList.end(); ) {
        auto& sun = *iter;
        sun->update();
        if (sun->getLifeTime() <= 0) {
            iter = sunList.erase(iter);
        }
        else {
            ++iter;
        }
    }

    // 更新院子状态
    yard.update();

    // 自然生成阳光
    if (Visible::currentGameTick % 700 == 0) {
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

    // 生成僵尸
    if (Visible::currentGameTick % 5 == 0) {
        std::shared_ptr<Zombie> zombie;
        // 30%概率生成铁桶僵尸，70%概率生成普通僵尸
        if (rand() % 100 < 30) {
            zombie = std::make_shared<BucketHeadZombie>();
        }
        else {
            zombie = std::make_shared<NormalZombie>();
        }

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
    theDoc->getSeedBank().updateAllSeedState();
}

void PVZWinApp::updateScreen() {
    CFrameWnd* frame = (CFrameWnd*)theApp.m_pMainWnd;
    if (frame) {
        auto* view = frame->GetActiveView();
        if (view) {
            view->Invalidate(FALSE);
        }
    }
}

void PVZWinApp::loadNextFPS() {
    if (gamePaused) return;

    Yard& yard = theDoc->getYard();

    // 更新植物动画帧
    yard.foreach(yard.getPlantMatrix(), [](Yard::plant_iter& iter, int) {
        if (*iter) (*iter)->nextAnimateTick();
        });

    // 更新僵尸动画帧
    yard.foreach(yard.getZombieList(), [](Yard::zombie_iter& iter, int row) {
        (*iter)->nextAnimateTick();
        });

    // 更新投射物动画帧
    yard.foreach(yard.getEjectList(), [](Yard::ejects_iter& iter, int) {
        (*iter)->nextAnimateTick();
        });

    // 更新阳光动画帧
    auto& sunList = theDoc->getSunList();
    for (auto& sun : sunList) {
        sun->nextAnimateTick();
    }
}

BOOL PVZWinApp::InitInstance() {
    // MFC单文档模板初始化
    CSingleDocTemplate* pTemplate = new CSingleDocTemplate(
        IDR_MENU1, RUNTIME_CLASS(PVZDoc), RUNTIME_CLASS(PVZFrameWnd),
        RUNTIME_CLASS(PVZView));
    AddDocTemplate(pTemplate);
    OnFileNew();

    if (m_pMainWnd) {
        // 获取文档指针
        theDoc = (PVZDoc*)((CFrameWnd*)m_pMainWnd)->GetActiveDocument();

        // 设置主循环计时器
        m_pMainWnd->SetTimer(MAIN_LOOP_TIMER, FLASH_TICK, PVZWinApp::mainLoop);
        // 设置动画计时器
        m_pMainWnd->SetTimer(ANIMATION_LOOP_TIMER, FPS, PVZWinApp::animationLoop);
        // 设置游戏逻辑计时器
        m_pMainWnd->SetTimer(GAME_TICK_LOOP, GAME_TICK, PVZWinApp::gameTickLoop);
    }

    if (m_pMainWnd) {
        // 设置窗口大小和位置
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
    resetScoreCounter = -1;

    return TRUE;
}