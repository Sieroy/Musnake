#define _CRT_SECURE_NO_WARNINGS

#include"SDL.h"
#include"SDL_main.h"
#include"SDL_image.h"
#include"SDL_mixer.h"

#include"MU_declaration.h"
#include"MU_path.h"
#include"MU_game.h"

using namespace musnake;


SDL_Window* window;
SDL_Renderer* render;

void load(SDL_Renderer* render);

int main(int argc, char* argv[]) {
	SDL_Event evt;

	initPath(argv[0]);
	SDL_Init(SDL_INIT_EVERYTHING);
	IMG_Init(IMG_INIT_PNG);
	Mix_Init(MIX_INIT_MP3);

	window = SDL_CreateWindow("Musnake", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 800, 600, SDL_WINDOW_RESIZABLE);
	render = SDL_CreateRenderer(window, -1, 0);
	SDL_GetDisplayUsableBounds(0, &drawableScreenArea);
	Mix_OpenAudio(22050, MIX_DEFAULT_FORMAT, 2, 4096);
	musnakeState = MU_STATE_RUNNING;
	
	while (musnakeState) {
		while (SDL_PollEvent(&evt)) {
			switch (evt.type) {
			case SDL_QUIT:
				musnakeState = false;
				break;
			}
		}

	}
	
	thisGame = new Game();
	thisGame->setRenderer(render);
	thisGame->init((char*)"Jinshekuangwu");
	thisGame->run();

	IMG_Quit();
	Mix_Quit();
	SDL_DestroyRenderer(render);
	SDL_DestroyWindow(window);
	SDL_Quit();

	return 0;
}

