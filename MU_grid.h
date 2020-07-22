#pragma once

#include "MU_snake.h"

namespace musnake {
	// ��������Ʒ����
	enum GridObjectType {
		dark = -1,  // ���ƿ���ǽ�Ķ���
		empty = 0,  // �ո�
		snake,  // ����
		block,  // ש��
		food,  // ʳ��
	};
}

// ��ͼ��
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
	if (flameTime >= 0) {  // ���ظ��֡�ø����ˣ���ֱ�Ӹ���
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
