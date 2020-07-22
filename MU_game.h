#pragma once

#include "SDL.h"

#include "MU_grid.h"
#include "MU_snake.h"

namespace musnake{
	// ��Ϸ״̬ö��
	enum GameState {
		running,
		loading,
		paused,
		over
	};

	class Game;
}

class musnake::Game {
public:
	Game();
	~Game();

	int state;

	Grid* map[64][64];  // ��ʼ����64x64�ɡ���

private:
	SDL_Rect drawRect;  // ��ǰ��Ļ���������Ӧ��������ͼ�ľ���
};

musnake::Game::Game() {
	// ��˵����Ӧ���Ǵ������ļ������������ʼ���ظ�ģ��ֽ׶ξ���ɫ�����ɣ��Ͼ�Spriteͼ��û����
	state = loading;

	for (int i = 0;i < 64;i++) {
		for (int j = 0;j < 64;j++) {
			map[i][j] = new Grid;
			map[i][j]->setPosition(j * 40, i * 40, 40, 40);
		}
	}
}
