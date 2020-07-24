#pragma once

#include<string.h>

#include "MU_declaration.h"

inline void musnake::initPath(char* path) {
	char* p = PATH;

	strcpy(PATH, path);
	while (*p)p++;
	while (*p != '\\' && *p != '/')p--;
	*(++p) = 0;
}

inline void musnake::catPath(char* dest, const char* relative) {
	strcpy(dest, PATH);
	strcat(dest, relative);
}
