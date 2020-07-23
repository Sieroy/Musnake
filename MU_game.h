#pragma once

#include "SDL.h"
#include "SDL_image.h"

#include "MU_path.h"
#include "MU_grid.h"
#include "MU_snake.h"
#include "MU_flame.h"

namespace musnake{
	// ��Ϸ״̬ö��
	enum GameState {
		MU_GAME_STATE_RUNNING,
		MU_GAME_STATE_LOADING,
		MU_GAME_STATE_PAUSED,
		MU_GAME_STATE_OVER
	};

	// ����֡�����Ƿ�
	enum SnakeFlameIndex {
		MU_SNAKE_FLAME_HEAD_UP,
		MU_SNAKE_FLAME_HEAD_RIGHT,
		MU_SNAKE_FLAME_HEAD_DOWN,
		MU_SNAKE_FLAME_HEAD_LEFT,

		MU_SNAKE_FLAME_TAIL_UP,
		MU_SNAKE_FLAME_TAIL_RIGHT,
		MU_SNAKE_FLAME_TAIL_DOWN,
		MU_SNAKE_FLAME_TAIL_LEFT,

		MU_SNAKE_FLAME_BODY_UPDOWN,
		MU_SNAKE_FLAME_BODY_RIGHTLEFT,
		MU_SNAKE_FLAME_BODY_UPRIGHT,
		MU_SNAKE_FLAME_BODY_RIGHTDOWN,
		MU_SNAKE_FLAME_BODY_DOWNLEFT,
		MU_SNAKE_FLAME_BODY_UPLEFT,

		MU_SNAKE_FLAME_HEAD_0toUP,
		MU_SNAKE_FLAME_HEAD_UPtoRIGHT,
		MU_SNAKE_FLAME_HEAD_UPtoDOWN,
		MU_SNAKE_FLAME_HEAD_UPtoLEFT,
		MU_SNAKE_FLAME_HEAD_0toRIGHT,
		MU_SNAKE_FLAME_HEAD_RIGHTtoUP,
		MU_SNAKE_FLAME_HEAD_RIGHTtoDOWN,
		MU_SNAKE_FLAME_HEAD_RIGHTtoLEFT,
		MU_SNAKE_FLAME_HEAD_0toDOWN,
		MU_SNAKE_FLAME_HEAD_DOWNtoUP,
		MU_SNAKE_FLAME_HEAD_DOWNtoRIGHT,
		MU_SNAKE_FLAME_HEAD_DOWNtoLEFT,
		MU_SNAKE_FLAME_HEAD_0toLEFT,
		MU_SNAKE_FLAME_HEAD_LEFTtoUP,
		MU_SNAKE_FLAME_HEAD_LEFTtoRIGHT,
		MU_SNAKE_FLAME_HEAD_LEFTtoLEFT,

		MU_SNAKE_FLAME_TAIL_UPshake,
		MU_SNAKE_FLAME_TAIL_UPto0,
		MU_SNAKE_FLAME_TAIL_RIGHTtoUP,
		MU_SNAKE_FLAME_TAIL_DOWNtoUP,
		MU_SNAKE_FLAME_TAIL_LEFTtoUP,
		MU_SNAKE_FLAME_TAIL_RIGHTshake,
		MU_SNAKE_FLAME_TAIL_RIGHTto0,
		MU_SNAKE_FLAME_TAIL_UPtoRIGHT,
		MU_SNAKE_FLAME_TAIL_DOWNtoRIGHT,
		MU_SNAKE_FLAME_TAIL_LEFTtoRIGHT,
		MU_SNAKE_FLAME_TAIL_DOWNshake,
		MU_SNAKE_FLAME_TAIL_DOWNto0,
		MU_SNAKE_FLAME_TAIL_UPtoDOWN,
		MU_SNAKE_FLAME_TAIL_RIGHTtoDOWN,
		MU_SNAKE_FLAME_TAIL_LEFTtoDOWN,
		MU_SNAKE_FLAME_TAIL_LEFTshake,
		MU_SNAKE_FLAME_TAIL_LEFTto0,
		MU_SNAKE_FLAME_TAIL_UPtoLEFT,
		MU_SNAKE_FLAME_TAIL_RIGHTtoLEFT,
		MU_SNAKE_FLAME_TAIL_DOWNtoLEFT,
	};

	class Game;

	Grid* gameMap[64][64] = { nullptr };  // ������Ϸ�õ��ĵظ��ǣ���һάΪX���꣬�ڶ�άΪY����
	Flame* snakeFlame[50] = { nullptr };  // ������Ҫ�õ���֡��
}

class musnake::Game {
public:
	Game();
	~Game();

	int state;
	SDL_Renderer* gameRender;

	void setRenderer(SDL_Renderer* render);

private:
	SDL_Rect drawRect;  // ��ǰ��Ļ���������Ӧ��������ͼ�ľ���
};

