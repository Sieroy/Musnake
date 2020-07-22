#pragma once

#include"SDL.h"

namespace musnake {

	unsigned long long timeVal = 0;
	unsigned long long timeDelta = 0;

	/* ����ʱ��ֵ */
	inline unsigned long long updateTime();

	/* ��ȡ��ǰ��ʱ��ֵ */
	inline unsigned long long getTimeVal();

	/* ��ȡ��ǰʱ��ֵ����һ�θ��¼��ʱ��� */
	inline unsigned long long getTimeDelta();
}

inline unsigned long long musnake::updateTime() {
	unsigned long long tv = SDL_GetTicks();
	timeDelta = tv - timeVal;
	timeVal = tv;
	return tv;
}

inline unsigned long long musnake::getTimeVal() {
	return timeVal;
}

inline unsigned long long musnake::getTimeDelta() {
	return timeDelta;
}
