#pragma once

#include "SDL.h"

#include "MU_declaration.h"
#include "MU_game.h"
#include "MU_time.h"
#include "MU_flame.h"
#include "MU_grid.h"

/* �ߵ����峯��ö�� */

/* �ߵ������� */
class musnake::Snake {
public:
	// ֱ����һ����
	Snake();
	// ָ��β�򣬽�����Ϊ��ͷ����ʼ��
	Snake(int tailDir);
	~Snake();

	// ������ͷ����
	void setHeadDir(int dir);

	// ������β����
	void setTailDir(int dir);

	// ֱ�Ӹı�����ĵ�ǰ֡
	void setFlame(Flame* flame);

	// ������������
	void setNext(Snake* snake);
	void setPrev(Snake* snake);

	// �������ڵؿ顣����ı�ؿ������
	void setGrid(Grid* grid);

	// ��ȡ��������
	Snake* getNext();
	Snake* getPrev();

	// ��ȡ����ؿ�
	Grid* getGrid();

	// ��ȡ�߷���
	int getHeadDir();
	int getTailDir();

	// ��ͷ������
	void turnBody();  // ʵ���Եĺ�����������û�м�Ч��
	// ��ͷ�����壬ָ������İ汾
	void turnBody(int headDir);  // ʵ���Եĺ�����������û�м�Ч��

	// �������β
	void turnTail();  // ʵ���Եĺ�����������û�м�Ч��

	// �Զ���ʱβ��������ҡ��
	void shakeTail();  // ʵ���Եĺ�����������û�м�Ч��

	// β����ǰ������ʧ��һ����ͼ��
	void endTail();  // ʵ���Եĺ�����������û�м�Ч��

	// ��������
	void update();

	// ��������ͣ�ȶ��Ƶ���flameTime
	void refreshTime(int delta);

	// ��������
	void draw(SDL_Renderer* render);

private:
	int headDir;  // ��ͷ��ĳ���
	int tailDir;  // ��β��ĳ���

