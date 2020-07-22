#pragma once

#include"SDL.h"

namespace musnake {

	unsigned long long timeVal = 0;
	unsigned long long timeDelta = 0;

	/* 更新时间值 */
	inline unsigned long long updateTime();

	/* 获取当前的时间值 */
	inline unsigned long long getTimeVal();

	/* 获取当前时间值与上一次更新间的时间差 */
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
