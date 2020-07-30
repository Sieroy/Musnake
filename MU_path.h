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
}

inline void int2str(char* dest, int num) {
	sprintf(dest, "%d", num);
}

inline int str2int(char* src) {
	int i;
	sscanf(src, "%d", &i);
	return i;
}
