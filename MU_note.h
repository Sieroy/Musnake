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

// ��ʵ��������O(n^2)�Ĺ��ߺ���������������Ҫ�ĵط���дһ����������β�Ĳ��룬�����ļ���note���ź����ˡ���
void addNote(Note** list, Note* note) {
	Note* np = *list;
	if (!np) {  // ��Ϊ��
		*list = note;
	}
	else {
		while (np->next) np = np->next;
		np->next = note;
	}
}
