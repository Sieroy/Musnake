#pragma once

#include "MU_declaration.h"
#include "MU_element.h"
#include "MU_grid.h"

class musnake::Food :public Element{  // ��Ȼ��д�������ˡ��������ˣ��ع�ʱ��˵~
public:
	Food(int type);
	~Food();

	// ָ�����ڵظ�
	void setGrid(Grid* grid);
	Grid* getGrid();

	void update();

private:
	Grid* grid = nullptr;
	long long duration = -1;  // ������ʱ�䡣Ϊ-1ʱ�����棬���������ʱʳ��
};

inline musnake::Food::Food(int type) {
	setFlame(foodFlame[type]);
}

inline musnake::Food::~Food() {

}
