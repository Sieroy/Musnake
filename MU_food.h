#pragma once

#include "MU_declaration.h"
#include "MU_element.h"
#include "MU_grid.h"

class musnake::Food :public Element{
public:
	Food(int type);
	~Food();

	void setGrid(Grid* grid);
	Grid* getGrid();

	void update();

private:
	Grid* grid = nullptr;
	long long duration = -1;
};

inline musnake::Food::Food(int type) {
	setFlame(foodFlame[type]);
}

inline musnake::Food::~Food() {

}
