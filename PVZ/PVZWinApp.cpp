#include "PVZWinApp.h"
#include "PVZDoc.h"
#include "PVZFrameWnd.h"
#include "PVZView.h"
#include "Sun.h"
#include "Plant.h"
#include "Zombie.h"
#include "NormalZombie.h"
#include "Yard.h"
#include "Config.h"
#include "resource.h"

// 程序爆破点
PVZWinApp theApp;

PVZDoc* theDoc;

void PVZWinApp::mainLoop(HWND, UINT, UINT_PTR, DWORD) {

    // 显示刷新
    updateScreen();
}

void PVZWinApp::animationLoop(HWND, UINT, UINT_PTR, DWORD) {
    // 更新元素动画 进入下一帧
    loadNextFPS();
}

void PVZWinApp::gameTickLoop(HWND, UINT, UINT_PTR, DWORD) {
    Yard& yard = theDoc->getYard();
    auto& sunList = theDoc->getSunList();
    auto& ejectList = yard.getEjectList();
    auto& zombieList = yard.getZombieList();
    auto iter = sunList.begin();
    PVZDoc::SunlightList::iterator pos = sunList.end();
    // 自然生成阳光
    for (; iter != sunList.end(); ++iter) {
        auto& sun = *iter;
        sun->update();
        // AfxMessageBox("create");
        if (sun->getHp() <= 0 || sun->isState(Sun::FLASH)) {
            pos = iter;
            iter = sunList.erase(decltype(iter){iter});
            if (iter == sunList.end()) {
                break;
            }
        } 
    }

    yard.update();
    
    if (Visible::currentGameTick % 500 == 0) {
        // 生成阳光
        int stX = yard.getWidth() / 6 + rand() % (int)(yard.getWidth() * 0.8);
        int stY = 80 + rand() % 40;
        srand(time(NULL));
        int lifeTime = 2000 + rand() % 200;
        POINT finalPoint = { stX, rand() % (int)yard.getHeight() + 150};
        auto sun(std::make_shared<Sun>());
        sun->setLeftX(stX);
        sun->setTopY(stY);
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
            yard.getPlantHeight() * (row < 3 ? 0.1 : 0) - (zombie->getHeight() * 0.35));
        zombie->addState(Zombie::MOVE);
        zombieList[row].push_back(zombie);
    }

    ++Visible::currentGameTick;

    // SeedBank更新
    theDoc->getSeedBank().updateAllSeedState();
}

void PVZWinApp::attackLoop(HWND, UINT, UINT_PTR, DWORD) {
    theDoc->getYard().attackUpdate();
  
}

void PVZWinApp::updateScreen() {
    // 获得框架类
    CFrameWnd* frame = (CFrameWnd*)theApp.m_pMainWnd;
    auto* view = frame->GetActiveView();
    // 更新屏幕
    view->Invalidate(FALSE);
}

void PVZWinApp::loadNextFPS() {
    Yard& yard = theDoc->getYard();
    SeedBank& sbank = theDoc->getSeedBank();

    yard.foreach(yard.getPlantMatrix(), 
        [](Yard::plant_iter& iter, int) { if (*iter) (*iter)->nextAnimateTick(); });

    yard.foreach(yard.getZombieList(),
        [](Yard::zombie_iter& iter, int row) {
            (*iter)->nextAnimateTick();
        });

    yard.foreach(yard.getEjectList(),
        [](Yard::ejects_iter& iter, int) {
            (*iter)->nextAnimateTick();
        });
    
    auto &sunList = theDoc->getSunList();
    for (auto& sun : sunList) {
        sun->nextAnimateTick();
    }
    
}

BOOL PVZWinApp::InitInstance() {
    
    // MFCSdi基础代码
    CSingleDocTemplate* pTemplate = new CSingleDocTemplate(IDR_MENU1,
        RUNTIME_CLASS(PVZDoc), RUNTIME_CLASS(PVZFrameWnd), RUNTIME_CLASS(PVZView));
    AddDocTemplate(pTemplate);
    OnFileNew();
     

    theDoc = (PVZDoc*)((CFrameWnd*)theApp.m_pMainWnd)->GetActiveDocument();


    // 设置主循环计时器
    m_pMainWnd->SetTimer(MAIN_LOOP_TIMER, FLASH_TICK, PVZWinApp::mainLoop);
    // 设置动画计时器
    m_pMainWnd->SetTimer(ANIMATION_LOOP_TIMER, FPS, PVZWinApp::animationLoop);
    // 设置更新状态计时器
    m_pMainWnd->SetTimer(GAME_TICK_LOOP, GAME_TICK, PVZWinApp::gameTickLoop);
    // 设置攻击状态计时器
    m_pMainWnd->SetTimer(ATTACK_TICK_LOOP, GAME_TICK, PVZWinApp::attackLoop);


    auto& rc = Visible::rcManage.getResource("Yard", Yard::ImgNoon).at(0);
    // 初始化主窗口
    m_pMainWnd->MoveWindow(0, 0, rc->GetWidth(), rc->GetHeight() + rc->GetHeight() * 0.1);
    m_pMainWnd->ShowWindow(SW_SHOW);
    m_pMainWnd->UpdateWindow();
    return TRUE;
}
