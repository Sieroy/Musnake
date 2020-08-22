#pragma once

#include"SDL.h"

#include"MU_declaration.h"
#include"MU_flame.h"

class musnake::Element {
public:
	void setPosition(int x, int y, int w, int h);
	void setPosition(SDL_Rect* rect);
	void setFlame(Flame* flame);

	void delayFlameTime(long long delta);  // ������ͣ�ȶ����µ��ӳٴ���

	void update();
	void draw();
	void draw(SDL_Point* base);  // ָ�����ƿ�ʼλ�á�

protected:
	SDL_Rect rect = { 0,0,0,0 };  // �������
	Flame* flame = nullptr;  // ���嵱ǰ֡
	long long flameTime = -1;  // ���嵱ǰ֡ʣ�����ʱ��
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
	if (flameTime < 0) return;  // ��������֡�����ٸ���
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
