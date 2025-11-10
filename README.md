# 🌱 基于MFC实现的植物大战僵尸-基础功能与优化改进介绍
![MFC-PVZ Logo](此处替换为项目Logo路径)  
*项目全称：《植物大战僵尸:全龄智斗模式+盲盒植物工坊》*
<img width="2048" height="2048" alt="image" src="https://github.com/user-attachments/assets/87d88436-3ebb-41c4-8374-4f52d3d454de" />


## 一、基础功能实现
基于MFC框架构建的植物大战僵尸游戏，首先还原了经典玩法的核心流程与交互逻辑，确保基础体验贴合原作，同时通过模块化设计保证功能稳定性。

### 1.1 核心对战流程
遵循“植物防御-僵尸进攻-胜负判定”的经典塔防逻辑，实现完整游戏闭环：
- **植物系统基础能力**：支持豌豆射手、寒冰射手、食人花等核心植物的种植（消耗阳光）、自动攻击（按冷却时间触发）、技能释放（如寒冰射手减速、食人花吞噬），植物需种植在指定网格区域，且不可重叠。
- **僵尸系统基础行为**：实现普通僵尸、铁桶僵尸的路径移动（沿固定行向植物阵地推进）、碰撞攻击（接触植物时持续造成伤害）、状态变化（受伤减HP、死亡消失），僵尸生成频率随游戏时间逐步提升以增加难度。
- **胜负判定规则**：当僵尸突破植物防线到达阵地终点时，触发“游戏结束”；若玩家成功抵御所有僵尸波次（或持续存活至目标时间），判定“游戏胜利”，Player Mode下会显示最终分数与重新开始按钮。

### 1.2 基础交互与界面
- **操作交互**：支持鼠标点击种植植物（从卡槽拖拽至网格）、收集阳光（点击空中掉落的阳光）、使用铲子工具（移除已种植植物），键盘操作包含“空格键暂停/继续”“ESC键退出铲子模式”，满足基础游玩操作需求。
- **界面显示**：主界面实时展示阳光数量（种植植物的消耗单位）、当前分数（击杀僵尸+时间累积）、植物卡槽（含冷却时间提示），状态界面包含暂停画面（半透明覆盖层）、游戏结束结算界面，支持多分辨率设备适配（如iPhone14 Pro Max、iPad Pro）。

### 1.3 基础数据与资源管理
- **数据存储**：维护游戏状态数据（分数、阳光、冷却时间）、配置数据（植物/僵尸属性：豌豆射手攻击力20、普通僵尸HP180、铁桶僵尸HP540）、临时数据（场上植物/僵尸/投射物实例），确保游戏过程数据实时更新。
- **资源加载**：通过`ResourceManage`类统一加载植物/僵尸精灵图、特效动画（如寒冰冰晶、分数飘字）、界面元素，采用预加载与缓存机制，避免重复IO操作，提升画面渲染流畅度。


## 二、优化改进部分
在基础功能之上，项目通过**三模式设计**、**功能增强**、**性能优化**三大方向进行改进，既提升开发测试效率，又丰富玩家体验。

### 2.1 核心优化：三模式切换设计
新增**Player Mode（玩家模式）、Simple Player Mode（简单模式）、Test Mode（测试模式）** ，适配不同场景需求，解决“开发调试”与“玩家体验”的矛盾：
#### 2.1.1 模式差异与适配
| 优化点                | Player Mode（玩家模式）       | Simple Player Mode（简单模式） | Test Mode（测试模式）               |
|-----------------------|--------------------------------------|--------------------------------|-------------------------------------|
| 难度与流程            | 完整对战（正常冷却、僵尸强度）        | 简化机制（冷却缩短30%、僵尸强度降50%） | 跳过开局，直接生成测试僵尸，可跳过胜负判定 |
| 功能开放              | 禁用调试功能，专注游玩体验            | 禁用调试功能，降低入门门槛       | 启用全部调试功能（右键显示坐标、显示僵尸HP/帧率） |
| 资源消耗              | 植物价格正常（如豌豆射手25阳光）     | 植物价格正常                   | 植物价格为0，冷却时间减半，支持快速测试植物组合 |
| 数据显示              | 仅显示阳光、分数                     | 仅显示阳光、分数               | 额外显示坐标（如(624,376)）、帧率（目标30FPS）、僵尸数量 |
|界面展示               |<img width="2078" height="901" alt="image" src="https://github.com/user-attachments/assets/1a72bf5c-8351-4121-860d-e9542cd0f750" />|<img width="2079" height="902" alt="image" src="https://github.com/user-attachments/assets/4e4d4ac3-aaa2-4397-920c-1c5ee0b1a4d9" />|<img width="2082" height="901" alt="image" src="https://github.com/user-attachments/assets/bc89b08b-a77f-45af-87bd-818d1fd29776" />|




