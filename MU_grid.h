#pragma once

#include "MU_declaration.h"
#include "MU_snake.h"
#include "MU_food.h"


class musnake::Grid :public Element {
public:
	Grid();
	Grid(int x, int y);
	~Grid();

	int x, y;
	int objType;

	void setSnake(Snake* snake);
	void setFood(Food* food);

	Snake* getSnake();
	Food* getFood();

	SDL_Rect* getRect();
	SDL_Point* getCenterPoint();

	void update();
	void draw(SDL_Point* base);

private:
	SDL_Point center;
	union {
		Snake* snake;
		Food* food;
	}obj;
};

musnake::Grid::Grid() {
	objType = MU_GRID_OBJECT_TYPE_EMPTY;
	obj.snake = nullptr;
	x = y = -1;
	setFlame(gridFlame);
}

musnake::Grid::Grid(int x, int y) {
	objType = MU_GRID_OBJECT_TYPE_EMPTY;
	obj.snake = nullptr;
	this->x = x;
	this->y = y;
	setPosition(x * 40, y * 40, 40, 40);
	center.x = x * 40 + 20;
	center.y = y * 40 + 20;
	setFlame(gridFlame);
}

musnake::Grid::~Grid() {

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

inline SDL_Rect* musnake::Grid::getRect() {
	return &rect;
}

inline SDL_Point* musnake::Grid::getCenterPoint() {
	return &center;
}

inline void musnake::Grid::update() {
	Element::update();
	if (objType == GridObjectType::MU_GRID_OBJECT_TYPE_SNAKE) obj.snake->update();
}

inline void musnake::Grid::draw(SDL_Point* base) {
	if( objType == MU_GRID_OBJECT_TYPE_BLOCK ) {
		SDL_Rect r = rect;
		r.x += base->x;
		r.y += base->y - 20;
		if (r.x > MU_WINDOW_WIDTH || r.y > MU_WINDOW_HEIGHT || r.x + 40 < 0 || r.y + 60 < 0) return;
		r.h = 60;
		gridBlockFlame->draw(&r);
		return;
	}
	if (objType == MU_GRID_OBJECT_TYPE_DARK) {
		SDL_Rect r = rect;
		r.x += base->x;
		r.y += base->y - 20;
		if (r.x > 800 || r.y > 600 || r.x + 40 < 0 || r.y + 60 < 0) return;
		r.h = 60;
		gridDarkFlame->draw(&r);
		return;
	}
	Element::draw(base);
	if (objType == MU_GRID_OBJECT_TYPE_SNAKE) obj.snake->draw(base);
	if (objType == MU_GRID_OBJECT_TYPE_FOOD) obj.food->draw(base);
}


inline void musnake::Snake::setGrid(Grid* grid) {
	this->grid = grid;
}

inline void musnake::Food::setGrid(Grid* grid) {
	this->grid = grid;
}

inline musnake::Grid* musnake::Food::getGrid() {
	return grid;
}

inline void musnake::Food::update() {
	Element::update();
}
