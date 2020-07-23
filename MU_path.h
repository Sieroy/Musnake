#pragma once

#include<string.h>

namespace musnake {
	// 程序所在路径
	char PATH[256];

	// 初始化路径
	void initPath(char* path);
	// 由相对路径获取绝对路径
	void catPath(char* dest, const char* relative);
}

inline void musnake::initPath(char* path) {
	char* p = PATH;

	strcpy(PATH, path);
	while (*p)p++;
	while (*p != '\\')p--;
	*(++p) = 0;
}

inline void musnake::catPath(char* dest, const char* relative) {
	strcpy(dest, PATH);
	strcat(dest, relative);
}
