#pragma once

#include<string.h>

namespace musnake {
	// ��������·��
	char PATH[256];

	// ��ʼ��·��
	void initPath(char* path);
	// �����·����ȡ����·��
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
