#pragma once

#include<string.h>
#include<stdio.h>

#include "MU_declaration.h"

inline void musnake::initPath(char* path) {
	char* p = PATH;

	strcpy(PATH, path);
	while (*p)p++;
	while (*p != '\\' && *p != '/')p--;
	*(++p) = 0;
}

inline void musnake::catPath(char* dest, char* relative) {
	strcpy(dest, PATH);
	strcat(dest, relative);
	char* cp = dest;
	while (*cp) {
#if defined(_WIN32) || defined(_WIN64)
		if (*cp == '/') *cp = '\\';
#elif defined(_linux) || defined(_linux_) || defined(_unix) || defined(_unix_)
		if (*cp == '\\') *cp = '/';
#endif
		cp++;
	}
}

inline void int2str(char* dest, int num) {
	sprintf(dest, "%d", num);
}

inline int str2int(char* src) {
	int i;
	sscanf(src, "%d", &i);
	return i;
}
