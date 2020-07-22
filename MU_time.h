#pragma once

#include"SDL.h"

namespace musnake {

	typedef struct _DelayFunc {
		void (*func)();
		unsigned long long time;
		struct _DelayFunc* next;
	}DelayFunc;

	/* ���������뵽ָ������ʱ�������� */
	void addDelayFunc(DelayFunc** list, void (*func)(), long long delay);
	/* ������������ʱ�����ʱ���� */
	void triggerDelayFunc(DelayFunc** list);
	/* ���Ŀ����ڵ���ʱ���� */
	void clearDelayFunc(DelayFunc** list);

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

inline void musnake::addDelayFunc(DelayFunc** list, void (*func)(), long long delay) {
	DelayFunc* ndf = new DelayFunc;
	ndf->func = func;
	ndf->time = delay + getTimeVal();

	if (!*list) {  // �����ʱ���ǿյ�
		*list = ndf;
		ndf->next = nullptr;
	}
	else {
		if (ndf->time < (*list)->time) {  // �������ʱֵ�������Ķ�ҪС
			ndf->next = *list;
			*list = ndf;
		}
		else {
			DelayFunc* np = *list;
			while (np->next) {
				if (ndf->time < (*list)->next->time) {  // �������ʱֵ��С����ĳ����֮��
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
