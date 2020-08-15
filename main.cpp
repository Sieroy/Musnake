#define _CRT_SECURE_NO_WARNINGS
#define SDL_MAIN_HANDLED

#include"SDL.h"
#include"SDL_main.h"
#include"SDL_ttf.h"
#include"SDL_image.h"
#include"SDL_mixer.h"

#include"MU_declaration.h"
#include"MU_flame.h"
#include"MU_file.h"
#include"MU_path.h"
#include"MU_game.h"

using namespace musnake;


SDL_Window* window;

void load(SDL_Renderer* render);
void unload();
void drawStart(SDL_Renderer* render);
void drawPanels(SDL_Renderer* render, Level** nowPanel, LevelClass** nowClass, int* turningLevel, int* turningClass);

int main(int argc, char* argv[]) {
	SDL_Event evt;
	SDL_Surface* tmpSurf = nullptr;
	Level* nowLevel = nullptr;
	LevelClass* nowClass = nullptr;
	int panelTurning = 0;
	int classTurning = 0;

	initPath(argv[0]);
	SDL_Init(SDL_INIT_EVERYTHING);
	IMG_Init(IMG_INIT_PNG);
	Mix_Init(MIX_INIT_MP3);
	TTF_Init();

	window = SDL_CreateWindow("Musnake", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 800, 600, SDL_WINDOW_SHOWN);
	render = SDL_CreateRenderer(window, -1, 0);
	Mix_OpenAudio(22050, MIX_DEFAULT_FORMAT, 2, 4096);
	musnakeState = MU_STATE_RUNNING;

	load(render);
	nowLevel = levelClasses->levels;
	nowClass = levelClasses;
	SDL_SetRenderDrawBlendMode(render, SDL_BLENDMODE_BLEND);
	SDL_SetRenderDrawColor(render, 0, 0, 0, 150);

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
				case SDLK_RETURN2:
					Mix_HaltMusic();
					goto __menu;
					break;
				case SDLK_ESCAPE:
					musnakeState = MU_STATE_OVER;
					break;
				}
			case SDL_MOUSEBUTTONDOWN:
				if( evt.button.button == SDL_BUTTON_LEFT){
					SDL_Point point{evt.button.x, evt.button.y};
					SDL_Rect ReturnButton{ 300,440,150,60 };
					if( SDL_PointInRect( &point, &ReturnButton ) ){
						Mix_HaltMusic();
						goto __menu;
						break;
					}
				}
			}
		}
		drawStart(render);
		SDL_RenderPresent(render);
	}

__menu:
	if (userData["hasTutorial"].empty()) {
		musnakeState = MU_STATE_GAMING;
		while (musnakeState == MU_STATE_GAMING) {
			Mix_HaltMusic();
			thisGame = new Game();
			thisGame->setRenderer(render);
			thisGame->init(bonusTutorialLevel);
			thisGame->run();
			delete thisGame;
		}
		userData["hasTutorial"] = 1;
	}
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
					if (!(panelTurning || classTurning)) {
						Mix_FadeOutMusic(100);
						goto __start;
					}
					break;
				case SDLK_UP:
					if (!(panelTurning || classTurning)) panelTurning = -200;
					break;
				case SDLK_DOWN:
					if (!(panelTurning || classTurning)) panelTurning = 200;
					break;
				case SDLK_LEFT:
					if (!(panelTurning || classTurning)) classTurning = -200;
					break;
				case SDLK_RIGHT:
					if (!(panelTurning || classTurning)) classTurning = 200;
					break;
				case SDLK_RETURN:
				case SDLK_RETURN2:
					if (!(panelTurning || classTurning)) {
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
				case SDL_MOUSEBUTTONDOWN:
					if( evt.button.button == SDL_BUTTON_LEFT && !(classTurning || panelTurning ) ){
						SDL_Point point{evt.button.x, evt.button.y};
						SDL_Rect BackButton, UPButton, DownButton, PlayButton, LeftButton, RightButton, ImgButton ;
						BackButton = { 0,0,150,60 };
						UPButton = { 0,100,150,60 };
						DownButton = { 0,170,150,60 };
						PlayButton = { 0,520,150,60 };
						LeftButton = { 305,40,40,40 };
						RightButton = { 605,40,40,40 };
						ImgButton = {200, 200, 200, 200};
						if( SDL_PointInRect( &point, &BackButton ) ) {
							goto __start;
						}
						else if( SDL_PointInRect( &point, &UPButton ) ) {
							panelTurning = -200;
						}
						else if( SDL_PointInRect( &point, &DownButton ) ) {
							panelTurning = 200;
						}
						else if( SDL_PointInRect( &point, &PlayButton ) || SDL_PointInRect( &point, &ImgButton ) ) {
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
						else if( SDL_PointInRect( &point, &LeftButton ) ) {
							classTurning = -200;
						}
						else if( SDL_PointInRect( &point, &RightButton ) ) {
							classTurning = 200;
						}
						
					}
					break;
			}
		}
		drawPanels(render, &nowLevel,&nowClass, &panelTurning, &classTurning);
		SDL_RenderPresent(render);
	}
	
	if (Mix_PlayingMusic()) Mix_HaltMusic();

	unload();

	TTF_Quit();
	IMG_Quit();
	Mix_Quit();
	SDL_DestroyRenderer(render);
	SDL_DestroyWindow(window);
	SDL_Quit();

	return 0;
}

