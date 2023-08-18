#pragma once
//#include <afxwin.h>
//#include "PVZWinApp.h"
//#include "PVZFrameWnd.h"
//#include "PVZDoc.h"
//#include "Yard.h"
//#include "Player.h"
//#include "SeedBank.h"
//#include "Sun.h"
//#include <vector>
//#include <list>
//#include <memory>
#include <afxwin.h>
#include <list>
#include "Player.h"
#include "SeedBank.h"
#include "Yard.h"
#include "ResourceManage.h"

class PVZDoc : public CDocument {
    DECLARE_DYNCREATE(PVZDoc)

public:
    using SunlightList = std::list<std::shared_ptr<Sun>>;

private:
    // 文档类保存院子信息和玩家信息
    Yard yard;
    Player player;
    SeedBank sbank;
    SunlightList sunList;

public:
    PVZDoc();

public:
    Yard& getYard();
    Player& getPlayer();
    SeedBank& getSeedBank();
    SunlightList& getSunList();
};
