#pragma once

typedef struct _Note {
	long long time;
	struct _Note* next;
}Note;

Note* newNote(long long time) {
	Note* note = new Note;
	note->time = time;
	note->next = nullptr;
	return note;
}

// 其实相比于这个O(n^2)的工具函数，还不如在需要的地方手写一个利用链表尾的插入，反正文件里note都排好序了……
void addNote(Note** list, Note* note) {
	Note* np = *list;
	if (!np) {  // 表为空
		*list = note;
	}
	else {
		while (np->next) np = np->next;
		np->next = note;
	}
}