void load(SDL_Renderer* render) {
	// ��˵����Ӧ���Ǵ������ļ������������ʼ���ظ�ģ��ֽ׶ξ���д����
	SDL_Surface* picSurf, * tmpSurf;
	SDL_Texture* tmpTex;
	SDL_Color tmpColor = { 255, 255, 255, 255 };
	SDL_Rect tmpRect = { 0,0,170,1000 };

	// ���ﶨλ�ļ���λ��
	char tmpPath[256];

	// װ������
	catPath(tmpPath, (char*)"font\\SHOWG.TTF");
	titleMusnakeFont = TTF_OpenFont(tmpPath, 144);  // ����͵����棡
	gamePauseTitleFont =
	gameLoseTitleFont = TTF_OpenFont(tmpPath, 80);
	gameWinLengthnumFont =
	gameWinScorelabelFont = TTF_OpenFont(tmpPath, 50);
	gameScorenumFont =
	gameCombonumFont = TTF_OpenFont(tmpPath, 40);
	gameCombolabelFont = 
	menuSongBestNumFont = TTF_OpenFont(tmpPath, 20);

	catPath(tmpPath, (char*)"font\\msyhbd.ttc");
	titleAuthorFont = TTF_OpenFont(tmpPath, 16);
	menuSongtimeFont = 
	menuSongbyFont = 
	menuSongBestTextFont = 
	gameScorelabelFont = 
	gameLoseSongnameFont = TTF_OpenFont(tmpPath, 20);
	menuSongnameFont = 
	gameToastFont = 
	gamePauseSongnameFont = 
	gameWinSongnameFont =
	gameWinScorelabelFont = 
	gameWinLengthlabelFont = 
	gameWinNewBestFont = 
	menuClassNameFont = TTF_OpenFont(tmpPath, 30);

	// װ����Ŀ��Ϣ
	loadUserData();
	loadLevels();

	// װ�ر������BGM
	catPath(tmpPath, (char*)"sound\\bgm.mp3");
	titleBGM = Mix_LoadMUS(tmpPath);

	// װ�ر�����汳��ͼ
	catPath(tmpPath, (char*)"image\\menu_bg.png");
	picSurf = IMG_Load(tmpPath);
	titleBGFlame = new Flame(picSurf, NULL, -1);
	titleLBGFlame = new Flame(picSurf, &tmpRect, -1);
	tmpRect.x = 780;
	tmpRect.w = 20;
	titleRBGFlame = new Flame(picSurf, &tmpRect, -1);
	SDL_FreeSurface(picSurf);

	// װ��ENTER��ʾ��
	catPath(tmpPath, (char*)"image\\button_enter.png");
	picSurf = IMG_Load(tmpPath);
	titleEnterButtonFlame = new Flame(picSurf, NULL, -1);
	SDL_FreeSurface(picSurf);

	// װ��BACK��ʾ��
	catPath(tmpPath, (char*)"image\\button_back.png");
	picSurf = IMG_Load(tmpPath);
	menuBackButtonFlame = gamePauseBackButtonLFlame = gameOverBackButtonFlame = new Flame(picSurf, NULL, -1);
	SDL_FreeSurface(picSurf);

	// װ��UP��ʾ��
	catPath(tmpPath, (char*)"image\\button_up.png");
	picSurf = IMG_Load(tmpPath);
	menuUpButtonFlame = gamePauseUpButtonFlame = new Flame(picSurf, NULL, -1);
	SDL_FreeSurface(picSurf);

	// װ��DOWN��ʾ��
	catPath(tmpPath, (char*)"image\\button_down.png");
	picSurf = IMG_Load(tmpPath);
	menuDownButtonFlame = gamePauseDownButtonFlame = new Flame(picSurf, NULL, -1);
	SDL_FreeSurface(picSurf);

	// װ��PLAY��ʾ��
	catPath(tmpPath, (char*)"image\\button_play.png");
	picSurf = IMG_Load(tmpPath);
	menuPlayButtonFlame = new Flame(picSurf, NULL, -1);
	SDL_FreeSurface(picSurf);

	// װ��CLASS��ʾ��
	catPath(tmpPath, (char*)"image\\button_class.png");
	picSurf = IMG_Load(tmpPath);
	menuClassButtonFlame = new Flame(picSurf, NULL, -1);
	SDL_FreeSurface(picSurf);

	// װ��RESUME��ʾ��
	catPath(tmpPath, (char*)"image\\button_resume_nc.png");
	picSurf = IMG_Load(tmpPath);
	gamePauseResumeButtonFlame[0] = new Flame(picSurf, NULL, -1);
	SDL_FreeSurface(picSurf);

	catPath(tmpPath, (char*)"image\\button_resume_c.png");
	picSurf = IMG_Load(tmpPath);
	gamePauseResumeButtonFlame[1] = new Flame(picSurf, NULL, -1);
	SDL_FreeSurface(picSurf);

	// װ��RETRY��ʾ��
	catPath(tmpPath, (char*)"image\\button_retry_nc.png");
	picSurf = IMG_Load(tmpPath);
	gamePauseRetryButtonFlame[0] = new Flame(picSurf, NULL, -1);
	SDL_FreeSurface(picSurf);

	catPath(tmpPath, (char*)"image\\button_retry_c.png");
	picSurf = IMG_Load(tmpPath);
	gamePauseRetryButtonFlame[1] = new Flame(picSurf, NULL, -1);
	SDL_FreeSurface(picSurf);

	// װ��BACK��ʾ��
	catPath(tmpPath, (char*)"image\\button_back_nc.png");
	picSurf = IMG_Load(tmpPath);
	gamePauseBackButtonFlame[0] = new Flame(picSurf, NULL, -1);
	SDL_FreeSurface(picSurf);

	catPath(tmpPath, (char*)"image\\button_back_c.png");
	picSurf = IMG_Load(tmpPath);
	gamePauseBackButtonFlame[1] = new Flame(picSurf, NULL, -1);
	SDL_FreeSurface(picSurf);

	// װ��RETRY��ʾ��
	catPath(tmpPath, (char*)"image\\button_retry.png");
	picSurf = IMG_Load(tmpPath);
	gameOverRetryButtonFlame = new Flame(picSurf, NULL, -1);
	SDL_FreeSurface(picSurf);

	// װ��OK��ʾ��
	catPath(tmpPath, (char*)"image\\button_ok.png");
	picSurf = IMG_Load(tmpPath);
	gameOverOKButtonFlame = new Flame(picSurf, NULL, -1);
	SDL_FreeSurface(picSurf);

	// װ��Noteͼ
	catPath(tmpPath, (char*)"image\\notesign.png");
	picSurf = IMG_Load(tmpPath);
	notesignFlame[0] = new Flame(picSurf, NULL, -1);
	SDL_FreeSurface(picSurf);

	catPath(tmpPath, (char*)"image\\notesign_fever.png");
	picSurf = IMG_Load(tmpPath);
	notesignFlame[1] = new Flame(picSurf, NULL, -1);
	SDL_FreeSurface(picSurf);

	catPath(tmpPath, (char*)"image\\notesign_edge.png");
	picSurf = IMG_Load(tmpPath);
	notesignFlame[2] = new Flame(picSurf, NULL, -1);
	SDL_FreeSurface(picSurf);

	// װ��Ѫ��ͼ
	catPath(tmpPath, (char*)"image\\hp.png");
	picSurf = IMG_Load(tmpPath);
	for (int i = 0;i < 6;i++) {
		SDL_Rect srect = { i * 20, 0, 20, 20 };

		hpFlame[i / 2][i & 1] = new Flame(picSurf, &srect, -1);
		hpFlame[i / 2][i & 1]->setNext(nullptr);
	}
	SDL_FreeSurface(picSurf);

	// װ��ʳ��ͼ
	catPath(tmpPath, (char*)"image\\food_0.png");
	picSurf = IMG_Load(tmpPath);
	foodFlame[0] = new Flame(picSurf, NULL, -1);
	SDL_FreeSurface(picSurf);
	foodFlame[0]->setNext(nullptr);

	catPath(tmpPath, (char*)"image\\food_1.png");
	picSurf = IMG_Load(tmpPath);
	foodFlame[1] = new Flame(picSurf, NULL, -1);
	SDL_FreeSurface(picSurf);
	foodFlame[1]->setNext(nullptr);

	catPath(tmpPath, (char*)"image\\food_2.png");
	picSurf = IMG_Load(tmpPath);
	foodFlame[2] = new Flame(picSurf, NULL, -1);
	SDL_FreeSurface(picSurf);
	foodFlame[2]->setNext(nullptr);

	// 读取食物指示图
	catPath(tmpPath, (char*)"image\\foodpointer_0.png");
	picSurf = IMG_Load(tmpPath);
	foodPointerFlame[0][0] = new Flame(picSurf, NULL, -1);
	SDL_FreeSurface(picSurf);
	foodPointerFlame[0][0]->setNext(nullptr);

	catPath(tmpPath, (char*)"image\\foodpointer_1.png");
	picSurf = IMG_Load(tmpPath);
	foodPointerFlame[1][0] = new Flame(picSurf, NULL, -1);
	SDL_FreeSurface(picSurf);
	foodPointerFlame[1][0]->setNext(nullptr);

	catPath(tmpPath, (char*)"image\\foodpointer_2.png");
	picSurf = IMG_Load(tmpPath);
	foodPointerFlame[2][0] = new Flame(picSurf, NULL, -1);
	SDL_FreeSurface(picSurf);
	foodPointerFlame[2][0]->setNext(nullptr);

	catPath(tmpPath, (char*)"image\\foodpointer_0L.png");
	picSurf = IMG_Load(tmpPath);
	foodPointerFlame[0][1] = new Flame(picSurf, NULL, -1);
	SDL_FreeSurface(picSurf);
	foodPointerFlame[0][1]->setNext(nullptr);

	catPath(tmpPath, (char*)"image\\foodpointer_1L.png");
	picSurf = IMG_Load(tmpPath);
	foodPointerFlame[1][1] = new Flame(picSurf, NULL, -1);
	SDL_FreeSurface(picSurf);
	foodPointerFlame[1][1]->setNext(nullptr);

	catPath(tmpPath, (char*)"image\\foodpointer_2L.png");
	picSurf = IMG_Load(tmpPath);
	foodPointerFlame[2][1] = new Flame(picSurf, NULL, -1);
	SDL_FreeSurface(picSurf);
	foodPointerFlame[2][1]->setNext(nullptr);

	// 读取地格图像
	catPath(tmpPath, (char*)"image\\grid_earth.png");
	picSurf = IMG_Load(tmpPath);
	gridFlame = new Flame(picSurf, NULL, -1);
	SDL_FreeSurface(picSurf);

	catPath(tmpPath, (char*)"image\\grid_water.png");
	picSurf = IMG_Load(tmpPath);
	gridWaterFlame = new Flame(picSurf, NULL, -1);
	SDL_FreeSurface(picSurf);

	catPath(tmpPath, (char*)"image\\grid_none.png");
	picSurf = IMG_Load(tmpPath);
	gridDarkFlame = new Flame(picSurf, NULL, -1);
	SDL_FreeSurface(picSurf);

	catPath(tmpPath, (char*)"image\\grid_block.png");
	picSurf = IMG_Load(tmpPath);
	gridBlockFlame = new Flame(picSurf, NULL, -1);
	SDL_FreeSurface(picSurf);

	// ��ʼװ���ַ�ͼ
	catPath(tmpPath, (char*)"image\\char.png");
	picSurf = IMG_Load(tmpPath);
	for (int i = 0;i < 6;i++) {  // 6����������
		for (int j = 0;j < 16;j++) {  // ÿ��16���ַ�
			SDL_Rect srect = { j * 80, i * 160, 80, 160 };
			charFlame[16 * i + j] = new Flame(picSurf, &srect, -1);
		}
	}
	SDL_FreeSurface(picSurf);

	// ��ʼװ����ͷͼ��������ͷ��Ӧ��ö�ٴ�14�ţ�MU_SNAKE_FLAME_HEAD_0toUP����ʼ��29�Ž���
	catPath(tmpPath, (char*)"image\\snake_0_head.png");
	picSurf = IMG_Load(tmpPath);
	for (int i = 0;i < 16;i++) {  // 16����ͷ�˶����
		Flame* flames[8];
		for (int j = 0;j < 8;j++) {  // ÿ���˶������8֡
			SDL_Rect srect = { j * 20, i * 20, 20, 20 };

			flames[j] = new Flame(picSurf, &srect, 9);
			flames[j]->setGroupId(MU_SNAKE_FLAME_HEAD_0toUP + i);
			if (j)flames[j - 1]->setNext(flames[j]);
		}
		snakeFlame[MU_SNAKE_FLAME_HEAD_0toUP + i] = flames[0];
	}
	SDL_FreeSurface(picSurf);

	// ��ʼװ����βͼ��������β��Ӧ��ö�ٴ�30�ţ�MU_SNAKE_FLAME_TAIL_UPshake����ʼ��49�Ž���
	catPath(tmpPath, (char*)"image\\snake_0_tail.png");
	picSurf = IMG_Load(tmpPath);
	for (int i = 0;i < 20;i++) {  // 20��������β�����
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

	// ��ʼװ��һ������ͼ��һ�������Ӧ��ö�ٴ�0�ţ�MU_SNAKE_FLAME_HEAD_UP����ʼ��13�Ž���
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

	// Ȼ��Բ�ͬ��֡����й�����ܳ��
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

	// ����Title����Ҫ�õ�����
	tmpSurf = TTF_RenderText_Blended(titleMusnakeFont, "MUSNAKE", tmpColor);
	titleMusnakeFlame = new Flame(tmpSurf, NULL, -1);
	SDL_FreeSurface(tmpSurf);
	tmpSurf = TTF_RenderText_Blended(titleAuthorFont, "By Sieroy & StdCat", tmpColor);
	titleAuthorFlame = new Flame(tmpSurf, NULL, -1);
	SDL_FreeSurface(tmpSurf);

	// װ��Pause���������ͼ
	catPath(tmpPath, (char*)"image\\mask_gamepause.png");
	picSurf = IMG_Load(tmpPath);
	gamePauseBGMask = new Flame(picSurf, NULL, -1);
	SDL_FreeSurface(picSurf);

	// ����GamePause����Ҫ�õ�������
	tmpSurf = TTF_RenderText_Blended(gamePauseTitleFont, "- PAUSED -", tmpColor);
	gamePauseTitleFlame = new Flame(tmpSurf, NULL, -1);
	SDL_FreeSurface(tmpSurf);

	tmpSurf = TTF_RenderUTF8_Blended(gameWinNewBestFont, "\xe6\x96\xb0\xe7\xba\xaa\xe5\xbd\x95\xef\xbc\x81\xef\xbc\x81", tmpColor);;
	gamewinNewBestFlame = new Flame(tmpSurf, NULL, -1);
	SDL_FreeSurface(tmpSurf);

	// װ��GameWin����ı���
	catPath(tmpPath, (char*)"image\\gamewin_bg.png");
	picSurf = IMG_Load(tmpPath);
	gamewinBGFlame = new Flame(picSurf, NULL, -1);
	SDL_FreeSurface(picSurf);

}

void unload() {
	flushUserData();
}

void drawStart(SDL_Renderer* render) {  // ������Ϸ��ʼҳ��
	static int dt = 0;
	dt += getTimeDelta();
	dt %= 3290;
	titleBGFlame->draw(render, 0, -dt / 10);
	titleMusnakeFlame->draw(render, 75, 120);
	titleEnterButtonFlame->draw(render, 300, 440);
	titleAuthorFlame->draw(render, 5, 570);
}

void drawPanels(SDL_Renderer* render, Level** nowPanel, LevelClass** nowClass, int* turningLevel, int* turningClass) {
	static int dt = 0;
	static int turningFlag = 0;
	SDL_Rect prect = { 170, 0, 610, 600 };
	Level* panel = *nowPanel;
	LevelClass* levelClass = *nowClass;
	dt += getTimeDelta();
	dt %= 3290;
	titleBGFlame->draw(render, 0, -dt / 10);
	SDL_RenderFillRect(render, &prect);
	if (!*turningClass) {  // û���л�����
		menuBackButtonFlame->draw(render, 0, 0);
		menuUpButtonFlame->draw(render, 0, 100);
		menuDownButtonFlame->draw(render, 0, 170);
		menuPlayButtonFlame->draw(render, 0, 520);
		if (!*turningLevel) {  // �����Ƶ�ǰ��Ŀ
			SDL_Rect rect = {200, 200, 200, 200};
			panel->cover->draw(render, &rect);
			panel->nameFlm->draw(render, 430, 200);
			panel->byFlm->draw(render, 430, 244);
			panel->timeFlm->draw(render, 430, 280);
			if (panel->bestFlm) panel->bestFlm->draw(render, 430, 320);
		}
		else if (*turningLevel > 0) {  // ���¹��������Ҫ�������ƶ���Ч��
			Level* lp = panel->next;
			SDL_Rect re1 = { 200, 200 + *turningLevel * 3, 200, 200 }, re2 = { 200, -400 + *turningLevel * 3, 200, 200 };
			if (turningFlag == 0) {  // ��������״ε���
				turningFlag = 1;  // 1����ԭ�ȵ�������������״̬
				Mix_FadeOutMusic(190);
			}
			lp->cover->draw(render, &re1);
			lp->nameFlm->draw(render, 430, 200 + *turningLevel * 3);
			lp->byFlm->draw(render, 430, 244 + *turningLevel * 3);
			lp->timeFlm->draw(render, 430, 280 + *turningLevel * 3);
			if (lp->bestFlm) lp->bestFlm->draw(render, 430, 320 + *turningLevel * 3);

			panel->cover->draw(render, &re2);
			panel->nameFlm->draw(render, 430, -400 + *turningLevel * 3);
			panel->byFlm->draw(render, 430, -350 + *turningLevel * 3);
			panel->timeFlm->draw(render, 430, -320 + *turningLevel * 3);
			if (panel->bestFlm) panel->bestFlm->draw(render, 430, -280 + *turningLevel * 3);

			*turningLevel -= getTimeDelta();
			if (*turningLevel <= 0) {
				SDL_RenderClear(render);
				titleBGFlame->draw(render, 0, -dt / 10);
				menuBackButtonFlame->draw(render, 0, 0);
				menuUpButtonFlame->draw(render, 0, 100);
				menuDownButtonFlame->draw(render, 0, 170);
				menuPlayButtonFlame->draw(render, 0, 520);
				SDL_RenderFillRect(render, &prect);
				re1.y = 200;
				lp->cover->draw(render, &re1);
				lp->nameFlm->draw(render, 430, 200);
				lp->byFlm->draw(render, 430, 244);
				lp->timeFlm->draw(render, 430, 280);
				if (lp->bestFlm) lp->bestFlm->draw(render, 430, 320);

				menuClassButtonFlame->draw(render, 275, 30);
				(*nowClass)->nameFlm->draw(render, 412, 37);

				SDL_RenderPresent(render);

				*turningLevel = 0;
				turningFlag = 0;
				Mix_PlayMusic(lp->sample, -1);
				*nowPanel = lp;
			}
		}
		else if (*turningLevel < 0) {  // ���Ϲ��������Ҫ�������ƶ���Ч��
			Level* lp = panel->prev;
			SDL_Rect re1 = { 200, 200 + *turningLevel * 3, 200, 200 }, re2 = { 200, 800 + *turningLevel * 3, 200, 200 };
			if (turningFlag == 0) {  // ��������״ε���
				turningFlag = 1;  // 1����ԭ�ȵ�������������״̬
				Mix_FadeOutMusic(190);
			}
			lp->cover->draw(render, &re1);
			lp->nameFlm->draw(render, 430, 200 + *turningLevel * 3);
			lp->byFlm->draw(render, 430, 244 + *turningLevel * 3);
			lp->timeFlm->draw(render, 430, 280 + *turningLevel * 3);
			if (lp->bestFlm) lp->bestFlm->draw(render, 430, 320 + *turningLevel * 3);

			panel->cover->draw(render, &re2);
			panel->nameFlm->draw(render, 430, 800 + *turningLevel * 3);
			panel->byFlm->draw(render, 430, 850 + *turningLevel * 3);
			panel->timeFlm->draw(render, 430, 880 + *turningLevel * 3);
			if (panel->bestFlm) panel->bestFlm->draw(render, 430, 920 + *turningLevel * 3);

			*turningLevel += getTimeDelta();
			if (*turningLevel >= 0) {
				SDL_RenderClear(render);
				titleBGFlame->draw(render, 0, -dt / 10);
				menuBackButtonFlame->draw(render, 0, 0);
				menuUpButtonFlame->draw(render, 0, 100);
				menuDownButtonFlame->draw(render, 0, 170);
				menuPlayButtonFlame->draw(render, 0, 520);
				SDL_RenderFillRect(render, &prect);
				re1.y = 200;
				lp->cover->draw(render, &re1);
				lp->nameFlm->draw(render, 430, 200);
				lp->byFlm->draw(render, 430, 244);
				lp->timeFlm->draw(render, 430, 280);
				if (lp->bestFlm) lp->bestFlm->draw(render, 430, 320);

				menuClassButtonFlame->draw(render, 275, 30);
				(*nowClass)->nameFlm->draw(render, 412, 37);

				SDL_RenderPresent(render);

				*turningLevel = 0;
				turningFlag = 0;
				Mix_PlayMusic(lp->sample, -1);
				*nowPanel = lp;
			}
		}
		menuClassButtonFlame->draw(render, 275, 30);
		levelClass->nameFlm->draw(render, 412, 37);
	}
	else if (*turningClass > 0) {  // ��������
		LevelClass* clp = levelClass->next;
		Level* lp = clp->levels;
		SDL_Rect re1 = { 200 + *turningClass * 3, 200, 200, 200 }, re2 = { -400 + *turningClass * 3,200, 200, 200 };
		if (turningFlag == 0) {
			turningFlag = 1;
			Mix_FadeOutMusic(190);
		}
		menuClassButtonFlame->draw(render, 275 + *turningClass * 3, 30);
		menuClassButtonFlame->draw(render, -325 + *turningClass * 3, 30);
		levelClass->nameFlm->draw(render, -188 + *turningClass * 3, 37);
		clp->nameFlm->draw(render, 412 + *turningClass * 3, 37);

		lp->cover->draw(render, &re1);
		lp->nameFlm->draw(render, 430 + *turningClass * 3, 200);
		lp->byFlm->draw(render, 430 + *turningClass * 3, 244);
		lp->timeFlm->draw(render, 430 + *turningClass * 3, 280);
		if (lp->bestFlm) lp->bestFlm->draw(render, 430 + *turningClass * 3, 320);

		panel->cover->draw(render, &re2);
		panel->nameFlm->draw(render, -170 + *turningClass * 3, 200);
		panel->byFlm->draw(render, -170 + *turningClass * 3, 244);
		panel->timeFlm->draw(render, -170 + *turningClass * 3, 280);
		if (panel->bestFlm) panel->bestFlm->draw(render, -170 + *turningClass * 3, 320);

		titleLBGFlame->draw(render, 0, -dt / 10);
		titleRBGFlame->draw(render, 780, -dt / 10);
		menuBackButtonFlame->draw(render, 0, 0);
		menuUpButtonFlame->draw(render, 0, 100);
		menuDownButtonFlame->draw(render, 0, 170);
		menuPlayButtonFlame->draw(render, 0, 520);

		*turningClass -= getTimeDelta();
		if (*turningClass <= 0) {
			SDL_RenderClear(render);
			titleBGFlame->draw(render, 0, -dt / 10);
			menuBackButtonFlame->draw(render, 0, 0);
			menuUpButtonFlame->draw(render, 0, 100);
			menuDownButtonFlame->draw(render, 0, 170);
			menuPlayButtonFlame->draw(render, 0, 520);
			SDL_RenderFillRect(render, &prect);

			menuClassButtonFlame->draw(render, 275, 30);
			clp->nameFlm->draw(render, 412, 37);

			re1.x = 200;
			lp->cover->draw(render, &re1);
			lp->nameFlm->draw(render, 430, 200);
			lp->byFlm->draw(render, 430, 244);
			lp->timeFlm->draw(render, 430, 280);
			if (lp->bestFlm) lp->bestFlm->draw(render, 430, 320);

			SDL_RenderPresent(render);

			*turningClass = 0;
			turningFlag = 0;
			Mix_PlayMusic(lp->sample, -1);
			*nowClass = clp;
			*nowPanel = lp;
		}
	}
	else if (*turningClass < 0) {  // ��������
		LevelClass* clp = levelClass->prev;
		Level* lp = clp->levels;
		SDL_Rect re1 = { 200 + *turningClass * 3, 200, 200, 200 }, re2 = { 800 + *turningClass * 3, 200, 200, 200 };
		if (turningFlag == 0) {
			turningFlag = 1;
			Mix_FadeOutMusic(190);
		}
		menuClassButtonFlame->draw(render, 275 + *turningClass * 3, 30);
		menuClassButtonFlame->draw(render, 875 + *turningClass * 3, 30);
		levelClass->nameFlm->draw(render, 1012 + *turningClass * 3, 37);
		clp->nameFlm->draw(render, 412 + *turningClass * 3, 37);

		lp->cover->draw(render, &re1);
		lp->nameFlm->draw(render, 430 + *turningClass * 3, 200);
		lp->byFlm->draw(render, 430 + *turningClass * 3, 244);
		lp->timeFlm->draw(render, 430 + *turningClass * 3, 280);
		if (lp->bestFlm) lp->bestFlm->draw(render, 430 + +*turningClass * 3, 320);

		panel->cover->draw(render, &re2);
		panel->nameFlm->draw(render, 1030 + *turningClass * 3, 200);
		panel->byFlm->draw(render, 1030 + *turningClass * 3, 244);
		panel->timeFlm->draw(render, 1030 + *turningClass * 3, 280);
		if (panel->bestFlm) panel->bestFlm->draw(render, 1030 + *turningClass * 3, 320);

		titleLBGFlame->draw(render, 0, -dt / 10);
		titleRBGFlame->draw(render, 780, -dt / 10);
		menuBackButtonFlame->draw(render, 0, 0);
		menuUpButtonFlame->draw(render, 0, 100);
		menuDownButtonFlame->draw(render, 0, 170);
		menuPlayButtonFlame->draw(render, 0, 520);

		*turningClass += getTimeDelta();
		if (*turningClass >= 0) {
			SDL_RenderClear(render);
			titleBGFlame->draw(render, 0, -dt / 10);
			menuBackButtonFlame->draw(render, 0, 0);
			menuUpButtonFlame->draw(render, 0, 100);
			menuDownButtonFlame->draw(render, 0, 170);
			menuPlayButtonFlame->draw(render, 0, 520);
			SDL_RenderFillRect(render, &prect);

			menuClassButtonFlame->draw(render, 275, 30);
			clp->nameFlm->draw(render, 412, 37);

			re1.x = 200;
			lp->cover->draw(render, &re1);
			lp->nameFlm->draw(render, 430, 200);
			lp->byFlm->draw(render, 430, 244);
			lp->timeFlm->draw(render, 430, 280);
			if (lp->bestFlm) lp->bestFlm->draw(render, 430, 320);

			SDL_RenderPresent(render);

			*turningClass = 0;
			turningFlag = 0;
			Mix_PlayMusic(lp->sample, -1);
			*nowClass = clp;
			*nowPanel = lp;
		}
	}


}
