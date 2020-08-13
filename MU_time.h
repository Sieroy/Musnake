#pragma once

#include"SDL.h"

#include"MU_declaration.h"

inline unsigned long long musnake::updateTime() {
	static unsigned flameCount = 0;
	static unsigned timeCount = 0;
	unsigned long long tv = SDL_GetTicks();
	timeDelta = tv - timeVal;
	timeVal = tv;
	flameCount++;
	timeCount += timeDelta;
	if (timeCount > 1000) {
		timeCount %= 1000;
		fps = flameCount;
		flameCount = 0;
	}
	return tv;
}

inline unsigned long long musnake::getTimeVal() {
	return timeVal;
}

inline unsigned long long musnake::getTimeDelta() {
	return timeDelta;
}

inline void musnake::addDelayFunc(DelayFunc** list, void (*func)(unsigned long), unsigned long arg, long long delay) {
	DelayFunc* ndf = new DelayFunc;
	ndf->func = func;
	ndf->arg = arg;
	ndf->time = delay + getTimeVal();

	if (!*list) {  // �����ʱ���ǿյ�
		*list = ndf;
		ndf->next = nullptr;
	}
	else {
		if (ndf->time <= (*list)->time) {  // �������ʱֵ�������Ķ�ҪС
			ndf->next = *list;
			*list = ndf;
		}
		else {
			DelayFunc* np = *list;
			while (np->next) {
				if (ndf->time <= np->next->time) {  // �������ʱֵ��С����ĳ����֮��
					ndf->next = np->next;
					np->next = ndf;
					return;
				}
				np = np->next;
			}
			// �������ʱֵ�������Ķ�Ҫ��
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
		np->func(np->arg);
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