#### 2.1.2 模式切换实现
- **触发方式**：通过主界面“模式切换”按钮或快捷键（F1）触发，切换时自动重置当前游戏状态（清空场上植物/僵尸、重置分数与阳光），避免模式参数冲突。
- **参数适配代码**：通过`GameModeConfig`单例类封装模式参数，按模式动态调整冷却时间、调试显示等配置，示例如下：
```cpp
// 模式参数封装与适配
void GameModeConfig::setMode(int modeType) {
    if (modeType == 2) { // Test Mode
        plantCoolDownRatio = 0.5; // 冷却减半
        enableDebugDisplay = true; // 显示调试信息
    } else if (modeType == 1) { // Simple Mode
        plantCoolDownRatio = 0.7; // 冷却缩短30%
        zombieStrengthRatio = 0.5; // 僵尸强度降50%
    } else { // Player Mode
        plantCoolDownRatio = 1.0; // 恢复默认
        enableDebugDisplay = false;
    }
}
```

### 2.2 功能增强：植物与僵尸系统优化
#### 2.2.1 植物功能升级
- **寒冰射手增强**：在基础减速功能（子弹命中僵尸后速度降为30%）基础上，新增“概率范围冰冻”效果，触发时生成冰晶旋转动画，覆盖僵尸周围30像素范围，持续300帧（约5秒），提升控场能力。
- **豌豆射手升级**：从“单次1颗子弹”优化为“阶梯式发射10颗子弹”，水平间距30像素，攻击覆盖范围提升2倍，解决原作中“单一行仅能攻击1个僵尸”的局限，代码示例：
```cpp
// 豌豆射手多子弹发射逻辑
void PeaShooterSingle::skill() {
    for (int i = 0; i < 10; i++) {
        std::shared_ptr<Ejects> pea((Ejects*)ejectClass->CreateObject());
        int horizontalOffset = i * 30; // 阶梯式水平偏移
        pea->setLeftX(leftX + width * 0.9 + horizontalOffset);
        pea->setTopY(topY);
        ejects[row].push_front(pea);
    }
}
```
<img width="800" height="439" alt="image" src="https://github.com/user-attachments/assets/2be37596-a40d-4559-af7a-b94c27a2d1c8" />

- **铲子工具新增**：玩家可点击“铲子按钮”进入移除模式，左键点击已种植植物即可删除，解决原作中“种植错误后无法调整”的问题，优化布局灵活性。
<img width="125" height="83" alt="image" src="https://github.com/user-attachments/assets/8feda70c-d434-4735-9178-38d7d75fec04" />

#### 2.2.2 僵尸系统扩展与分数机制
- **新增铁桶僵尸**：相比普通僵尸，生命值提升至800（普通僵尸300）、移动速度降低至0.0003（普通僵尸0.0002），击败后获得30分（普通僵尸10分），增加游戏策略多样性（需用高伤害植物优先击破）。
- **分数系统优化**：新增“时间得分”（每1秒加1分）与“分数飘字特效”，击杀僵尸时弹出对应分数（如“+30”），飘字随时间渐变透明并上移，提升视觉反馈与成就感，代码示例：
```cpp
// 分数飘字添加逻辑
void PVZWinApp::AddScorePopup(int points, int x, int y) {
    ScorePopup popup;
    popup.points = points;
    popup.x = x - 50;
    popup.y = y + 100;
    popup.lifeTime = 0;
    popup.maxLifeTime = 10;
    popup.alpha = 1.0;
    scorePopups.push_back(popup);
}
```
<img width="800" height="257" alt="image" src="https://github.com/user-attachments/assets/5152ac2c-a2ab-4025-8910-360702baa180" />

### 2.3 性能与开发效率优化
#### 2.3.1 代码重构与模块化
- **模式参数封装**：将冷却时间比例、调试显示开关等参数封装到`GameModeConfig`单例类，避免全局变量分散，提升代码可维护性。
- **调试功能模块化**：将Test Mode的坐标显示、帧率计算、僵尸HP统计等调试逻辑提取到`DebugDisplay`类，避免`PVZView`类职责过重，示例：
```cpp
// 调试显示模块化
class DebugDisplay {
public:
    static void draw(CDC* cDC) {
        if (!GameModeConfig::getInstance().getEnableDebugDisplay()) return;
        // 绘制坐标、帧率等调试信息
        CString coordText;
        coordText.Format(_T("坐标:(%d,%d)"), pos.x, pos.y);
        cDC->TextOut(10, 50, coordText);
    }
};
```

#### 2.3.2 性能优化
- **资源缓存**：`ResourceManage`类预加载所有图形资源，避免重复读取文件，减少IO耗时。
- **懒加载机制**：Test Mode下仅初始化调试相关模块（如`DebugDisplay`），Player Mode不加载冗余功能，降低内存占用。
- **动画帧控制**：通过`nextAnimateTick`方法统一管理动画帧切换，确保三模式下帧率稳定在28-30FPS，响应时间<300ms。


## 三、优化改进价值总结
项目的优化改进并非单纯“增加功能”，而是围绕“体验提升”与“效率提升”双目标：
1. **玩家体验层面**：Simple Player Mode降低入门门槛，让新手快速上手；植物功能增强（多子弹、范围冰冻）与分数特效，提升玩法趣味性与反馈感；多分辨率适配确保不同设备的流畅体验。
2. **开发测试层面**：Test Mode支持快速生成测试对象、显示调试信息，避免开发中反复“等待开局-触发场景”的低效流程，三模式参数封装也为后续功能扩展（如新增难度、植物）奠定基础。
3. **技术实践层面**：通过模块化重构、性能优化，验证了MFC框架下“复杂游戏逻辑+多模式适配”的实现方案，为同类MFC游戏开发提供参考。
