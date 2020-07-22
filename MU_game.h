#pragma once

#include "SDL.h"

#include "MU_grid.h"
#include "MU_snake.h"

namespace musnake{
	// 游戏状态枚举
	enum GameState {
		running,
		loading,
		paused,
		over
	};

	class Game;
}

class musnake::Game {
public:
	Game();
	~Game();

	int state;

	Grid* map[64][64];  // 初始就先64x64吧、、

private:
	SDL_Rect drawRect;  // 当前屏幕绘制区域对应的完整地图的矩形
};

musnake::Game::Game() {
	// 按说这里应该是从配置文件里读数据来初始化地格的，现阶段就先色块填充吧，毕竟Sprite图还没画完
	state = loading;

	for (int i = 0;i < 64;i++) {
		for (int j = 0;j < 64;j++) {
			map[i][j] = new Grid;
			map[i][j]->setPosition(j * 40, i * 40, 40, 40);
		}
	}
}
