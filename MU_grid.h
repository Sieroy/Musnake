#pragma once

#include "MU_declaration.h"
#include "MU_snake.h"
#include "MU_food.h"


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

	// 设置地块物体为一段蛇，会调用蛇的setGrid。使用前请注意地格上原本的物体
	void setSnake(Snake* snake);
	// 设置地块物体为食物，会调用食物的setGrid。使用前请注意地格上原本的物体
	void setFood(Food* food);

	Snake* getSnake();
	Food* getFood();

	void update();
	void draw(SDL_Renderer* render);

private:
	union {
		Snake* snake;
		Food* food;
	}obj;
	Flame* flame;
	long long flameTime;


};

musnake::Grid::Grid() {
	objType = MU_GRID_OBJECT_TYPE_EMPTY;
	obj.snake = nullptr;
	flame = nullptr;
	flameTime = 0;
	x = y = -1;
}

musnake::Grid::Grid(int x, int y) {
	objType = MU_GRID_OBJECT_TYPE_EMPTY;
	obj.snake = nullptr;
	flame = nullptr;
	flameTime = 0;
	this->x = x;
	this->y = y;
}

musnake::Grid::~Grid() {
	// 想不出来要写什么，就先这样吧、、
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
		obj.snake = snake;
		snake->setGrid(this);
	}
	else {
		obj.snake = nullptr;
		objType = MU_GRID_OBJECT_TYPE_EMPTY;
	}
}

inline void musnake::Grid::setFood(Food* food) {
	if (food) {
		objType = MU_GRID_OBJECT_TYPE_FOOD;
		obj.food = food;
		food->setGrid(this);
	}
	else {
		obj.food = nullptr;
		objType = MU_GRID_OBJECT_TYPE_EMPTY;
	}
}

inline musnake::Snake* musnake::Grid::getSnake() {
	if (objType == MU_GRID_OBJECT_TYPE_SNAKE)
		return obj.snake;
	else
		return nullptr;
}

inline musnake::Food* musnake::Grid::getFood() {
	if (objType == MU_GRID_OBJECT_TYPE_FOOD)
		return obj.food;
	else
		return nullptr;
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
	if (objType == GridObjectType::MU_GRID_OBJECT_TYPE_SNAKE) obj.snake->update();
}

inline void musnake::Grid::draw(SDL_Renderer* render) {
	if (flame) flame->draw(render, &rect);
	if (objType == GridObjectType::MU_GRID_OBJECT_TYPE_SNAKE) obj.snake->draw(render);
}


inline void musnake::Snake::setGrid(Grid* grid) {
	Snake::grid = grid;
	rect = grid->rect;
}

inline void musnake::Food::setGrid(Grid* grid) {
	this->grid = grid;
	rect = grid->rect;
}

inline void musnake::Food::update() {
	if (flameTime > 0) {  // 判断切帧
		flameTime -= getTimeVal();
		if (flameTime <= 0) {
			if (flame) flame = flame->getNext();
			if (flame) flameTime += flame->getDuration();
		}
	}

	if (duration > 0) {  // 如果是限时食物
		duration -= getTimeDelta();
		if (duration <= 0) {
			grid->setFood(nullptr);
			// 暂时先不考虑显示食物叭，下面还要有各种撤除
		}
	}
}
