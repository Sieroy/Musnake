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
int bonusM = 0;

void load(SDL_Renderer* render);
void unload();
void drawStart(SDL_Renderer* render);
void drawPanels(SDL_Renderer* render, Level** nowPanel, LevelClass** nowClass, int* turningLevel, int* turningClass);
void drawConfig(SDL_Renderer* render, int keyread);

int main(int argc, char* argv[]) {
	SDL_Event evt;
	SDL_Surface* tmpSurf = nullptr;
	Level* nowLevel = nullptr;
	LevelClass* nowClass = nullptr;
	int panelTurning = 0;
	int classTurning = 0;
	int bonus = 0;
	int settingKey = -1;
	bool settingDelta = false;
	SDL_Point settingDeltaPoint;
	int settingDeltaOri = 0;

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
				case SDLK_UP:
					if (bonus == 0 || bonus == 1) {
						bonus++;
						bonusM = 199;
					}
					else 
						bonus = 1;
						bonusM = 199;
					break;
				case SDLK_DOWN:
					if (bonus == 2 || bonus == 3) {
						bonus++;
						bonusM = 399;
					}
					else
						bonus = 0;
					break;
				case SDLK_LEFT:
					if (bonus == 4 || bonus == 6) {
						bonus++;
						bonusM = 599;
					}
					else
						bonus = 0;
					break;
				case SDLK_RIGHT:
					if (bonus == 5 || bonus == 7) {
						bonus++;
						bonusM = 799;
					}
					else
						bonus = 0;
					break;
				case SDLK_a:
					if (bonus == 9) {
						bonus++;
						bonusM = 999;
					}
					else
						bonus = 0;
					break;
				case SDLK_b:
					if (bonus == 8) {
						bonus++;
						bonusM = 1199;
					}
					else
						bonus = 0;
					break;
				case SDLK_RETURN:
				case SDLK_RETURN2:
				case SDLK_KP_ENTER:
					Mix_HaltMusic();
					if (bonus == 10) {
						musnakeState = MU_STATE_GAMING;
						while (musnakeState == MU_STATE_GAMING) {
							Mix_HaltMusic();
							thisGame = new Game();
							thisGame->setRenderer(render);
							thisGame->init(bonusInfoLevel);
							thisGame->run();
							delete thisGame;
						}
						Mix_FadeInMusic(titleBGM, -1, 1000);
						bonusM = 0;
						bonus = 0;
					}
					else {
						bonus = 0;
						bonusM = 0;
						goto __menu;
					}
					break;
				case SDLK_ESCAPE:
					musnakeState = MU_STATE_OVER;
					break;
				default:
					bonusM = 0;
					bonus = 0;
				}
			case SDL_MOUSEBUTTONDOWN:
				if( evt.button.button == SDL_BUTTON_LEFT){
					SDL_Point point{evt.button.x, evt.button.y};
					SDL_Rect ReturnButton{ 300,410,150,60 };
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
	goto __end;

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
				case SDLK_TAB:
					goto __config;
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
				case SDLK_KP_ENTER:
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
						SDL_Rect BackButton, UPButton, DownButton, PlayButton, LeftButton, RightButton, ImgButton, ConfigButton;
						BackButton = { 0,0,150,60 };
						UPButton = { 0,100,150,60 };
						DownButton = { 0,170,150,60 };
						ConfigButton = { 0,380, 150, 60 };
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
						else if (SDL_PointInRect(&point, &ConfigButton)) {
							goto __config;
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
	goto __end;

__config:
	Mix_PlayMusic(configBGM, -1);
	settingKey = -1;
	while (musnakeState) {

		updateTime();
		SDL_RenderClear(render);

		while (SDL_PollEvent(&evt)) {
			switch (evt.type) {
			case SDL_QUIT:
				musnakeState = MU_STATE_OVER;
				break;
			case SDL_KEYDOWN:
				switch (evt.key.keysym.sym) {
				case SDLK_ESCAPE:
				case SDLK_RETURN:
				case SDLK_RETURN2:
				case SDLK_KP_ENTER:
					if (settingKey == -1) {
						userData["settings"]["timeOffset"] = noteDelta;
						flushUserData();
						goto __menu;
					}
					else
						settingKey = -1;
					break;
				default:
					if (settingKey >= 0) {
						updateUserKeySetting(settingKey, evt.key.keysym.sym);
						settingKey = -1;
					}
				}
			case SDL_MOUSEBUTTONDOWN:
				if (evt.button.button == SDL_BUTTON_LEFT && !(classTurning || panelTurning)) {
					SDL_Point point{ evt.button.x, evt.button.y };
					SDL_Rect BackButton, OKButton, KeyUpButton, KeyDownButton, KeyLeftButton, KeyRightButton, DeltaBarRect, DeltaPinRect;
					BackButton = { 0,0,150,60 };
					OKButton = { 0,520,150,60 };
					KeyUpButton = { 240, 100, 80, 160 };
					KeyDownButton = { 380, 100, 80, 160 };
					KeyLeftButton = { 520, 100, 80, 160 };
					KeyRightButton = { 660, 100, 80, 160 };
					DeltaBarRect = { 240, 380, 500, 6 };
					DeltaPinRect = { 480 + noteDelta / 4, 368 , 20, 30};
					if (settingKey == -1 && !settingDelta) {
						if (SDL_PointInRect(&point, &BackButton) || SDL_PointInRect(&point, &OKButton)) {
							userData["settings"]["timeOffset"] = noteDelta;
							flushUserData();
							goto __menu;
						}
						else if (SDL_PointInRect(&point, &KeyUpButton))
							settingKey = MU_KEY_UP;
						else if (SDL_PointInRect(&point, &KeyRightButton))
							settingKey = MU_KEY_RIGHT;
						else if (SDL_PointInRect(&point, &KeyDownButton))
							settingKey = MU_KEY_DOWN;
						else if (SDL_PointInRect(&point, &KeyLeftButton))
							settingKey = MU_KEY_LEFT;
						else if (SDL_PointInRect(&point, &DeltaBarRect)) {
							settingDelta = true;
							settingDeltaPoint = point;
							settingDeltaOri = noteDelta = (point.x - 490) * 4;
						}
						else if (SDL_PointInRect(&point, &DeltaPinRect)) {
							settingDelta = true;
							settingDeltaPoint = point;
							settingDeltaOri = noteDelta;
						}
					}
					else {
						settingKey = -1;
					}
				}
				break;
			case SDL_MOUSEMOTION:
				if (settingDelta) {
					noteDelta = settingDeltaOri + (evt.motion.x - settingDeltaPoint.x) * 4;
					if (noteDelta < -1000) noteDelta = -1000;
					else if (noteDelta > 1000) noteDelta = 1000;
				}
				break;
			case SDL_MOUSEBUTTONUP:
				userData["settings"]["timeOffset"] = noteDelta;
				settingDelta = false;
				break;
			}
		}
		drawConfig(render, settingKey);
		SDL_RenderPresent(render);
	}

__end:
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
	SDL_Surface* picSurf;
	SDL_Color tmpColor = { 255, 255, 255, 255 };
	SDL_Rect tmpRect = { 0,0,170,1000 };

	// ���ﶨλ�ļ���λ��
	char tmpPath[256];

	// װ������
	catPath(tmpPath, (char*)"font\\SHOWG.TTF");
	titleMusnakeFont = TTF_OpenFont(tmpPath, 144);  // ����͵����棡
	gamePauseTitleFont =
	gameLoseTitleFont = TTF_OpenFont(tmpPath, 80);
	numberTotalFont =
	gameWinScorelabelFont = TTF_OpenFont(tmpPath, 50);
	numberHitsFont =
	numberScoreFont = TTF_OpenFont(tmpPath, 40);
	menuSongBestDataFont = TTF_OpenFont(tmpPath, 20);
	gameCombolabelFont = TTF_OpenFont(tmpPath, 14);

	catPath(tmpPath, (char*)"font\\msyhbd.ttc");
	titleAuthorFont = 
	configKeyFont =
	numberFPSFont = TTF_OpenFont(tmpPath, 16);
	menuSongtimeFont = 
	menuSongbyFont = 
	menuSongBestTextFont =
	configDeltaLabelFont =
	gameScorelabelFont = 
	numberConfigDeltaFont = TTF_OpenFont(tmpPath, 20);
	menuSongnameFont = 
	configSettingFont =
	gameToastFont = 
	gameWinScorelabelFont = 
	gameWinLengthlabelFont = 
	gameWinNewBestFont = 
	menuClassNameFont = TTF_OpenFont(tmpPath, 30);

	// װ����Ŀ��Ϣ
	loadUserData();
	loadLevels();

	// װ�ر������BGM
	catPath(tmpPath, (char*)"sound\\bgm_title.mp3");
	titleBGM = Mix_LoadMUS(tmpPath);

	catPath(tmpPath, (char*)"sound\\bgm_config.mp3");
	configBGM = Mix_LoadMUS(tmpPath);

	// װ�ر�����汳��ͼ
	catPath(tmpPath, (char*)"image\\menu_bg.png");
	picSurf = IMG_Load(tmpPath);
	titleBGFlame = new Flame(picSurf, NULL, -1);
	titleLBGFlame = new Flame(picSurf, &tmpRect, -1);
	tmpRect.x = 780;
	tmpRect.w = 20;
	titleRBGFlame = new Flame(picSurf, &tmpRect, -1);
	SDL_FreeSurface(picSurf);

	titleEnterButtonFlame = loadFlameFromFile((char*)"image\\button_enter.png");
	menuBackButtonFlame = 
	configBackButtonFlame =
	gameOverBackButtonFlame = loadFlameFromFile((char*)"image\\button_back.png");
	menuConfigButtonFlame = loadFlameFromFile((char*)"image\\button_config.png");
	menuUpButtonFlame = loadFlameFromFile((char*)"image\\button_up.png");
	menuDownButtonFlame = loadFlameFromFile((char*)"image\\button_down.png");
	menuPlayButtonFlame = loadFlameFromFile((char*)"image\\button_play.png");
	menuClassButtonFlame = loadFlameFromFile((char*)"image\\button_class.png");
	configSetKeyFlame = loadFlameFromFile((char*)"image\\button_settingkey.png");
	configSetDeltaFlame = loadFlameFromFile((char*)"image\\button_settingbar.png");
	configSetPointerFlame = loadFlameFromFile((char*)"image\\button_settingpin.png");
	gamePauseResumeButtonFlame[0] = loadFlameFromFile((char*)"image\\button_resume_nc.png");
	gamePauseResumeButtonFlame[1] = loadFlameFromFile((char*)"image\\button_resume_c.png");
	gamePauseRetryButtonFlame[0] = loadFlameFromFile((char*)"image\\button_retry_nc.png");
	gamePauseRetryButtonFlame[1] = loadFlameFromFile((char*)"image\\button_retry_c.png");
	gamePauseBackButtonFlame[0] = loadFlameFromFile((char*)"image\\button_back_nc.png");
	gamePauseBackButtonFlame[1] = loadFlameFromFile((char*)"image\\button_back_c.png");
	gameOverRetryButtonFlame = loadFlameFromFile((char*)"image\\button_retry.png");
	configOKButtonFlame =
	gameOverOKButtonFlame = loadFlameFromFile((char*)"image\\button_ok.png");
	notesignFlame[0] = loadFlameFromFile((char*)"image\\notesign.png");
	notesignFlame[1] = loadFlameFromFile((char*)"image\\notesign_fever.png");
	notesignFlame[2] = loadFlameFromFile((char*)"image\\notesign_edge.png");

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
	foodFlame[0] = loadFlameFromFile((char*)"image\\food_0.png");
	foodFlame[1] = loadFlameFromFile((char*)"image\\food_1.png");
	foodFlame[2] = loadFlameFromFile((char*)"image\\food_2.png");

	// 读取食物指示图
	foodPointerFlame[0][0] = loadFlameFromFile((char*)"image\\foodpointer_0.png");
	foodPointerFlame[1][0] = loadFlameFromFile((char*)"image\\foodpointer_1.png");
	foodPointerFlame[2][0] = loadFlameFromFile((char*)"image\\foodpointer_2.png");
	foodPointerFlame[0][1] = loadFlameFromFile((char*)"image\\foodpointer_0L.png");
	foodPointerFlame[1][1] = loadFlameFromFile((char*)"image\\foodpointer_1L.png");
	foodPointerFlame[2][1] = loadFlameFromFile((char*)"image\\foodpointer_2L.png");

	// 读取地格图像
	gridFlame = loadFlameFromFile((char*)"image\\grid_earth.png");
	gridWaterFlame = loadFlameFromFile((char*)"image\\grid_water.png");
	gridDarkFlame = loadFlameFromFile((char*)"image\\grid_none.png");
	gridBlockFlame = loadFlameFromFile((char*)"image\\grid_block.png");

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
	titleMusnakeFlame = loadFlameForText(titleMusnakeFont, (char*)"MUSNAKE", &tmpColor);
	titleAuthorFlame = loadFlameForText(titleAuthorFont, (char*)"By Sieroy & StdCat", &tmpColor);

	text_KeyConf_Flame = loadFlameForUTF8(configSettingFont, (char*)"\xe6\xb8\xb8\xe6\x88\x8f\xe6\x8c\x89\xe9\x94\xae\xe8\xae\xbe\xe7\xbd\xae", &tmpColor);
	text_DeltaConf_Flame = loadFlameForUTF8(configSettingFont, (char*)"\xe8\x8a\x82\xe5\xa5\x8f\xe5\x81\x8f\xe7\xa7\xbb\xe8\xae\xbe\xe7\xbd\xae", &tmpColor);
	text_DeltaValue_Flame = loadFlameForUTF8(configDeltaLabelFont, (char*)"\xe5\x81\x8f\xe7\xa7\xbb\xe5\x80\xbc: ", &tmpColor);
	text_Score_Flame = loadFlameForUTF8(gameScorelabelFont, (char*)"\xe5\xbe\x97\xe5\x88\x86", &tmpColor);
	text_Hits_Flame = loadFlameForText(gameCombolabelFont, (char*)"Hits!", &tmpColor);
	text_TotalScore_Flame = loadFlameForUTF8(gameWinScorelabelFont, (char*)"\xe5\x88\x86\xe6\x95\xb0", &tmpColor);
	text_TotalLength_Flame = loadFlameForUTF8(gameWinLengthlabelFont, (char*)"\xe9\x95\xbf\xe5\xba\xa6", &tmpColor);

	tmpPath[1] = 0;
	for (char i = 0;i < 10;i++) {
		tmpPath[0] = i + '0';
		numberConfigDeltaFlame[i] = loadFlameForText(numberConfigDeltaFont, tmpPath, &tmpColor);
		numberFPSFlame[i] = loadFlameForText(numberFPSFont, tmpPath, &tmpColor);
		numberHitsFlame[i] = loadFlameForText(numberHitsFont, tmpPath, &tmpColor);
		numberScoreFlame[i] = loadFlameForText(numberScoreFont, tmpPath, &tmpColor);
		numberTotalFlame[i] = loadFlameForText(numberTotalFont, tmpPath, &tmpColor);
	}
	numberConfigDeltaFlame[10] = loadFlameForText(numberConfigDeltaFont, (char*)"-", &tmpColor);

	configFGMask = loadFlameFromFile((char*)"image\\mask_configkey.png");
	configFGMask->setAlpha(0);
	gamePauseBGMask = loadFlameFromFile((char*)"image\\mask_gamepause.png");

	// ����GamePause����Ҫ�õ�������
	gamePauseTitleFlame = loadFlameForText(gamePauseTitleFont, (char*)"- PAUSE -", &tmpColor);

	gamewinNewBestFlame = loadFlameForUTF8(gameWinNewBestFont, (char*)"\xe6\x96\xb0\xe7\xba\xaa\xe5\xbd\x95\xef\xbc\x81\xef\xbc\x81", &tmpColor);

	// װ��GameWin����ı���
	gamewinBGFlame = loadFlameFromFile((char*)"image\\gamewin_bg.png");

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
	titleAuthorFlame->draw(render, 5, 570);
	if(!bonusM)
		titleEnterButtonFlame->draw_centered(render, 400, 440);
	else {
		switch (bonusM / 200) {
		case 0:  // 0 - 199 上键的动效触发
			titleEnterButtonFlame->draw_centered(render, 400, 440 - bonusM / 4);
			bonusM -= getTimeDelta();
			if (bonusM < 0) bonusM = 0;
			break;
		case 1:  // 200 - 399 下键的动效触发
			titleEnterButtonFlame->draw_centered(render, 400, 390 + bonusM / 4);
			bonusM -= getTimeDelta();
			if (bonusM < 200) bonusM = 0;
			break;
		case 2:  // 400 - 599 左键的动效触发
			titleEnterButtonFlame->draw_centered(render, 500 - bonusM / 4, 440);
			bonusM -= getTimeDelta();
			if (bonusM < 400) bonusM = 0;
			break;
		case 3:  // 600 - 799 右键的动效触发
			titleEnterButtonFlame->draw_centered(render, bonusM / 4 + 250, 440);
			bonusM -= getTimeDelta();
			if (bonusM < 600) bonusM = 0;
			break;
		case 4:  // 800 - 999 A键的动效触发
			titleEnterButtonFlame->draw_centered(render, 400, 440, 1800 - 9 * bonusM / 5.);
			bonusM -= getTimeDelta();
			if (bonusM < 800) bonusM = 0;
			break;
		case 5:  // 1000 - 1199 B键的动效触发
			titleEnterButtonFlame->draw_centered(render, 400, 440, 9 * bonusM / 5. - 1800);
			bonusM -= getTimeDelta();
			if (bonusM < 1000) bonusM = 0;
			break;
		}
	}
	drawNumber(render, numberFPSFlame, fps, 750, 580);
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
		menuConfigButtonFlame->draw(render, 0, 380);
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
				menuConfigButtonFlame->draw(render, 0, 380);
				menuPlayButtonFlame->draw(render, 0, 520);
				SDL_RenderFillRect(render, &prect);
				re1.y = 200;
				lp->cover->draw(render, &re1);
				lp->nameFlm->draw(render, 430, 200);
				lp->byFlm->draw(render, 430, 244);
				lp->timeFlm->draw(render, 430, 280);
				if (lp->bestFlm) lp->bestFlm->draw(render, 430, 320);

				menuClassButtonFlame->draw(render, 275, 30);
				(*nowClass)->nameFlm->draw_centered(render, 475, 60);

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
				menuConfigButtonFlame->draw(render, 0, 380);
				menuPlayButtonFlame->draw(render, 0, 520);
				SDL_RenderFillRect(render, &prect);
				re1.y = 200;
				lp->cover->draw(render, &re1);
				lp->nameFlm->draw(render, 430, 200);
				lp->byFlm->draw(render, 430, 244);
				lp->timeFlm->draw(render, 430, 280);
				if (lp->bestFlm) lp->bestFlm->draw(render, 430, 320);

				menuClassButtonFlame->draw(render, 275, 30);
				(*nowClass)->nameFlm->draw_centered(render, 475, 60);

				SDL_RenderPresent(render);

				*turningLevel = 0;
				turningFlag = 0;
				Mix_PlayMusic(lp->sample, -1);
				*nowPanel = lp;
			}
		}
		menuClassButtonFlame->draw(render, 275, 30);
		levelClass->nameFlm->draw_centered(render, 475, 60);
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
		levelClass->nameFlm->draw_centered(render, -125 + *turningClass * 3, 60);
		clp->nameFlm->draw_centered(render, 475 + *turningClass * 3, 60);

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
		menuConfigButtonFlame->draw(render, 0, 380);
		menuPlayButtonFlame->draw(render, 0, 520);

		*turningClass -= getTimeDelta();
		if (*turningClass <= 0) {
			SDL_RenderClear(render);
			titleBGFlame->draw(render, 0, -dt / 10);
			menuBackButtonFlame->draw(render, 0, 0);
			menuUpButtonFlame->draw(render, 0, 100);
			menuDownButtonFlame->draw(render, 0, 170);
			menuConfigButtonFlame->draw(render, 0, 380);
			menuPlayButtonFlame->draw(render, 0, 520);
			SDL_RenderFillRect(render, &prect);

			menuClassButtonFlame->draw(render, 275, 30);
			clp->nameFlm->draw_centered(render, 475, 60);

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
		levelClass->nameFlm->draw_centered(render, 1075 + *turningClass * 3, 60);
		clp->nameFlm->draw_centered(render, 475 + *turningClass * 3, 60);

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
		menuConfigButtonFlame->draw(render, 0, 380);
		menuPlayButtonFlame->draw(render, 0, 520);

		*turningClass += getTimeDelta();
		if (*turningClass >= 0) {
			SDL_RenderClear(render);
			titleBGFlame->draw(render, 0, -dt / 10);
			menuBackButtonFlame->draw(render, 0, 0);
			menuUpButtonFlame->draw(render, 0, 100);
			menuDownButtonFlame->draw(render, 0, 170);
			menuConfigButtonFlame->draw(render, 0, 380);
			menuPlayButtonFlame->draw(render, 0, 520);
			SDL_RenderFillRect(render, &prect);

			menuClassButtonFlame->draw(render, 275, 30);
			clp->nameFlm->draw_centered(render, 475, 60);

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
	drawNumber(render, numberFPSFlame, fps, 750, 580);
}

void drawConfig(SDL_Renderer* render, int keyread) {
	static int x = 0, y = 0;
	static int dt = 0;
	SDL_Rect prect = { 170, 0, 610, 600 };
	dt += getTimeDelta();
	dt %= 3290;
	titleBGFlame->draw(render, 0, -dt / 10);
	SDL_RenderFillRect(render, &prect);
	configBackButtonFlame->draw(render, 0, 0);
	configOKButtonFlame->draw(render, 0, 520);
	text_KeyConf_Flame->draw(render, 200, 40);
	text_DeltaConf_Flame->draw(render, 200, 300);
	text_DeltaValue_Flame->draw(render, 240, 400);
	configSetKeyFlame->draw(render, 170, 100);
	configSetDeltaFlame->draw(render, 170, 380);
	drawNumber(render, numberConfigDeltaFlame, noteDelta, 250 + text_DeltaValue_Flame->getW(), 400);
	for (int i = 0;i < 4;i++)
		if(configKeyFlame[i]) configKeyFlame[i]->draw_centered(render, 280 + 140 * i, 220);
	configSetPointerFlame->draw_centered(render, 490 + noteDelta / 4, 383);

	int al = (int)configFGMask->getAlpha();
	if (keyread >= 0) {
		if (al < 255) {
			al += getTimeDelta();
			if (al > 255) al = 255;
			configFGMask->setAlpha((unsigned char)al);
		}
		configFGMask->draw(render, (x = 140 * keyread -420), (y = 0));
	}
	else if (al > 0) {
		al -= getTimeDelta();
		if (al < 0) al = 0;
		configFGMask->setAlpha(al);
		configFGMask->draw(render, x, y);
	}
	drawNumber(render, numberFPSFlame, fps, 750, 580);
}
