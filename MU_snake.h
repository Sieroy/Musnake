#pragma once

#include "SDL.h"

#include "MU_time.h"
#include "MU_flame.h"
#include "MU_grid.h"

namespace musnake {
	enum BodyDirect;
	class Snake;
}

/* �ߵ����峯��ö�� */
enum musnake::BodyDirect {
	none  = -1,
	up = 0,
	right,
	down,
	left
};

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

	// ��������
	void update();

	// ��������
	void draw(SDL_Renderer* render);

private:
	SDL_Rect rect;  // ��������λ��
	int headDir;  // ��ͷ��ĳ���
	int tailDir;  // ��β��ĳ���

	Flame* flame;  // ���嵱ǰ֡
	long long flameTime;  // ���嵱ǰ֡ʣ�����ʱ��

	Snake* next;  // ��һ�Σ�β������
	Snake* prev;  // ��һ�Σ�ͷ������

	Grid* grid;  // ���嵱ǰ�����ĸ���
};

inline musnake::Snake::Snake(){
	headDir = none;
	tailDir = none;
	flame = nullptr;
	flameTime = 0;
	next = nullptr;
	prev = nullptr;
}

inline void musnake::Snake::setHeadDir(int dir) {
	headDir = dir;
}

inline void musnake::Snake::setTailDir(int dir) {
	tailDir = dir;
}

inline void musnake::Snake::setPosition(int x, int y, int w, int h) {
	rect.x = x;
	rect.y = y;
	rect.w = w;
	rect.h = h;
}

inline void musnake::Snake::setFlame(Flame* flame) {
	Snake::flame = flame;
	flameTime = flame->getDuration();
}

inline void musnake::Snake::update() {
	if (flameTime < 0) return;  // ��������֡�����ٸ���
	unsigned long long dt = getTimeDelta();
	flameTime -= dt;
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
	flame->draw(render, &rect);
}
