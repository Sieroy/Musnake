#pragma once

#include "MU_declaration.h"
#include "MU_snake.h"


// ��ͼ��
class musnake::Grid {
public:
	Grid();
	~Grid();

	SDL_Rect rect;
	int objType;

	void setPosition(int x, int y, int w, int h);

	// ���õؿ�����Ϊһ���ߣ�������ߵ�setGrid
	void setSnake(Snake* snake);

	Snake* popSnake();

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

inline void musnake::Grid::setSnake(Snake* snake) {
	objType = MU_GRID_OBJECT_TYPE_SNAKE;
	Grid::snake = snake;
	snake->setGrid(this);
}

inline musnake::Snake* musnake::Grid::popSnake() {
	Snake* sp = snake;
	snake = nullptr;
	objType = MU_GRID_OBJECT_TYPE_EMPTY;
	return sp;
}

inline void musnake::Grid::update() {
	if (flameTime >= 0) {  // ���ظ��֡�ø����ˣ���ֱ�Ӹ���
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
