#pragma once
// 主循环计时器ID
#define MAIN_LOOP_TIMER WM_USER + 10001
#define ANIMATION_LOOP_TIMER WM_USER + 10002
#define GAME_TICK_LOOP WM_USER + 10003
#define ATTACK_TICK_LOOP WM_USER + 10004
// 设置游戏帧率
#define FLASH_TICK  1000 / 200
#define FPS         1000 / 30
#define GAME_TICK   1000 / 100//50
// 窗口大小 px
#define WIDTH 900
#define HEIGHT 700

// 窗口滑动比例
#define SLIDING_WINDOW 0
// 内容显示比例 
#define ZOOM_FACTOR 0.5