void load(SDL_Renderer* render) {
	// 按说这里应该是从配置文件里读数据来初始化地格的，现阶段就先写死吧
	SDL_Surface* picSurf, * tmpSurf;
	SDL_Texture* tmpTex;

	// 这里定位文件的位置
	char tmpPath[256];

	// 装载曲目信息
	catPath(tmpPath, (char*)"level\\list.mu");
	SDL_RWops* f = SDL_RWFromFile(tmpPath, "r");
	char c, * nsp;
	LevelPanel* lp = nullptr;
	while (SDL_RWread(f, &c, 1, 1)) {
		switch (c) {  // 遇见换行，就要开新的曲表了
		case '\n':
			if (lp) {  // 并非第一行的情况
				*nsp = 0;
				lp->next = new LevelPanel;
				lp->next->prev = lp;
				lp = lp->next;
				nsp = lp->name;
			}
			else {  // 第一行的情况
				levels = lp = new LevelPanel;
				nsp = lp->name;
			}
			break;
		case '`':
			*nsp = 0;
			nsp = lp->time;
			break;
		default:
			*nsp = c;
			nsp++;
			break;
		}
	}
	lp->next = levels;
	levels->prev = lp;


	// 装载Note图
	catPath(tmpPath, (char*)"image\\notesign.png");
	picSurf = IMG_Load(tmpPath);
	tmpTex = SDL_CreateTextureFromSurface(render, picSurf);
	SDL_FreeSurface(picSurf);
	notesignFlame[0] = new Flame(tmpTex, -1);
	notesignFlame[0]->setNext(nullptr);

	// 装载血条图
	catPath(tmpPath, (char*)"image\\hp.png");
	picSurf = IMG_Load(tmpPath);
	for (int i = 0;i < 6;i++) {
		SDL_Rect srect = { i * 20, 0, 20, 20 };

#if SDL_BYTEORDER == SDL_BIG_ENDIAN
		tmpSurf = SDL_CreateRGBSurface(0, 20, 20, 32, 0xff000000, 0x00ff0000, 0x0000ff00, 0x000000ff);
#else
		tmpSurf = SDL_CreateRGBSurface(0, 20, 20, 32, 0x000000ff, 0x0000ff00, 0x00ff0000, 0xff000000);
#endif
		SDL_BlitSurface(picSurf, &srect, tmpSurf, NULL);
		tmpTex = SDL_CreateTextureFromSurface(render, tmpSurf);
		SDL_FreeSurface(tmpSurf);
		hpFlame[i / 2][i & 1] = new Flame(tmpTex, -1);
		hpFlame[i / 2][i & 1]->setNext(nullptr);
	}

	// 装载食物图
	catPath(tmpPath, (char*)"image\\food_0.png");
	picSurf = IMG_Load(tmpPath);
	tmpTex = SDL_CreateTextureFromSurface(render, picSurf);
	SDL_FreeSurface(picSurf);
	foodFlame[0] = new Flame(tmpTex, -1);
	foodFlame[0]->setNext(nullptr);

	// 开始装载字符图
	catPath(tmpPath, (char*)"image\\char.png");
	picSurf = IMG_Load(tmpPath);
	for (int i = 0;i < 6;i++) {  // 6行文字排列
		for (int j = 0;j < 16;j++) {  // 每行16个字符
			SDL_Rect srect = { j * 80, i * 160, 80, 160 };

#if SDL_BYTEORDER == SDL_BIG_ENDIAN
			tmpSurf = SDL_CreateRGBSurface(0, 80, 160, 32, 0xff000000, 0x00ff0000, 0x0000ff00, 0x000000ff);
#else
			tmpSurf = SDL_CreateRGBSurface(0, 80, 160, 32, 0x000000ff, 0x0000ff00, 0x00ff0000, 0xff000000);
#endif

			SDL_BlitSurface(picSurf, &srect, tmpSurf, NULL);
			tmpTex = SDL_CreateTextureFromSurface(render, tmpSurf);
			SDL_FreeSurface(tmpSurf);
			charFlame[16 * i + j] = new Flame(tmpTex, -1);
		}
	}
	SDL_FreeSurface(picSurf);

	// 开始装载蛇头图，有向蛇头对应的枚举从14号（MU_SNAKE_FLAME_HEAD_0toUP）开始，29号结束
	catPath(tmpPath, (char*)"image\\snake_0_head.png");
	picSurf = IMG_Load(tmpPath);
	for (int i = 0;i < 16;i++) {  // 16种蛇头运动情况
		Flame* flames[8];
		for (int j = 0;j < 8;j++) {  // 每种运动情况有8帧
			SDL_Rect srect = { j * 20, i * 20, 20, 20 };

#if SDL_BYTEORDER == SDL_BIG_ENDIAN
			tmpSurf = SDL_CreateRGBSurface(0, 20, 20, 32, 0xff000000, 0x00ff0000, 0x0000ff00, 0x000000ff);
#else
			tmpSurf = SDL_CreateRGBSurface(0, 20, 20, 32, 0x000000ff, 0x0000ff00, 0x00ff0000, 0xff000000);
#endif

			SDL_BlitSurface(picSurf, &srect, tmpSurf, NULL);
			tmpTex = SDL_CreateTextureFromSurface(render, tmpSurf);
			SDL_FreeSurface(tmpSurf);
			flames[j] = new Flame(tmpTex, 10);
			flames[j]->setGroupId(MU_SNAKE_FLAME_HEAD_0toUP + i);
			if (j)flames[j - 1]->setNext(flames[j]);
		}
		snakeFlame[MU_SNAKE_FLAME_HEAD_0toUP + i] = flames[0];
	}
	SDL_FreeSurface(picSurf);

	// 开始装载蛇尾图，有向蛇尾对应的枚举从30号（MU_SNAKE_FLAME_TAIL_UPshake）开始，49号结束
	catPath(tmpPath, (char*)"image\\snake_0_tail.png");
	picSurf = IMG_Load(tmpPath);
	for (int i = 0;i < 20;i++) {  // 20种有向蛇尾的情况
		Flame* flames[8];
		for (int j = 0;j < 8;j++) {
			SDL_Rect srect = { j * 20, i * 20, 20, 20 };

#if SDL_BYTEORDER == SDL_BIG_ENDIAN
			tmpSurf = SDL_CreateRGBSurface(0, 20, 20, 32, 0xff000000, 0x00ff0000, 0x0000ff00, 0x000000ff);
#else
			tmpSurf = SDL_CreateRGBSurface(0, 20, 20, 32, 0x000000ff, 0x0000ff00, 0x00ff0000, 0xff000000);
#endif

			SDL_BlitSurface(picSurf, &srect, tmpSurf, NULL);
			tmpTex = SDL_CreateTextureFromSurface(render, tmpSurf);
			SDL_FreeSurface(tmpSurf);
			flames[j] = new Flame(tmpTex, 10);
			flames[j]->setGroupId(MU_SNAKE_FLAME_TAIL_UPshake + i);
			if (j)flames[j - 1]->setNext(flames[j]);
		}
		snakeFlame[MU_SNAKE_FLAME_TAIL_UPshake + i] = flames[0];
	}
	SDL_FreeSurface(picSurf);

	// 开始装载一般蛇体图，一般蛇体对应的枚举从0号（MU_SNAKE_FLAME_HEAD_UP）开始，13号结束
	catPath(tmpPath, (char*)"image\\snake_0_body.png");
	picSurf = IMG_Load(tmpPath);
	for (int j = MU_SNAKE_FLAME_HEAD_UP;j <= MU_SNAKE_FLAME_TAIL_LEFT;j++) {
		SDL_Rect srect = { j * 20, 0, 20, 20 };

#if SDL_BYTEORDER == SDL_BIG_ENDIAN
		tmpSurf = SDL_CreateRGBSurface(0, 20, 20, 32, 0xff000000, 0x00ff0000, 0x0000ff00, 0x000000ff);
#else
		tmpSurf = SDL_CreateRGBSurface(0, 20, 20, 32, 0x000000ff, 0x0000ff00, 0x00ff0000, 0xff000000);
#endif

		SDL_BlitSurface(picSurf, &srect, tmpSurf, NULL);
		tmpTex = SDL_CreateTextureFromSurface(render, tmpSurf);
		SDL_FreeSurface(tmpSurf);
		snakeFlame[j] = new Flame(tmpTex, -1);
		snakeFlame[j]->setGroupId(j);
	}
	for (int j = MU_SNAKE_FLAME_BODY_UPDOWN;j <= MU_SNAKE_FLAME_BODY_UPLEFT;j++) {
		SDL_Rect srect = { (j - MU_SNAKE_FLAME_BODY_UPDOWN) * 20, 20, 20, 20 };

#if SDL_BYTEORDER == SDL_BIG_ENDIAN
		tmpSurf = SDL_CreateRGBSurface(0, 20, 20, 32, 0xff000000, 0x00ff0000, 0x0000ff00, 0x000000ff);
#else
		tmpSurf = SDL_CreateRGBSurface(0, 20, 20, 32, 0x000000ff, 0x0000ff00, 0x00ff0000, 0xff000000);
#endif

		SDL_BlitSurface(picSurf, &srect, tmpSurf, NULL);
		tmpTex = SDL_CreateTextureFromSurface(render, tmpSurf);
		SDL_FreeSurface(tmpSurf);
		snakeFlame[j] = new Flame(tmpTex, -1);
		snakeFlame[j]->setGroupId(j);
	}

	// 然后对不同的帧组进行关联，艹了
	snakeFlame[MU_SNAKE_FLAME_HEAD_0toUP]->setNext(snakeFlame[MU_SNAKE_FLAME_HEAD_UP]);
	snakeFlame[MU_SNAKE_FLAME_HEAD_0toRIGHT]->setNext(snakeFlame[MU_SNAKE_FLAME_HEAD_RIGHT]);
	snakeFlame[MU_SNAKE_FLAME_HEAD_0toDOWN]->setNext(snakeFlame[MU_SNAKE_FLAME_HEAD_DOWN]);
	snakeFlame[MU_SNAKE_FLAME_HEAD_0toLEFT]->setNext(snakeFlame[MU_SNAKE_FLAME_HEAD_LEFT]);
	snakeFlame[MU_SNAKE_FLAME_HEAD_UPtoRIGHT]->setNext(snakeFlame[MU_SNAKE_FLAME_BODY_UPRIGHT]);
	snakeFlame[MU_SNAKE_FLAME_HEAD_UPtoDOWN]->setNext(snakeFlame[MU_SNAKE_FLAME_BODY_UPDOWN]);
	snakeFlame[MU_SNAKE_FLAME_HEAD_UPtoLEFT]->setNext(snakeFlame[MU_SNAKE_FLAME_BODY_UPLEFT]);
	snakeFlame[MU_SNAKE_FLAME_HEAD_RIGHTtoUP]->setNext(snakeFlame[MU_SNAKE_FLAME_BODY_UPRIGHT]);
	snakeFlame[MU_SNAKE_FLAME_HEAD_RIGHTtoDOWN]->setNext(snakeFlame[MU_SNAKE_FLAME_BODY_RIGHTDOWN]);
	snakeFlame[MU_SNAKE_FLAME_HEAD_RIGHTtoLEFT]->setNext(snakeFlame[MU_SNAKE_FLAME_BODY_RIGHTLEFT]);
	snakeFlame[MU_SNAKE_FLAME_HEAD_DOWNtoUP]->setNext(snakeFlame[MU_SNAKE_FLAME_BODY_UPDOWN]);
	snakeFlame[MU_SNAKE_FLAME_HEAD_DOWNtoRIGHT]->setNext(snakeFlame[MU_SNAKE_FLAME_BODY_RIGHTDOWN]);
	snakeFlame[MU_SNAKE_FLAME_HEAD_DOWNtoLEFT]->setNext(snakeFlame[MU_SNAKE_FLAME_BODY_DOWNLEFT]);
	snakeFlame[MU_SNAKE_FLAME_HEAD_LEFTtoUP]->setNext(snakeFlame[MU_SNAKE_FLAME_BODY_UPLEFT]);
	snakeFlame[MU_SNAKE_FLAME_HEAD_LEFTtoRIGHT]->setNext(snakeFlame[MU_SNAKE_FLAME_BODY_RIGHTLEFT]);
	snakeFlame[MU_SNAKE_FLAME_HEAD_LEFTtoDOWN]->setNext(snakeFlame[MU_SNAKE_FLAME_BODY_DOWNLEFT]);

	snakeFlame[MU_SNAKE_FLAME_TAIL_UPshake]->setNext(snakeFlame[MU_SNAKE_FLAME_TAIL_UP]);
	snakeFlame[MU_SNAKE_FLAME_TAIL_RIGHTtoUP]->setNext(snakeFlame[MU_SNAKE_FLAME_TAIL_UP]);
	snakeFlame[MU_SNAKE_FLAME_TAIL_DOWNtoUP]->setNext(snakeFlame[MU_SNAKE_FLAME_TAIL_UP]);
	snakeFlame[MU_SNAKE_FLAME_TAIL_LEFTtoUP]->setNext(snakeFlame[MU_SNAKE_FLAME_TAIL_UP]);
	snakeFlame[MU_SNAKE_FLAME_TAIL_RIGHTshake]->setNext(snakeFlame[MU_SNAKE_FLAME_TAIL_RIGHT]);
	snakeFlame[MU_SNAKE_FLAME_TAIL_UPtoRIGHT]->setNext(snakeFlame[MU_SNAKE_FLAME_TAIL_RIGHT]);
	snakeFlame[MU_SNAKE_FLAME_TAIL_DOWNtoRIGHT]->setNext(snakeFlame[MU_SNAKE_FLAME_TAIL_RIGHT]);
	snakeFlame[MU_SNAKE_FLAME_TAIL_LEFTtoRIGHT]->setNext(snakeFlame[MU_SNAKE_FLAME_TAIL_RIGHT]);
	snakeFlame[MU_SNAKE_FLAME_TAIL_DOWNshake]->setNext(snakeFlame[MU_SNAKE_FLAME_TAIL_DOWN]);
	snakeFlame[MU_SNAKE_FLAME_TAIL_UPtoDOWN]->setNext(snakeFlame[MU_SNAKE_FLAME_TAIL_DOWN]);
	snakeFlame[MU_SNAKE_FLAME_TAIL_RIGHTtoDOWN]->setNext(snakeFlame[MU_SNAKE_FLAME_TAIL_DOWN]);
	snakeFlame[MU_SNAKE_FLAME_TAIL_LEFTtoDOWN]->setNext(snakeFlame[MU_SNAKE_FLAME_TAIL_DOWN]);
	snakeFlame[MU_SNAKE_FLAME_TAIL_LEFTshake]->setNext(snakeFlame[MU_SNAKE_FLAME_TAIL_LEFT]);
	snakeFlame[MU_SNAKE_FLAME_TAIL_UPtoLEFT]->setNext(snakeFlame[MU_SNAKE_FLAME_TAIL_LEFT]);
	snakeFlame[MU_SNAKE_FLAME_TAIL_RIGHTtoLEFT]->setNext(snakeFlame[MU_SNAKE_FLAME_TAIL_LEFT]);
	snakeFlame[MU_SNAKE_FLAME_TAIL_DOWNtoLEFT]->setNext(snakeFlame[MU_SNAKE_FLAME_TAIL_LEFT]);
}

void drawStart(SDL_Renderer* render) {  // 绘制游戏开始页面

}