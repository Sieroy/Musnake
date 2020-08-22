#pragma once

#include"SDL.h"

#include"MU_declaration.h"
#include"MU_flame.h"

class musnake::Element {
public:
	void setPosition(int x, int y, int w, int h);
	void setPosition(SDL_Rect* rect);
	void setFlame(Flame* flame);

	void delayFlameTime(long long delta);  // 由于暂停等而导致的延迟处理

	void update();
	void draw();
	void draw(SDL_Point* base);  // 指定绘制开始位置。

protected:
	SDL_Rect rect = { 0,0,0,0 };  // 物体矩形
	Flame* flame = nullptr;  // 物体当前帧
	long long flameTime = -1;  // 物体当前帧剩余持续时间
};

inline void musnake::Element::setFlame(Flame* flame) {
	Element::flame = flame;
	flameTime = flame->getDuration();
}

inline void musnake::Element::setPosition(int x, int y, int w, int h) {
	rect.x = x;
	rect.y = y;
	rect.w = w;
	rect.h = h;
}

inline void musnake::Element::setPosition(SDL_Rect* rect) {
	this->rect = *rect;
}

inline void musnake::Element::delayFlameTime(long long delta) {
	if(this)
		flameTime += delta;
}

inline void musnake::Element::update() {
	if (flameTime < 0) return;  // 对于永续帧，不再更新
	flameTime -= getTimeDelta();
	if (flameTime <= 0) {
		if (flame) flame = flame->getNext();
		if (flame) {
			do {
				flameTime += flame->getDuration();
			} while (flameTime <= 0 && flame->getDuration() > 0);
		}
	}
}

inline void musnake::Element::draw() {
	if (flame) flame->draw(&rect);
}

inline void musnake::Element::draw(SDL_Point* base) {
	if (flame) {
		SDL_Rect r = rect;
		if (base) {
			r.x += base->x;
			r.y += base->y;
		}
		if (r.x > MU_WINDOW_WIDTH || r.y > MU_WINDOW_HEIGHT || r.x + 40 < 0 || r.y + 40 < 0) return;
		flame->draw(&r);
	}
}
