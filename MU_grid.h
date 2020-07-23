#pragma once

#include "MU_snake.h"

namespace musnake {
	// 格子内物品种类
	enum GridObjectType {
		MU_GRID_OBJECT_TYPE_DARK = -1,  // 类似空气墙的东西
		MU_GRID_OBJECT_TYPE_EMPTY = 0,  // 空格
		MU_GRID_OBJECT_TYPE_SNAKE,  // 蛇体
		MU_GRID_OBJECT_TYPE_BLOCK,  // 障碍物
		MU_GRID_OBJECT_TYPE_FOOD,  // 食物
	};
}

// 地图格
class musnake::Grid {
public:
	Grid();
	~Grid();

	SDL_Rect rect;
	int objType;

	void setPosition(int x, int y, int w, int h);

	void update();
	void draw(SDL_Renderer* render);

private:
	Snake* snake;

	Flame* flame;
	long long flameTime;


};

musnake::Grid::Grid() {
	objType = MU_GRID_OBJECT_TYPE_EMPTY;
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
	if (objType == GridObjectType::MU_GRID_OBJECT_TYPE_SNAKE)snake->update();
}

inline void musnake::Grid::draw(SDL_Renderer* render) {
	flame->draw(render, &rect);
	if (objType == GridObjectType::MU_GRID_OBJECT_TYPE_SNAKE)snake->draw(render);
}
