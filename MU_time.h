#pragma once

#include<SDL.h>

#include"MU_declaration.h"

static unsigned long long timeVal = 0;
static unsigned long timeDelta = 0;

inline unsigned long long musnake::updateTime() {
	static unsigned flameCount = 0;
	static unsigned timeCount = 0;
	unsigned long long tv = SDL_GetTicks();
	timeDelta = (unsigned long)(tv - timeVal);
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

inline unsigned long musnake::getTimeDelta() {
	return timeDelta;
}

inline void musnake::addDelayFunc(DelayFunc** list, void (*func)(unsigned long), unsigned long arg, long long delay) {
	DelayFunc* ndf = new DelayFunc;
	ndf->func = func;
	ndf->arg = arg;
	ndf->time = delay + getTimeVal();

	if (!*list) {
		*list = ndf;
		ndf->next = nullptr;
	}
	else {
		if (ndf->time <= (*list)->time) {
			ndf->next = *list;
			*list = ndf;
		}
		else {
			DelayFunc* np = *list;
			while (np->next) {
				if (ndf->time <= np->next->time) {
					ndf->next = np->next;
					np->next = ndf;
					return;
				}
				np = np->next;
			}
			np->next = ndf;
			ndf->next = nullptr;
		}
	}
}

inline int musnake::removeDelayFuncByFuncArg(DelayFunc** list, void(*func)(unsigned long), unsigned long arg) {
	DelayFunc* dp = *list;

	if (!dp) return 1; 
	if (dp->func == func && dp->arg == arg) { 
		*list = dp->next;
		delete dp;
		return 0;
	}
	else {
		while (DelayFunc* tp = dp->next) {
			if (tp->func == func && tp->arg == arg) {
				dp->next = dp->next->next;
				delete tp;
				return 0;
			}
			dp = dp->next;
		}
		return 1;
	}
}

inline void musnake::triggerDelayFunc(DelayFunc** list) {
	DelayFunc* np;
	
	while (np = *list) {
		if (np->time > (long long)getTimeVal()) break;
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
