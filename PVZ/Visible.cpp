#include "Visible.h"

#include "ResourceManage.h"

IMPLEMENT_DYNCREATE(Visible, CObject)

// 初始化资源管理器
ResourceManage Visible::rcManage{"resources x1.5"};

size_t Visible::currentGameTick = 0;

Visible::Visible(CRuntimeClass* msg) : classMsg(msg) {
  // AfxMessageBox(msg->m_lpszClassName);
  auto& rc = rcManage.getResource(classMsg->m_lpszClassName, mapState).at(0);
  width = rc->GetWidth();
  height = rc->GetHeight();
}

void Visible::draw(HDC hDC, int xOffset, int yOffset) {
  // ::SetPixel(hDC, center.x, center.y, RGB(255, 0, 0));
}

void Visible::stateSwitch(bool isHalf) {}
