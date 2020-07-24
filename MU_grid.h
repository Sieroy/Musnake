#pragma once

#include "MU_declaration.h"
#include "MU_snake.h"


// 地图格
class musnake::Grid {
public:
	Grid();
	// 初始化，并告知该地格它所处的地格坐标
	Grid(int x, int y);
	~Grid();

	int x, y;
	SDL_Rect rect;
	int objType;

	void setPosition(int x, int y, int w, int h);

	// 设置地块物体为一段蛇，会调用蛇的setGrid
	void setSnake(Snake* snake);

	Snake* getSnake();

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
	flame = nullptr;
	flameTime = 0;
	x = y = -1;
}

musnake::Grid::Grid(int x, int y) {
	objType = MU_GRID_OBJECT_TYPE_EMPTY;
	snake = nullptr;
	flame = nullptr;
	flameTime = 0;
	this->x = x;
	this->y = y;
}

inline void musnake::Grid::setPosition(int x, int y, int w, int h) {
	rect.x = x;
	rect.y = y;
	rect.w = w;
	rect.h = h;
}

inline void musnake::Grid::setSnake(Snake* snake) {
	if (snake) {
		objType = MU_GRID_OBJECT_TYPE_SNAKE;
		this->snake = snake;
		snake->setGrid(this);
	}
	else {
		this->snake = nullptr;
		objType = MU_GRID_OBJECT_TYPE_EMPTY;
	}
}

inline musnake::Snake* musnake::Grid::getSnake() {
	return snake;
}

inline void musnake::Grid::update() {
	if (flame) {
		if (flameTime >= 0) {  // 若地格的帧该更新了，就直接更新
			flameTime -= getTimeDelta();
			if (flameTime <= 0) {
				flame = flame->getNext();
				if (flame)
					flameTime += flame->getDuration();
			}
		}
	}
	if (objType == GridObjectType::MU_GRID_OBJECT_TYPE_SNAKE)snake->update();
}

inline void musnake::Grid::draw(SDL_Renderer* render) {
	if (flame) flame->draw(render, &rect);
	if (objType == GridObjectType::MU_GRID_OBJECT_TYPE_SNAKE)snake->draw(render);
}
