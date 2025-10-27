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

// �����Ƶ�
PVZWinApp theApp;

PVZDoc* theDoc;

void PVZWinApp::mainLoop(HWND, UINT, UINT_PTR, DWORD) {
  // ��ʾˢ��
  updateScreen();
}

void PVZWinApp::animationLoop(HWND, UINT, UINT_PTR, DWORD) {
  // ����Ԫ�ض��� ������һ֡
  loadNextFPS();
}

void PVZWinApp::gameTickLoop(HWND, UINT, UINT_PTR, DWORD) {
  Yard& yard = theDoc->getYard();
  auto& sunList = theDoc->getSunList();
  auto& ejectList = yard.getEjectList();
  auto& zombieList = yard.getZombieList();
  auto iter = sunList.begin();
  PVZDoc::SunlightList::iterator pos = sunList.end();
  // ��Ȼ��������
  for (; iter != sunList.end(); ++iter) {
    auto& sun = *iter;
    sun->update();
    // AfxMessageBox("create");
    if (sun->getLifeTime() <= 0) {
      pos = iter;
      iter = sunList.erase(decltype(iter){iter});
      if (iter == sunList.end()) {
        break;
      }
    }
  }

  yard.update();

  if (Visible::currentGameTick % 70 == 0) {
    // ��������
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

  if (Visible::currentGameTick % 50 == 0) {
    // ���ɽ�ʬ
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

  // SeedBank����
  theDoc->getSeedBank().updateAllSeedState();
}

void PVZWinApp::updateScreen() {
  // ��ÿ����
  CFrameWnd* frame = (CFrameWnd*)theApp.m_pMainWnd;
  auto* view = frame->GetActiveView();
  // ������Ļ
  view->Invalidate(FALSE);
}

void PVZWinApp::loadNextFPS() {
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
  // MFCSdi��������
  CSingleDocTemplate* pTemplate = new CSingleDocTemplate(
    IDR_MENU1, RUNTIME_CLASS(PVZDoc), RUNTIME_CLASS(PVZFrameWnd),
    RUNTIME_CLASS(PVZView));
  AddDocTemplate(pTemplate);
  OnFileNew();

  theDoc = (PVZDoc*)((CFrameWnd*)theApp.m_pMainWnd)->GetActiveDocument();

  // ������ѭ����ʱ��
  m_pMainWnd->SetTimer(MAIN_LOOP_TIMER, FLASH_TICK, PVZWinApp::mainLoop);
  // ���ö�����ʱ��
  m_pMainWnd->SetTimer(ANIMATION_LOOP_TIMER, FPS, PVZWinApp::animationLoop);
  // ���ø���״̬��ʱ��
  m_pMainWnd->SetTimer(GAME_TICK_LOOP, GAME_TICK, PVZWinApp::gameTickLoop);
  // ���ù���״̬��ʱ��
  // m_pMainWnd->SetTimer(ATTACK_TICK_LOOP, GAME_TICK, PVZWinApp::attackLoop);

  auto& rc = Visible::rcManage.getResource("Yard", Yard::ImgNoon).at(0);
  // ��ʼ��������
  m_pMainWnd->MoveWindow(0, 0, rc->GetWidth(),
    (int)(rc->GetHeight() + rc->GetHeight() * 0.1));
  m_pMainWnd->ShowWindow(SW_SHOW);
  m_pMainWnd->UpdateWindow();
  return TRUE;
}