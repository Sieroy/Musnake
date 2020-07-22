#pragma once

#include "MU_snake.h"

namespace musnake {
	// 格子内物品种类
	enum GridObjectType {
		dark = -1,  // 类似空气墙的东西
		empty = 0,  // 空格
		snake,  // 蛇体
		block,  // 砖块
		food,  // 食物
	};
}

// 地图格
class musnake::Grid {
public:
	Grid();
	~Grid();

	SDL_Rect rect;

	void setPosition(int x, int y, int w, int h);

	void update();
	void draw(SDL_Renderer* render);

private:
	int objType;
	Snake* snake;

	Flame* flame;
	long long flameTime;


};

musnake::Grid::Grid() {
	objType = empty;
	snake = nullptr;
}

inline void musnake::Grid::setPosition(int x, int y, int w, int h) {
	rect.x = x;
	rect.y = y;
	rect.w = w;
	rect.h = h;
}

inline void musnake::Grid::update() {
	if (flameTime >= 0) {  // 若地格的帧该更新了，就直接更新
		flameTime -= getTimeDelta();
		if (flameTime <= 0) {
			flame = flame->getNext();
			if (flame)
				flameTime += flame->getDuration();
		}
	}
	if (objType == GridObjectType::snake)snake->update();
}

inline void musnake::Grid::draw(SDL_Renderer* render) {
	flame->draw(render, &rect);
	if (objType == GridObjectType::snake)snake->draw(render);
}
