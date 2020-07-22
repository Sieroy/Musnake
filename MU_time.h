#pragma once

#include"SDL.h"

namespace musnake {

	typedef struct _DelayFunc {
		void (*func)();
		unsigned long long time;
		struct _DelayFunc* next;
	}DelayFunc;

	/* 将函数插入到指定的延时函数表中 */
	void addDelayFunc(DelayFunc** list, void (*func)(), long long delay);
	/* 轮流触发到达时间的延时函数 */
	void triggerDelayFunc(DelayFunc** list);
	/* 清空目标表内的延时函数 */
	void clearDelayFunc(DelayFunc** list);

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

inline void musnake::addDelayFunc(DelayFunc** list, void (*func)(), long long delay) {
	DelayFunc* ndf = new DelayFunc;
	ndf->func = func;
	ndf->time = delay + getTimeVal();

	if (!*list) {  // 如果延时表是空的
		*list = ndf;
		ndf->next = nullptr;
	}
	else {
		if (ndf->time < (*list)->time) {  // 如果新延时值比其他的都要小
			ndf->next = *list;
			*list = ndf;
		}
		else {
			DelayFunc* np = *list;
			while (np->next) {
				if (ndf->time < (*list)->next->time) {  // 如果新延时值大小介于某两个之间
					ndf->next = np->next;
					np->next = ndf;
					return;
				}
				np = np->next;
			}
			// 如果新延时值比其他的都要大
			np->next = ndf;
			ndf->next = nullptr;
		}
	}
}

inline void musnake::triggerDelayFunc(DelayFunc** list) {
	DelayFunc* np;
	
	while (np = *list) {
		if (np->time > getTimeVal()) break;
		*list = np->next;
		np->func();
		delete np;
	}
}

inline void musnake::clearDelayFunc(DelayFunc** list) {
	DelayFunc* np;

	while (np = *list) {
		*list = np->next;
		delete np;
	}
}
