#pragma once

#include "MU_declaration.h"
#include "MU_flame.h"
#include "MU_grid.h"

class musnake::Food {  // ��Ȼ��д�������ˡ��������ˣ��ع�ʱ��˵~
public:
	Food();
	~Food();

	// ����֡��
	void setFlame(Flame* flame);
	// ָ�����ڵظ�
	void setGrid(Grid* grid);

	void update();
	void draw(SDL_Renderer* render);

private:
	SDL_Rect rect;
	Grid* grid;
	Flame* flame;
	long long flameTime;
	long long duration;  // ������ʱ�䡣Ϊ-1ʱ�����棬���������ʱʳ��
};

inline musnake::Food::Food() {
	
}

inline musnake::Food::~Food() {

}

inline void musnake::Food::setFlame(Flame* flame) {
	this->flame = flame;
	flameTime = flame->getDuration();
}

inline void musnake::Food::draw(SDL_Renderer* render) {
	if (flame) flame->draw(render, &rect);
}