musnake::Game::Game() {
	// ��˵����Ӧ���Ǵ������ļ������������ʼ���ظ�ģ��ֽ׶ξ���д����
	SDL_Surface* picSurf, tmpSurf;
	SDL_Texture* tmpTex;

	state = MU_GAME_STATE_LOADING;

	// ���ﶨλͼƬ�ļ���λ��
	char snakeBodyPicturePath[256], snakeHeadPicturePath[256], snakeTailPicturePath[256];
	catPath(snakeBodyPicturePath, "image\\snake_0_body.png");
	catPath(snakeHeadPicturePath, "image\\snake_0_head.png");
	catPath(snakeTailPicturePath, "image\\snake_0_tail.png");

	// ��ʼװ����ͷͼ��������ͷ��Ӧ��ö�ٴ�14�ţ�MU_SNAKE_FLAME_HEAD_0toUP����ʼ��29�Ž���
	picSurf = IMG_Load(snakeHeadPicturePath);
	for (int i = 0;i < 16;i++) {  // 16����ͷ�˶����
		Flame* flames[8];
		for (int j = 0;j < 8;j++) {  // ÿ���˶������8֡
			SDL_Rect srect = { j * 20, i * 20, 20, 20 };
			SDL_BlitSurface(picSurf, &srect, &tmpSurf, NULL);
			tmpTex = SDL_CreateTextureFromSurface(gameRender, &tmpSurf);
			flames[j] = new Flame(tmpTex, 25);
			flames[j]->setGroupId(MU_SNAKE_FLAME_HEAD_0toUP + i);
			if (j)flames[j - 1]->setNext(flames[j]);
		}
		snakeFlame[MU_SNAKE_FLAME_HEAD_0toUP + i] = flames[0];
	}
	SDL_FreeSurface(picSurf);

	// ��ʼװ����βͼ��������β��Ӧ��ö�ٴ�30�ţ�MU_SNAKE_FLAME_TAIL_UPshake����ʼ��49�Ž���
	picSurf = IMG_Load(snakeTailPicturePath);
	for (int i = 0;i < 20;i++) {  // 20��������β�����
		Flame* flames[8];
		for (int j = 0;j < 8;j++) {
			SDL_Rect srect = { j * 20, i * 20, 20, 20 };
			SDL_BlitSurface(picSurf, &srect, &tmpSurf, NULL);
			tmpTex = SDL_CreateTextureFromSurface(gameRender, &tmpSurf);
			flames[j] = new Flame(tmpTex, 25);
			flames[j]->setGroupId(MU_SNAKE_FLAME_TAIL_UPshake + i);
			if (j)flames[j - 1]->setNext(flames[j]);
		}
		snakeFlame[MU_SNAKE_FLAME_TAIL_UPshake + i] = flames[0];
	}
	SDL_FreeSurface(picSurf);

	// ��ʼװ��һ������ͼ��һ�������Ӧ��ö�ٴ�0�ţ�MU_SNAKE_FLAME_HEAD_UP����ʼ��13�Ž���
	picSurf = IMG_Load(snakeBodyPicturePath);
	for (int j = MU_SNAKE_FLAME_HEAD_UP;j <= MU_SNAKE_FLAME_TAIL_LEFT;j++) {
		SDL_Rect srect = { j * 20, 0, 20, 20 };
		SDL_BlitSurface(picSurf, &srect, &tmpSurf, NULL);
		tmpTex = SDL_CreateTextureFromSurface(gameRender, &tmpSurf);
		snakeFlame[j] = new Flame(tmpTex, -1);
		snakeFlame[j]->setGroupId(j);
	}
	for (int j = MU_SNAKE_FLAME_BODY_UPDOWN;j <= MU_SNAKE_FLAME_BODY_UPLEFT;j++) {
		SDL_Rect srect = { (j - MU_SNAKE_FLAME_BODY_UPDOWN) * 20, 20, 20, 20 };
		SDL_BlitSurface(picSurf, &srect, &tmpSurf, NULL);
		tmpTex = SDL_CreateTextureFromSurface(gameRender, &tmpSurf);
		snakeFlame[j] = new Flame(tmpTex, -1);
		snakeFlame[j]->setGroupId(j);
	}
	SDL_FreeSurface(picSurf);

	/* ����Spritesͼ��װ�أ����ȿ϶������Ż������ġ�����ͷ��β����֡��ĩ֡����һ���;�̬��ͷ��βͼ�ظ���
	 * ������Щ��ͬ��ͼ��û��Ҫ�ٴ�װ���ˣ���������Ÿ���һ�¾ͺ��ˣ������������Ż������ڴ����⡣
	 * ���⣬Ŀǰ��ֻ������һ��Ƥ����������򵥵�����Ƥ����֮���ٻ����Ƥ���Ļ����Ϳ��ܳ���ԭ��Ӧ����̬��ͼ��ö�̬֮������
	 * �������ã��Ͼ����ڵ�֡��ṹ��Ӧ���������������̬ͼ��Sprites���̲�һ��ô�졭��
	 * �������ڿ���Ҫ��Ҫ���������JSON֧�֣�������̬�������ˣ��������������Կ�����������֮��ġ�
	 * �ȿ����Űɣ�����˳��+ʱ���ԣ�Ļ�����Ҳ���ǲ����ԡ�
	 */

	for (int i = 0;i < 64;i++) {
		for (int j = 0;j < 64;j++) {
			Grid* map = gameMap[i][j] = new Grid;
			map->setPosition(i * 40, j * 40, 40, 40);
			map->objType = MU_GRID_OBJECT_TYPE_EMPTY;
		}
	}
}

inline void musnake::Game::setRenderer(SDL_Renderer* render) {
	gameRender = render;
}
