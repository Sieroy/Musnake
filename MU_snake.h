#pragma once

#include "SDL.h"

#include "MU_time.h"
#include "MU_flame.h"
#include "MU_grid.h"

namespace musnake {
	enum SnakeDirect {
		MU_SNAKE_DIRECT_NONE = -1,
		MU_SNAKE_DIRECT_UP = 0,
		MU_SNAKE_DIRECT_RIGHT,
		MU_SNAKE_DIRECT_DOWN,
		MU_SNAKE_DIRECT_LEFT
	};
	class Grid;
	class Snake;
}

/* �ߵ����峯��ö�� */

/* �ߵ������� */
class musnake::Snake {
public:
	Snake();
	~Snake();

	// ������ͷ����
	void setHeadDir(int dir);

	// ������β����
	void setTailDir(int dir);

	// ��������λ��
	void setPosition(int x, int y, int w, int h);

	// ֱ�Ӹı�����ĵ�ǰ֡
	void setFlame(Flame* flame);

	// ������������
	void setNext(Snake* snake);
	void setPrev(Snake* snake);

	// ��ͷ������
	void turnBody();  // ʵ���Եĺ�����������û�м�Ч��

	// �������β
	void turnTail();  // ʵ���Եĺ�����������û�м�Ч��

	// ��������
	void update();

	// ��������
	void draw(SDL_Renderer* render);

private:
	int headDir;  // ��ͷ��ĳ���
	int tailDir;  // ��β��ĳ���

	Flame* flame;  // ���嵱ǰ֡
	long long flameTime;  // ���嵱ǰ֡ʣ�����ʱ��

	Snake* next;  // ��һ�Σ�β������
	Snake* prev;  // ��һ�Σ�ͷ������

	Grid* grid;  // ���嵱ǰ�����ĸ���
};

inline musnake::Snake::Snake(){
	headDir = MU_SNAKE_DIRECT_NONE;
	tailDir = MU_SNAKE_DIRECT_NONE;
	flame = nullptr;
	flameTime = 0;
	next = nullptr;
	prev = nullptr;
	grid = nullptr;
}

inline void musnake::Snake::setHeadDir(int dir) {
	headDir = dir;
}

inline void musnake::Snake::setTailDir(int dir) {
	tailDir = dir;
}

inline void musnake::Snake::setFlame(Flame* flame) {
	Snake::flame = flame;
	flameTime = flame->getDuration();
}

inline void musnake::Snake::setNext(Snake* snake) {
	next = snake;
}

inline void musnake::Snake::setPrev(Snake* snake) {
	prev = snake;
}

inline void musnake::Snake::update() {
	if (flameTime < 0) return;  // ��������֡�����ٸ���
	flameTime -= getTimeDelta();
	if (flameTime <= 0) {
		flame = flame->getNext();
		if (flame)
			flameTime += flame->getDuration();
		else
			;
		// һ������Ӧ����תͷ��˦β����������ʱ�򴥷��ģ�������������ʱ����һֱ֡�Ӿ����õ���̬�����ˡ���
		// ���ԣ��о���̫������������Ǿ����������֧ͼһ�֣�
	}
}

inline void musnake::Snake::draw(SDL_Renderer* render) {
	flame->draw(render, &grid->rect);
}
