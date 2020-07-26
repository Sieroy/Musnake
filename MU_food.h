#pragma once

#include "MU_declaration.h"
#include "MU_flame.h"
#include "MU_grid.h"

class musnake::Food {  // 忽然想写个基类了。。。算了，重构时再说~
public:
	Food();
	~Food();

	// 设置帧组
	void setFlame(Flame* flame);
	// 指明所在地格
	void setGrid(Grid* grid);

	void update();
	void draw(SDL_Renderer* render);

private:
	SDL_Rect rect;
	Grid* grid;
	Flame* flame;
	long long flameTime;
	long long duration;  // 持续的时间。为-1时则永存，否则就是限时食物
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
