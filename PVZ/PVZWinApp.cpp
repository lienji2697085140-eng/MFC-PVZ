#include "stdafx.h"
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

PVZWinApp theApp;
PVZDoc* theDoc;

bool PVZWinApp::gameOver = false;
bool PVZWinApp::gamePaused = false;
int PVZWinApp::score = 0;
std::list<ScorePopup> PVZWinApp::scorePopups;

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

void PVZWinApp::AddScorePopup(int points, int x, int y) {
    ScorePopup popup;
    popup.points = points;
    popup.x = x-50;
    popup.y = y+100;
    popup.lifeTime = 0;
    popup.maxLifeTime = 10;
    popup.alpha = 1.0;
    scorePopups.push_back(popup);
}

void PVZWinApp::ResetGame() {
    gameOver = false;
    gamePaused = false;
    Visible::currentGameTick = 0;
    score = 0;
    scorePopups.clear();

    if (theDoc) {
        Yard& yard = theDoc->getYard();
        auto& sunList = theDoc->getSunList();
        auto& zombieList = yard.getZombieList();
        auto& ejectList = yard.getEjectList();

        auto sunIter = sunList.begin();
        while (sunIter != sunList.end()) {
            sunIter = sunList.erase(sunIter);
        }

        for (int row = 0; row < 5; ++row) {
            auto zombieIter = zombieList[row].begin();
            while (zombieIter != zombieList[row].end()) {
                zombieIter = zombieList[row].erase(zombieIter);
            }
        }

        yard.foreach(yard.getPlantMatrix(), [](Yard::plant_iter& iter, int) {
            *iter = nullptr;
            });

        for (int row = 0; row < 5; ++row) {
            auto ejectIter = ejectList[row].begin();
            while (ejectIter != ejectList[row].end()) {
                ejectIter = ejectList[row].erase(ejectIter);
            }
        }
    }
}

void PVZWinApp::mainLoop(HWND hWnd, UINT nMsg, UINT_PTR nIDEvent, DWORD dwTime) {
    updateScreen();
}

void PVZWinApp::animationLoop(HWND hWnd, UINT nMsg, UINT_PTR nIDEvent, DWORD dwTime) {
    if (gamePaused) {
        return;
    }

    loadNextFPS();

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
    if (gameOver || gamePaused) return;

    Yard& yard = theDoc->getYard();
    auto& sunList = theDoc->getSunList();
    auto& ejectList = yard.getEjectList();
    auto& zombieList = yard.getZombieList();

    for (int row = 0; row < 5; ++row) {
        for (auto& zombie : zombieList[row]) {
            if (zombie->getLeftX() <= 220) {
                GameOver();
                break;
            }
            if (gameOver) break;
        }
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
                AddScorePopup(10, popupX, popupY);
                AddScore(10);

#ifdef _DEBUG
                TRACE(_T("僵尸被消灭，在位置(%d,%d)弹出分数\n"), popupX, popupY);
#endif
            }
        }

        lastZombieLists[row] = zombieList[row];
    }

    if (Visible::currentGameTick % 100 == 0) {
        AddScore(1);
    }

    auto iter = sunList.begin();
    PVZDoc::SunlightList::iterator pos = sunList.end();

    for (; iter != sunList.end(); ++iter) {
        auto& sun = *iter;
        sun->update();
        if (sun->getLifeTime() <= 0) {
            pos = iter;
            iter = sunList.erase(iter);
            if (iter == sunList.end()) break;
        }
    }

    yard.update();

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

    if (Visible::currentGameTick % 500 == 0) {
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
    CSingleDocTemplate* pTemplate = new CSingleDocTemplate(
        IDR_MENU1, RUNTIME_CLASS(PVZDoc), RUNTIME_CLASS(PVZFrameWnd),
        RUNTIME_CLASS(PVZView));
    AddDocTemplate(pTemplate);
    OnFileNew();

    if (m_pMainWnd) {
        theDoc = (PVZDoc*)((CFrameWnd*)theApp.m_pMainWnd)->GetActiveDocument();
    }

    if (m_pMainWnd) {
        m_pMainWnd->SetTimer(MAIN_LOOP_TIMER, FLASH_TICK, PVZWinApp::mainLoop);
        m_pMainWnd->SetTimer(ANIMATION_LOOP_TIMER, FPS, PVZWinApp::animationLoop);
        m_pMainWnd->SetTimer(GAME_TICK_LOOP, GAME_TICK, PVZWinApp::gameTickLoop);
    }

    if (m_pMainWnd) {
        auto& rc = Visible::rcManage.getResource("Yard", Yard::ImgNoon).at(0);
        m_pMainWnd->MoveWindow(0, 0, rc->GetWidth(),
            (int)(rc->GetHeight() + rc->GetHeight() * 0.1));
        m_pMainWnd->ShowWindow(SW_SHOW);
        m_pMainWnd->UpdateWindow();
    }

    gameOver = false;
    gamePaused = false;
    score = 0;
    scorePopups.clear();

    return TRUE;
}