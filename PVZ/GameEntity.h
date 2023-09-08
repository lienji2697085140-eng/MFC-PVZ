#include "GameElement.h"

/// <summary>
/// GameElement 是游戏中的实体 有生命值与碰撞体积 
/// </summary>

class GameEntity : public GameElement {
public:
    DECLARE_DYNCREATE(GameEntity)

public:
    GameEntity(CRuntimeClass* msg = nullptr);

protected:
    int healthPoint;
    CRect collisioArea{ 0 };
    
};