#pragma once

#include "MU_declaration.h"
#include "MU_snake.h"
#include "MU_food.h"


// 地图格
class musnake::Grid :public Element {
public:
	Grid();
	// 初始化，并告知该地格它所处的地格坐标
	Grid(int x, int y);
	~Grid();

	int x, y;
	int objType;

	// 设置地块物体为一段蛇，会调用蛇的setGrid。使用前请注意地格上原本的物体
	void setSnake(Snake* snake);
	// 设置地块物体为食物，会调用食物的setGrid。使用前请注意地格上原本的物体
	void setFood(Food* food);

	Snake* getSnake();
	Food* getFood();

	void update();
	void draw(SDL_Renderer* render, SDL_Point* base);

private:
	union {
		Snake* snake;
		Food* food;
	}obj;
};

musnake::Grid::Grid() {
	objType = MU_GRID_OBJECT_TYPE_EMPTY;
	obj.snake = nullptr;
	x = y = -1;
}

musnake::Grid::Grid(int x, int y) {
	objType = MU_GRID_OBJECT_TYPE_EMPTY;
	obj.snake = nullptr;
	this->x = x;
	this->y = y;
}

musnake::Grid::~Grid() {
	// 想不出来要写什么，就先这样吧、、
}

inline void musnake::Grid::setSnake(Snake* snake) {
	if (snake) {
		objType = MU_GRID_OBJECT_TYPE_SNAKE;
		obj.snake = snake;
		snake->setGrid(this);
		snake->setPosition(&rect);
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
		food->setPosition(&rect);
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
	Element::update();
	if (objType == GridObjectType::MU_GRID_OBJECT_TYPE_SNAKE) obj.snake->update();
}

inline void musnake::Grid::draw(SDL_Renderer* render, SDL_Point* base) {
	Element::draw(render, base);
	if (objType == MU_GRID_OBJECT_TYPE_SNAKE) obj.snake->draw(render, base);
	if (objType == MU_GRID_OBJECT_TYPE_FOOD) obj.food->draw(render, base);
}


inline void musnake::Snake::setGrid(Grid* grid) {
	this->grid = grid;
}

inline void musnake::Food::setGrid(Grid* grid) {
	this->grid = grid;
}

inline void musnake::Food::update() {
	Element::update();

	//if (duration > 0) {  // 如果是限时食物
	//	duration -= getTimeDelta();
	//	if (duration <= 0) {
	//		grid->setFood(nullptr);
	//		// 暂时先不考虑显示食物叭，下面还要有各种撤除
	//	}
	//}
}