	SDL_Rect rect;  // �������
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

inline musnake::Snake::Snake(int tailDir) {
	headDir = MU_SNAKE_DIRECT_NONE;
	Snake::tailDir = tailDir;
	next = nullptr;
	prev = nullptr;
	grid = nullptr;

	switch (tailDir) {
	case MU_SNAKE_DIRECT_UP:
		setFlame(snakeFlame[MU_SNAKE_FLAME_HEAD_0toUP]);
		break;
	case MU_SNAKE_DIRECT_RIGHT:
		setFlame(snakeFlame[MU_SNAKE_FLAME_HEAD_0toRIGHT]);
		break;
	case MU_SNAKE_DIRECT_DOWN:
		setFlame(snakeFlame[MU_SNAKE_FLAME_HEAD_0toDOWN]);
		break;
	case MU_SNAKE_DIRECT_LEFT:
		setFlame(snakeFlame[MU_SNAKE_FLAME_HEAD_0toLEFT]);
		break;
	}
}

inline musnake::Snake::~Snake() {
	if (next) {
		delete next;
	}
	/* ��Ϊһ����ԣ�ֻ����β�Ż�����Ϸ�������Զ����ͷţ��������������β��Ҳ������next��
	 * ��ôֻ������Ϸ����ʱ�Ĵ��ͷţ���ʱ��Ӧ�õݹ�ذ���Щ����ȫ�ͷŵ� */
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

inline musnake::Snake* musnake::Snake::getNext() {
	return next;
}

inline musnake::Snake* musnake::Snake::getPrev() {
	return prev;
}

inline musnake::Grid* musnake::Snake::getGrid() {
	return grid;
}

inline int musnake::Snake::getHeadDir() {
	return headDir;
}

inline int musnake::Snake::getTailDir() {
	return tailDir;
}

inline void musnake::Snake::turnBody() {
	switch (tailDir) {  // ��ʵ��������ͷ������λ��β��ģ�����������֡����ֺ��Ѷ��룬���Ծ��оٰɡ���
	case MU_SNAKE_DIRECT_UP:
		switch (headDir) {
		case MU_SNAKE_DIRECT_RIGHT:
			setFlame(snakeFlame[MU_SNAKE_FLAME_HEAD_UPtoRIGHT]);
			break;
		case MU_SNAKE_DIRECT_DOWN:
			setFlame(snakeFlame[MU_SNAKE_FLAME_HEAD_UPtoDOWN]);
			break;
		case MU_SNAKE_DIRECT_LEFT:
			setFlame(snakeFlame[MU_SNAKE_FLAME_HEAD_UPtoLEFT]);
			break;
		}
		break;

	case MU_SNAKE_DIRECT_RIGHT:
		switch (headDir) {
		case MU_SNAKE_DIRECT_UP:
			setFlame(snakeFlame[MU_SNAKE_FLAME_HEAD_RIGHTtoUP]);
			break;
		case MU_SNAKE_DIRECT_DOWN:
			setFlame(snakeFlame[MU_SNAKE_FLAME_HEAD_RIGHTtoDOWN]);
			break;
		case MU_SNAKE_DIRECT_LEFT:
			setFlame(snakeFlame[MU_SNAKE_FLAME_HEAD_RIGHTtoLEFT]);
			break;
		}
		break;

	case MU_SNAKE_DIRECT_DOWN:
		switch (headDir) {
		case MU_SNAKE_DIRECT_UP:
			setFlame(snakeFlame[MU_SNAKE_FLAME_HEAD_DOWNtoUP]);
			break;
		case MU_SNAKE_DIRECT_RIGHT:
			setFlame(snakeFlame[MU_SNAKE_FLAME_HEAD_DOWNtoRIGHT]);
			break;
		case MU_SNAKE_DIRECT_LEFT:
			setFlame(snakeFlame[MU_SNAKE_FLAME_HEAD_DOWNtoLEFT]);
			break;
		}
		break;

	case MU_SNAKE_DIRECT_LEFT:
		switch (headDir) {
		case MU_SNAKE_DIRECT_UP:
			setFlame(snakeFlame[MU_SNAKE_FLAME_HEAD_LEFTtoUP]);
			break;
		case MU_SNAKE_DIRECT_RIGHT:
			setFlame(snakeFlame[MU_SNAKE_FLAME_HEAD_LEFTtoRIGHT]);
			break;
		case MU_SNAKE_DIRECT_DOWN:
			setFlame(snakeFlame[MU_SNAKE_FLAME_HEAD_LEFTtoDOWN]);
			break;
		}
		break;
	}
}

inline void musnake::Snake::turnBody(int headDir) {
	Snake::headDir = headDir;
	turnBody();
}

inline void musnake::Snake::turnTail() {
	switch (tailDir) {  // ��ʵ��������ͷ������λ��β��ģ�����������֡����ֺ��Ѷ��룬���Ծ��оٰɡ���
	case MU_SNAKE_DIRECT_UP:
		switch (headDir) {
		case MU_SNAKE_DIRECT_RIGHT:
			setFlame(snakeFlame[MU_SNAKE_FLAME_TAIL_UPtoRIGHT]);
			break;
		case MU_SNAKE_DIRECT_DOWN:
			setFlame(snakeFlame[MU_SNAKE_FLAME_TAIL_UPtoDOWN]);
			break;
		case MU_SNAKE_DIRECT_LEFT:
			setFlame(snakeFlame[MU_SNAKE_FLAME_TAIL_UPtoLEFT]);
			break;
		}
		break;

	case MU_SNAKE_DIRECT_RIGHT:
		switch (headDir) {
		case MU_SNAKE_DIRECT_UP:
			setFlame(snakeFlame[MU_SNAKE_FLAME_TAIL_RIGHTtoUP]);
			break;
		case MU_SNAKE_DIRECT_DOWN:
			setFlame(snakeFlame[MU_SNAKE_FLAME_TAIL_RIGHTtoDOWN]);
			break;
		case MU_SNAKE_DIRECT_LEFT:
			setFlame(snakeFlame[MU_SNAKE_FLAME_TAIL_RIGHTtoLEFT]);
			break;
		}
		break;

	case MU_SNAKE_DIRECT_DOWN:
		switch (headDir) {
		case MU_SNAKE_DIRECT_UP:
			setFlame(snakeFlame[MU_SNAKE_FLAME_TAIL_DOWNtoUP]);
			break;
		case MU_SNAKE_DIRECT_RIGHT:
			setFlame(snakeFlame[MU_SNAKE_FLAME_TAIL_DOWNtoRIGHT]);
			break;
		case MU_SNAKE_DIRECT_LEFT:
			setFlame(snakeFlame[MU_SNAKE_FLAME_TAIL_DOWNtoLEFT]);
			break;
		}
		break;

	case MU_SNAKE_DIRECT_LEFT:
		switch (headDir) {
		case MU_SNAKE_DIRECT_UP:
			setFlame(snakeFlame[MU_SNAKE_FLAME_TAIL_LEFTtoUP]);
			break;
		case MU_SNAKE_DIRECT_RIGHT:
			setFlame(snakeFlame[MU_SNAKE_FLAME_TAIL_LEFTtoRIGHT]);
			break;
		case MU_SNAKE_DIRECT_DOWN:
			setFlame(snakeFlame[MU_SNAKE_FLAME_TAIL_LEFTtoDOWN]);
			break;
		}
		break;
	}

	setTailDir(MU_SNAKE_DIRECT_NONE);
}

inline void musnake::Snake::shakeTail() {
	switch (headDir) {
	case MU_SNAKE_DIRECT_UP:
		setFlame(snakeFlame[MU_SNAKE_FLAME_TAIL_UPshake]);
		break;
	case MU_SNAKE_DIRECT_RIGHT:
		setFlame(snakeFlame[MU_SNAKE_FLAME_TAIL_RIGHTshake]);
		break;
	case MU_SNAKE_DIRECT_DOWN:
		setFlame(snakeFlame[MU_SNAKE_FLAME_TAIL_DOWNshake]);
		break;
	case MU_SNAKE_DIRECT_LEFT:
		setFlame(snakeFlame[MU_SNAKE_FLAME_TAIL_LEFTshake]);
		break;
	}
}

inline void musnake::Snake::update() {
	if (flameTime < 0) return;  // ��������֡�����ٸ���
	flameTime -= getTimeDelta();
	if (flameTime <= 0) {
		if (flame) flame = flame->getNext();
		if (flame) {
		__next:
			// if (flame->getDuration() < 0) break;
			flameTime += flame->getDuration();
			if (flameTime <= 0 && flame->getDuration() > 0) goto __next;
		}
		else
			;
		// һ������Ӧ����תͷ��˦β����������ʱ�򴥷��ģ�������������ʱ����һֱ֡�Ӿ����õ���̬�����ˡ���
		// ���ԣ��о���̫������������Ǿ����������֧ͼһ�֣�
	}
}

inline void musnake::Snake::refreshTime(int delta) {
	flameTime += delta;
}

inline void musnake::Snake::draw(SDL_Renderer* render) {
	if(flame) flame->draw(render, &rect);
}
