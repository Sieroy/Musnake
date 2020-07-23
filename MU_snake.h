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

/* 蛇的身体朝向枚举 */

/* 蛇的身体类 */
class musnake::Snake {
public:
	Snake();
	~Snake();

	// 设置蛇头向方向
	void setHeadDir(int dir);

	// 设置蛇尾向方向
	void setTailDir(int dir);

	// 设置蛇体位置
	void setPosition(int x, int y, int w, int h);

	// 直接改变蛇体的当前帧
	void setFlame(Flame* flame);

	// 设置相邻蛇体
	void setNext(Snake* snake);
	void setPrev(Snake* snake);

	// 蛇头变蛇体
	void turnBody();  // 实验性的函数，试试有没有简化效果

	// 蛇体变蛇尾
	void turnTail();  // 实验性的函数，试试有没有简化效果

	// 更新蛇体
	void update();

	// 绘制蛇体
	void draw(SDL_Renderer* render);

private:
	int headDir;  // 蛇头向的朝向
	int tailDir;  // 蛇尾向的朝向

	Flame* flame;  // 蛇体当前帧
	long long flameTime;  // 蛇体当前帧剩余持续时间

	Snake* next;  // 下一段（尾向）蛇体
	Snake* prev;  // 上一段（头向）蛇体

	Grid* grid;  // 蛇体当前所处的格子
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
	if (flameTime < 0) return;  // 对于永续帧，不再更新
	flameTime -= getTimeDelta();
	if (flameTime <= 0) {
		flame = flame->getNext();
		if (flame)
			flameTime += flame->getDuration();
		else
			;
		// 一般这里应该是转头或甩尾动画结束的时候触发的，不过动画结束时，下一帧直接就设置到静态动画了。。
		// 所以，感觉不太可能来到这里，那就留着这个分支图一乐？
	}
}

inline void musnake::Snake::draw(SDL_Renderer* render) {
	flame->draw(render, &grid->rect);
}
