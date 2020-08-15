#pragma once

#include "MU_declaration.h"
#include "MU_element.h"
#include "MU_grid.h"

class musnake::Food :public Element{  // 忽然想写个基类了。。。算了，重构时再说~
public:
	Food(int type);
	~Food();

	// 指明所在地格
	void setGrid(Grid* grid);
	Grid* getGrid();

	void update();

private:
	Grid* grid = nullptr;
	long long duration = -1;  // 持续的时间。为-1时则永存，否则就是限时食物
};

inline musnake::Food::Food(int type) {
	setFlame(foodFlame[type]);
}

inline musnake::Food::~Food() {

}
