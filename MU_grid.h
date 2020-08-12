#pragma once

#include "MU_declaration.h"
#include "MU_snake.h"
#include "MU_food.h"


// ��ͼ��
class musnake::Grid :public Element {
public:
	Grid();
	// ��ʼ��������֪�õظ��������ĵظ�����
	Grid(int x, int y);
	~Grid();

	int x, y;
	int objType;

	// ���õؿ�����Ϊһ���ߣ�������ߵ�setGrid��ʹ��ǰ��ע��ظ���ԭ��������
	void setSnake(Snake* snake);
	// ���õؿ�����Ϊʳ������ʳ���setGrid��ʹ��ǰ��ע��ظ���ԭ��������
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
	// �벻����Ҫдʲô�����������ɡ���
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
	if( objType == MU_GRID_OBJECT_TYPE_BLOCK ) {
		SDL_Rect r = rect;
		r.x += base->x;
		r.y += base->y - 20;
		r.h = 60;
		gridBlockFlame->draw(render, &r);
		return;
	}
	if (objType == MU_GRID_OBJECT_TYPE_DARK) {
		SDL_Rect r = rect;
		r.x += base->x;
		r.y += base->y - 20;
		r.h = 60;
		gridDarkFlame->draw(render, &r);
		return;
	}
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

	//if (duration > 0) {  // �������ʱʳ��
	//	duration -= getTimeDelta();
	//	if (duration <= 0) {
	//		grid->setFood(nullptr);
	//		// ��ʱ�Ȳ�������ʾʳ��ȣ����滹Ҫ�и��ֳ���
	//	}
	//}
}
