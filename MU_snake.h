#pragma once

#include "SDL.h"

#include "MU_declaration.h"
#include "MU_element.h"
#include "MU_game.h"
#include "MU_time.h"
#include "MU_flame.h"
#include "MU_grid.h"

/* 蛇的身体朝向枚举 */

/* 蛇的身体类 */
class musnake::Snake: public Element {
public:
	// 直接造一个蛇
	Snake();
	// 指定尾向，将蛇作为蛇头而初始化
	Snake(int tailDir);
	~Snake();

	// 设置蛇头向方向
	void setHeadDir(int dir);

	// 设置蛇尾向方向
	void setTailDir(int dir);

	// 设置相邻蛇体
	void setNext(Snake* snake);
	void setPrev(Snake* snake);

	// 设置所在地块。不会改变地块的数据
	void setGrid(Grid* grid);

	// 获取相邻蛇体
	Snake* getNext();
	Snake* getPrev();

	// 获取蛇体地块
	Grid* getGrid();

	// 获取蛇方向
	int getHeadDir();
	int getTailDir();

	// 蛇头变蛇体
	void turnBody();  // 实验性的函数，试试有没有简化效果
	// 蛇头变蛇体，指定方向的版本
	void turnBody(int headDir);  // 实验性的函数，试试有没有简化效果

	// 蛇体变蛇尾
	void turnTail();  // 实验性的函数，试试有没有简化效果

	// 吃东西时尾巴增长的摇摆
	void shakeTail();  // 实验性的函数，试试有没有简化效果

	// 尾巴因前进而消失于一个地图块
	void endTail(bool delay=true);  // 实验性的函数，试试有没有简化效果

private:
	int headDir = MU_SNAKE_DIRECT_NONE;  // 蛇头向的朝向
	int tailDir = MU_SNAKE_DIRECT_NONE;  // 蛇尾向的朝向

	Snake* next = nullptr;  // 下一段（尾向）蛇体
	Snake* prev = nullptr;  // 上一段（头向）蛇体

	Grid* grid = nullptr;  // 蛇体当前所处的格子
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
	switch (tailDir) {  // 其实本来是想头向左移位或尾向的，但昨天排了帧组后发现很难对齐，所以就列举吧……
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
	switch (tailDir) {  // 其实本来是想头向左移位或尾向的，但昨天排了帧组后发现很难对齐，所以就列举吧……
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
