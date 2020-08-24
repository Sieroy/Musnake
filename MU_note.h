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

void clearNotes(Note** list) {
	Note* np;
	while (np = *list) {
		*list = np->next;
		delete np;
	}
}
