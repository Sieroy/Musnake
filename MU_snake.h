#pragma once

#include "SDL.h"

#include "MU_declaration.h"
#include "MU_element.h"
#include "MU_game.h"
#include "MU_time.h"
#include "MU_flame.h"
#include "MU_grid.h"

/* �ߵ����峯��ö�� */

/* �ߵ������� */
class musnake::Snake: public Element {
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
	void endTail(bool delay=true);  // ʵ���Եĺ�����������û�м�Ч��

private:
	int headDir = MU_SNAKE_DIRECT_NONE;  // ��ͷ��ĳ���
	int tailDir = MU_SNAKE_DIRECT_NONE;  // ��β��ĳ���

	Snake* next = nullptr;  // ��һ�Σ�β������
	Snake* prev = nullptr;  // ��һ�Σ�ͷ������

	Grid* grid = nullptr;  // ���嵱ǰ�����ĸ���
};

inline musnake::Snake::Snake(){

}

inline musnake::Snake::Snake(int tailDir) {
	Snake::tailDir = tailDir;

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

inline void musnake::Snake::setHeadDir(int dir) {
	headDir = dir;
}

inline void musnake::Snake::setTailDir(int dir) {
	tailDir = dir;
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
	setNext(nullptr);
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
