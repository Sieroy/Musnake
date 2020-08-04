#define _CRT_SECURE_NO_WARNINGS

#include"SDL.h"
#include"SDL_main.h"
#include"SDL_ttf.h"
#include"SDL_image.h"
#include"SDL_mixer.h"

#include"MU_declaration.h"
#include"MU_flame.h"
#include"MU_path.h"
#include"MU_game.h"

using namespace musnake;


SDL_Window* window;

void load(SDL_Renderer* render);
void drawStart(SDL_Renderer* render);
void drawPanels(SDL_Renderer* render, LevelPanel** nowPanel, int* turning);

int main(int argc, char* argv[]) {
	SDL_Event evt;
	SDL_Color tmpColor = { 255, 255, 255, 255 };
	SDL_Surface* tmpSurf = nullptr;
	LevelPanel* nowLevel = nullptr;
	int panelTurning = 0;

	initPath(argv[0]);
	SDL_Init(SDL_INIT_EVERYTHING);
	IMG_Init(IMG_INIT_PNG);
	Mix_Init(MIX_INIT_MP3);
	TTF_Init();

	window = SDL_CreateWindow("Musnake", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 800, 600, SDL_WINDOW_RESIZABLE);
	render = SDL_CreateRenderer(window, -1, 0);
	Mix_OpenAudio(22050, MIX_DEFAULT_FORMAT, 2, 4096);
	musnakeState = MU_STATE_RUNNING;

	load(render);
	nowLevel = levels;

	tmpSurf = TTF_RenderText_Blended(titleMusnakeFont, "MUSNAKE", tmpColor);
	titleMusnakeFlame = new Flame(tmpSurf, NULL, -1);
	SDL_FreeSurface(tmpSurf);
	tmpSurf = TTF_RenderText_Blended(titleAuthorFont, "By Sieroy & StdCat", tmpColor);
	titleAuthorFlame = new Flame(tmpSurf, NULL, -1);
	SDL_FreeSurface(tmpSurf);

__start:
	Mix_FadeInMusic(titleBGM, -1, 1000);
	while (musnakeState) {
		updateTime();
		SDL_RenderClear(render);

		while (SDL_PollEvent(&evt)) {
			switch (evt.type) {
			case SDL_QUIT:
				musnakeState = false;
				break;
			case SDL_KEYDOWN:
				switch (evt.key.keysym.sym) {
				case SDLK_RETURN:
					Mix_HaltMusic();
					goto __menu;
					break;
				case SDLK_ESCAPE:
					musnakeState = MU_STATE_OVER;
					break;
				}
			}
		}
		drawStart(render);
		SDL_RenderPresent(render);
	}

__menu:
	Mix_PlayMusic(nowLevel->sample, -1);
	while (musnakeState) {
		updateTime();
		SDL_RenderClear(render);

		while (SDL_PollEvent(&evt)) {
			switch (evt.type) {
			case SDL_QUIT:
				musnakeState = false;
				break;
			case SDL_KEYDOWN:
				switch (evt.key.keysym.sym) {
				case SDLK_ESCAPE:
					if (!panelTurning) {
						Mix_FadeOutMusic(100);
						goto __start;
					}
					break;
				case SDLK_UP:
					if (!panelTurning) panelTurning = -200;
					break;
				case SDLK_DOWN:
					if (!panelTurning) panelTurning = 200;
					break;
				case SDLK_RETURN:
					if (!panelTurning) {
						musnakeState = MU_STATE_GAMING;
						while (musnakeState == MU_STATE_GAMING) {
							Mix_HaltMusic();
							thisGame = new Game();
							thisGame->setRenderer(render);
							thisGame->init(nowLevel);
							thisGame->run();
							delete thisGame;
						}
						Mix_PlayMusic(nowLevel->sample, -1);
					}
					break;
				}
			}
		}
		drawPanels(render, &nowLevel, &panelTurning);
		SDL_RenderPresent(render);
	}
	
	TTF_Quit();
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

	// 装载字体
	catPath(tmpPath, (char*)"font\\SHOWG.TTF");
	titleMusnakeFont = TTF_OpenFont(tmpPath, 100);  // 标题就得牌面！
	gameWinLengthnumFont =
	gameWinScorelabelFont = TTF_OpenFont(tmpPath, 50);
	gameScorenumFont =
	gameCombonumFont = TTF_OpenFont(tmpPath, 40);
	gameCombolabelFont = TTF_OpenFont(tmpPath, 20);
	gamePauseTitleFont =
	gameLoseTitleFont = TTF_OpenFont(tmpPath, 80);

	catPath(tmpPath, (char*)"font\\msyhbd.ttc");
	titleAuthorFont = TTF_OpenFont(tmpPath, 10);
	menuSongtimeFont = 
	gameScorelabelFont = 
	gamePauseSongnameFont = 
	gameLoseSongnameFont = TTF_OpenFont(tmpPath, 20);
	menuSongnameFont = 
	gameWinSongnameFont =
	gameWinScorelabelFont = 
	gameWinLengthlabelFont = 
	gamePauseNotChosenFont = TTF_OpenFont(tmpPath, 30);
	gamePauseChosenFont = TTF_OpenFont(tmpPath, 40);

	// 装载曲目信息
	catPath(tmpPath, (char*)"level\\list.mu");
	SDL_RWops* f = SDL_RWFromFile(tmpPath, "r");
	char c, * nsp = &c;
	char ss[48];
	LevelPanel* lp = nullptr;
	SDL_Color tfg = { 255, 255, 255, 255 };
	while (SDL_RWread(f, &c, 1, 1)) {
		switch (c) {  // 遇见换行，就要开新的曲表了
		case '\n':
			if (lp) {  // 并非第一行的情况
				*nsp = 0;
				if (*(--nsp) == '\r') *nsp = 0;  // 兼容Windows下的逗逼换行
				lp->timev = str2int(ss);
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
		case '`':  // 名字已经读完了
			*nsp = 0;
			// 装载这些曲目的试听段和封面绘图
			SDL_strlcpy(ss, "level\\", 48);
			SDL_strlcat(ss, lp->name, 48);
			SDL_strlcat(ss, "\\sample.mp3", 48);
			catPath(tmpPath, ss);
			lp->sample = Mix_LoadMUS(tmpPath);
			SDL_strlcpy(ss, "level\\", 48);
			SDL_strlcat(ss, lp->name, 48);
			SDL_strlcat(ss, "\\cover.png", 48);
			catPath(tmpPath, ss);
			tmpSurf = IMG_Load(tmpPath);
			lp->cover = new Flame(SDL_CreateTextureFromSurface(render, tmpSurf), -1);
			SDL_FreeSurface(tmpSurf);
			tmpSurf = TTF_RenderUTF8_Blended(menuSongnameFont, lp->name, tfg);
			lp->nameFlm = new Flame(tmpSurf, NULL, -1);
			SDL_FreeSurface(tmpSurf);
			nsp = lp->time;
			break;
		case '^':
			*nsp = 0;
			tmpSurf = TTF_RenderText_Blended(menuSongtimeFont, lp->time, tfg);
			lp->timeFlm = new Flame(tmpSurf, NULL, -1);
			SDL_FreeSurface(tmpSurf);
			nsp = ss;
			break;
		default:
			*nsp = c;
			nsp++;
			break;
		}
	}  // 最后闭合成环
	*nsp = 0;
	lp->timev = str2int(ss);
	lp->next = levels;
	levels->prev = lp;
	SDL_RWclose(f);

	// 装载标题界面BGM
	catPath(tmpPath, (char*)"sound\\bgm.mp3");
	titleBGM = Mix_LoadMUS(tmpPath);

	// 装载Note图
	catPath(tmpPath, (char*)"image\\notesign.png");
	picSurf = IMG_Load(tmpPath);
	tmpTex = SDL_CreateTextureFromSurface(render, picSurf);
	SDL_FreeSurface(picSurf);
	notesignFlame[0] = new Flame(tmpTex, -1);
	notesignFlame[0]->setNext(nullptr);

	catPath(tmpPath, (char*)"image\\notesign_fever.png");
	picSurf = IMG_Load(tmpPath);
	tmpTex = SDL_CreateTextureFromSurface(render, picSurf);
	SDL_FreeSurface(picSurf);
	notesignFlame[1] = new Flame(tmpTex, -1);
	notesignFlame[1]->setNext(nullptr);

	// 装载血条图
	catPath(tmpPath, (char*)"image\\hp.png");
	picSurf = IMG_Load(tmpPath);
	for (int i = 0;i < 6;i++) {
		SDL_Rect srect = { i * 20, 0, 20, 20 };

		hpFlame[i / 2][i & 1] = new Flame(picSurf, &srect, -1);
		hpFlame[i / 2][i & 1]->setNext(nullptr);
	}
	SDL_FreeSurface(picSurf);

	// 装载食物图
	catPath(tmpPath, (char*)"image\\food_0.png");
	picSurf = IMG_Load(tmpPath);
	foodFlame[0] = new Flame(picSurf, NULL, -1);
	SDL_FreeSurface(picSurf);
	foodFlame[0]->setNext(nullptr);

	// 开始装载字符图
	catPath(tmpPath, (char*)"image\\char.png");
	picSurf = IMG_Load(tmpPath);
	for (int i = 0;i < 6;i++) {  // 6行文字排列
		for (int j = 0;j < 16;j++) {  // 每行16个字符
			SDL_Rect srect = { j * 80, i * 160, 80, 160 };
			charFlame[16 * i + j] = new Flame(picSurf, &srect, -1);
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

			flames[j] = new Flame(picSurf, &srect, 10);
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

			flames[j] = new Flame(picSurf, &srect, 10);
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

		snakeFlame[j] = new Flame(picSurf, &srect, -1);
		snakeFlame[j]->setGroupId(j);
	}
	for (int j = MU_SNAKE_FLAME_BODY_UPDOWN;j <= MU_SNAKE_FLAME_BODY_UPLEFT;j++) {
		SDL_Rect srect = { (j - MU_SNAKE_FLAME_BODY_UPDOWN) * 20, 20, 20, 20 };

		snakeFlame[j] = new Flame(picSurf, &srect, -1);
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

void unload() {
	
}

void drawStart(SDL_Renderer* render) {  // 绘制游戏开始页面
	long long nt = (getTimeVal() / 500) & 1 ? 4 - (getTimeVal() % 1000 / 200) : getTimeVal() % 1000 / 100;
	titleMusnakeFlame->draw(render, 175, 100);
	drawText(render, (char*)"enter!", 330 - 3 * nt, 330 - nt, 20 + nt);
	titleAuthorFlame->draw(render, 5, 580);
}

void drawPanels(SDL_Renderer* render, LevelPanel** nowPanel, int* turning) {
	static int turningFlag = 0;
	LevelPanel* panel = *nowPanel;
	if (!*turning) {  // 仅绘制当前曲目
		SDL_Rect rect = {200, 200, 200, 200};
		panel->cover->draw(render, &rect);
		panel->nameFlm->draw(render, 430, 200);
		panel->timeFlm->draw(render, 430, 250);
		//drawText(render, panel->name, 430, 200, 20);
		//drawText(render, panel->time, 430, 250, 16);
	}
	else if (*turning > 0) {  // 向下滚动，面板要有向上移动的效果
		LevelPanel* lp = panel->next;
		SDL_Rect re1 = { 200, 200 + *turning * 3, 200, 200 }, re2 = { 200, -400 + *turning * 3, 200, 200 };
		if (turningFlag == 0) {  // 滚动后的首次调用
			turningFlag = 1;  // 1代表原先的音乐正处结束状态
			Mix_FadeOutMusic(190);
		}
		lp->cover->draw(render, &re1);
		lp->nameFlm->draw(render, 430, 200 + *turning * 3);
		lp->timeFlm->draw(render, 430, 250 + *turning * 3);
		//drawText(render, lp->name, 430, 200 + *turning * 3, 20);
		//drawText(render, lp->time, 430, 250 + *turning * 3, 16);
		panel->cover->draw(render, &re2);
		panel->nameFlm->draw(render, 430, -400 + *turning * 3);
		panel->timeFlm->draw(render, 430, -350 + *turning * 3);
		//drawText(render, panel->name, 430, -400 + *turning * 3, 20);
		//drawText(render, panel->time, 430, -350 + *turning * 3, 16);
		*turning -= getTimeDelta();
		if (*turning <= 0) {
			SDL_RenderClear(render);
			re1.y = 200;
			lp->cover->draw(render, &re1);
			lp->nameFlm->draw(render, 430, 200);
			lp->timeFlm->draw(render, 430, 250);
			//drawText(render, lp->name, 430, 200, 20);
			//drawText(render, lp->time, 430, 250, 16);
			SDL_RenderPresent(render);

			*turning = 0;
			turningFlag = 0;
			Mix_PlayMusic(lp->sample, -1);
			*nowPanel = lp;
		}
	}
	else if (*turning < 0) {  // 向上滚动，面板要有向下移动的效果
		LevelPanel* lp = panel->prev;
		SDL_Rect re1 = { 200, 200 + *turning * 3, 200, 200 }, re2 = { 200, 800 + *turning * 3, 200, 200 };
		if (turningFlag == 0) {  // 滚动后的首次调用
			turningFlag = 1;  // 1代表原先的音乐正处结束状态
			Mix_FadeOutMusic(190);
		}
		lp->cover->draw(render, &re1);
		lp->nameFlm->draw(render, 430, 200 + *turning * 3);
		lp->timeFlm->draw(render, 430, 250 + *turning * 3);
		//drawText(render, lp->name, 430, 200 + *turning * 3, 20);
		//drawText(render, lp->time, 430, 250 + *turning * 3, 16);
		panel->cover->draw(render, &re2);
		panel->nameFlm->draw(render, 430, 800 + *turning * 3);
		panel->timeFlm->draw(render, 430, 850 + *turning * 3);
		//drawText(render, panel->name, 430, 800 + *turning * 3, 20);
		//drawText(render, panel->time, 430, 850 + *turning * 3, 16);
		*turning += getTimeDelta();
		if (*turning >= 0) {
			SDL_RenderClear(render);
			re1.y = 200;
			lp->cover->draw(render, &re1);
			lp->nameFlm->draw(render, 430, 200);
			lp->timeFlm->draw(render, 430, 250);
			//drawText(render, lp->name, 430, 200, 20);
			//drawText(render, lp->time, 430, 250, 16);
			SDL_RenderPresent(render);

			*turning = 0;
			turningFlag = 0;
			Mix_PlayMusic(lp->sample, -1);
			*nowPanel = lp;
		}
	}
}
