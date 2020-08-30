#define _CRT_SECURE_NO_WARNINGS
// #define SDL_MAIN_HANDLED

#include<SDL.h>
#include<SDL_main.h>
#include<SDL_ttf.h>
#include<SDL_image.h>
#include<SDL_mixer.h>

#include"MU_declaration.h"
#include"MU_flame.h"
#include"MU_loader.h"
#include"MU_path.h"
#include"MU_game.h"

using namespace musnake;


void load();
void unload();
void drawStart(int* bonusM);
void drawPanels(Level** nowPanel, LevelClass** nowClass, int* turningLevel, int* turningClass);
void drawConfig(int keyread);

int main(int argc, char* argv[]) {
	int bonusM = 0;
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

	musnakeWindow = SDL_CreateWindow("Musnake", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 800, 600, SDL_WINDOW_SHOWN);
	musnakeRender = SDL_CreateRenderer(musnakeWindow, -1, 0);
	Mix_OpenAudio(22050, MIX_DEFAULT_FORMAT, 2, 256);
	musnakeState = MU_STATE_RUNNING;

	load();
	nowLevel = levelClasses->levels;
	nowClass = levelClasses;
	SDL_SetRenderDrawBlendMode(musnakeRender, SDL_BLENDMODE_BLEND);
	SDL_SetRenderDrawColor(musnakeRender, 0, 0, 0, 150);

__start:
	loadForTitle();
	Mix_FadeInMusic(titleBGM, -1, 1000);
	while (musnakeState) {
		updateTime();
		SDL_RenderClear(musnakeRender);

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
						unloadForTitle();
						musnakeState = MU_STATE_GAMING;
						while (musnakeState == MU_STATE_GAMING) {
							Mix_HaltMusic();
							thisGame = new Game(bonusInfoLevel);
							thisGame->run();
							delete thisGame;
						}
						loadForTitle();
						Mix_FadeInMusic(titleBGM, -1, 1000);
						bonusM = 0;
						bonus = 0;
					}
					else {
						bonus = 0;
						bonusM = 0;
						unloadForTitle();
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
						unloadForTitle();
						goto __menu;
						break;
					}
				}
			}
		}
		drawStart(&bonusM);
		SDL_RenderPresent(musnakeRender);
	}
	unloadForTitle();
	goto __end;

__menu:
	if (userData["hasTutorial"].empty()) {
__tutorial:
		musnakeState = MU_STATE_GAMING;
		while (musnakeState == MU_STATE_GAMING) {
			if (Mix_PlayingMusic()) Mix_HaltMusic();
			thisGame = new Game(bonusTutorialLevel);
			thisGame->run();
			delete thisGame;
		}
		userData["hasTutorial"] = 1;
		flushUserData();
	}
	loadForMenu();
	loadForLevel(nowLevel);
	loadForClass(nowClass);
	Mix_PlayMusic(nowLevel->sample, -1);
	while (musnakeState) {
		updateTime();
		SDL_RenderClear(musnakeRender);

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
						unloadForLevel(nowLevel);
						unloadForMenu();
						goto __start;
					}
					break;
				case SDLK_TAB:
					unloadForLevel(nowLevel);
					unloadForMenu();
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
						unloadForLevel(nowLevel);
						unloadForMenu();
__game:
						musnakeState = MU_STATE_GAMING;
						while (musnakeState == MU_STATE_GAMING) {
							Mix_HaltMusic();
							thisGame = new Game(nowLevel);
							thisGame->run();
							delete thisGame;
						}
						loadForMenu();
						loadForLevel(nowLevel);
						Mix_PlayMusic(nowLevel->sample, -1);
					}
					break;
				}
				break;
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
						unloadForLevel(nowLevel);
						unloadForMenu();
						goto __start;
					}
					else if( SDL_PointInRect( &point, &UPButton ) ) {
						panelTurning = -200;
					}
					else if( SDL_PointInRect( &point, &DownButton ) ) {
						panelTurning = 200;
					}
					else if (SDL_PointInRect(&point, &ConfigButton)) {
						unloadForLevel(nowLevel);
						unloadForMenu();
						goto __config;
					}
					else if( SDL_PointInRect( &point, &PlayButton ) || SDL_PointInRect( &point, &ImgButton ) ) {
						unloadForMenu();
						unloadForLevel(nowLevel);
						goto __game;
					}
					else if( SDL_PointInRect( &point, &LeftButton ) ) {
						classTurning = -200;
					}
					else if( SDL_PointInRect( &point, &RightButton ) ) {
						classTurning = 200;
					}
				}
				break;
			case SDL_MOUSEWHEEL:
				if (evt.wheel.x < 0) {
					if (!(panelTurning || classTurning)) classTurning = -200;
				}
				else if (evt.wheel.x > 0) {
					if (!(panelTurning || classTurning)) classTurning = 200;
				}
				else if (evt.wheel.y < 0) {
					if (!(panelTurning || classTurning)) panelTurning = 200;
				}
				else if (evt.wheel.y > 0) {
					if (!(panelTurning || classTurning)) panelTurning = -200;
				}
			}
		}
		drawPanels(&nowLevel,&nowClass, &panelTurning, &classTurning);
		SDL_RenderPresent(musnakeRender);
	}
	unloadForLevel(nowLevel);
	unloadForMenu();
	goto __end;

__config:
	loadForConfig();
	Mix_PlayMusic(configBGM, -1);
	settingKey = -1;
	settingDelta = false;

	while (musnakeState) {
		updateTime();
		SDL_RenderClear(musnakeRender);
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
						unloadForConfig();
						goto __menu;
					}
					else
						settingKey = -1;
					break;
				case SDLK_t:
					unloadForConfig();
					goto __tutorial;
				default:
					if (settingKey >= 0) {
						updateUserKeySetting(settingKey, evt.key.keysym.sym);
						settingKey = -1;
					}
				}
			case SDL_KEYUP:
				switch (evt.key.keysym.sym) {
				case SDLK_DELETE:
					emptyUserData();
				}
				break;
			case SDL_MOUSEBUTTONDOWN:
				if (evt.button.button == SDL_BUTTON_LEFT && !(classTurning || panelTurning)) {
					SDL_Point point{ evt.button.x, evt.button.y };
					SDL_Rect BackButton, OKButton, KeyUpButton, KeyDownButton, KeyLeftButton, KeyRightButton, 
						DeltaBarRect, DeltaPinRect, DeleteDataRect, TutorialRect;
					BackButton = { 0,0,150,60 };
					OKButton = { 0,520,150,60 };
					KeyUpButton = { 240, 100, 80, 160 };
					KeyDownButton = { 380, 100, 80, 160 };
					KeyLeftButton = { 520, 100, 80, 160 };
					KeyRightButton = { 660, 100, 80, 160 };
					DeltaBarRect = { 240, 330, 500, 6 };
					DeltaPinRect = { 480 + noteDelta / 4, 318 , 20, 30};
					DeleteDataRect = {400, 394, 200, 60};
					TutorialRect = { 400, 474, 200, 60 };
					if (settingKey == -1 && !settingDelta) {
						if (SDL_PointInRect(&point, &BackButton) || SDL_PointInRect(&point, &OKButton)) {
							userData["settings"]["timeOffset"] = noteDelta;
							flushUserData();
							unloadForConfig();
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
						else if (SDL_PointInRect(&point, &DeleteDataRect)) {
							emptyUserData();
						}
						else if (SDL_PointInRect(&point, &TutorialRect)) {
							unloadForConfig();
							goto __tutorial;
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
		drawConfig(settingKey);
		SDL_RenderPresent(musnakeRender);
	}
	unloadForConfig();

__end:
	if (Mix_PlayingMusic()) Mix_HaltMusic();

	unload();

	TTF_Quit();
	IMG_Quit();
	Mix_Quit();
	SDL_DestroyRenderer(musnakeRender);
	SDL_DestroyWindow(musnakeWindow);
	SDL_Quit();

	return 0;
}

void load() {
	loadFonts();
	loadUserData();
	loadLevels();
	loadForPublic();
}

void unload() {
	unloadForPublic();
	flushUserData();
}

void drawStart(int* bonusM) {
	static int dt = 0;
	dt += getTimeDelta();
	dt %= 3290;
	titleBGFlame->draw(0, -dt / 10);
	titleMusnakeFlame->draw(75, 120);
	titleAuthorFlame->draw(5, 570);
	if(!*bonusM)
		titleEnterButtonFlame->draw_centered(400, 440);
	else {
		switch (*bonusM / 200) {
		case 0:  // 0 - 199 上键的动效触发
			titleEnterButtonFlame->draw_centered(400, 440 - *bonusM / 4);
			*bonusM -= getTimeDelta();
			if (*bonusM < 0) *bonusM = 0;
			break;
		case 1:  // 200 - 399 下键的动效触发
			titleEnterButtonFlame->draw_centered(400, 390 + *bonusM / 4);
			*bonusM -= getTimeDelta();
			if (*bonusM < 200) *bonusM = 0;
			break;
		case 2:  // 400 - 599 左键的动效触发
			titleEnterButtonFlame->draw_centered(500 - *bonusM / 4, 440);
			*bonusM -= getTimeDelta();
			if (*bonusM < 400) *bonusM = 0;
			break;
		case 3:  // 600 - 799 右键的动效触发
			titleEnterButtonFlame->draw_centered(*bonusM / 4 + 250, 440);
			*bonusM -= getTimeDelta();
			if (*bonusM < 600) *bonusM = 0;
			break;
		case 4:  // 800 - 999 A键的动效触发
			titleEnterButtonFlame->draw_centered(400, 440, 1800 - 9 * *bonusM / 5.);
			*bonusM -= getTimeDelta();
			if (*bonusM < 800) *bonusM = 0;
			break;
		case 5:  // 1000 - 1199 B键的动效触发
			titleEnterButtonFlame->draw_centered(400, 440, 9 * *bonusM / 5. - 1800);
			*bonusM -= getTimeDelta();
			if (*bonusM < 1000) *bonusM = 0;
			break;
		}
	}
	drawNumber(numberFPSFlame, fps, 750, 580);
}

void drawPanels(Level** nowPanel, LevelClass** nowClass, int* turningLevel, int* turningClass) {
	static int dt = 0;
	static int turningFlag = 0;
	SDL_Rect prect = { 170, 0, 610, 600 };
	Level* panel = *nowPanel;
	LevelClass* levelClass = *nowClass;
	dt += getTimeDelta();
	dt %= 3290;
	titleBGFlame->draw(0, -dt / 10);
	SDL_RenderFillRect(musnakeRender, &prect);
	if (!*turningClass) {
		menuBackButtonFlame->draw(0, 0);
		menuUpButtonFlame->draw(0, 100);
		menuDownButtonFlame->draw(0, 170);
		menuConfigButtonFlame->draw(0, 380);
		menuPlayButtonFlame->draw(0, 520);
		if (!*turningLevel) {
			SDL_Rect rect = {200, 200, 200, 200};
			panel->cover->draw(&rect);
			panel->nameFlm->draw(430, 200);
			panel->byFlm->draw(430, 244);
			panel->timeFlm->draw(430, 280);
			if (panel->bestFlm) panel->bestFlm->draw(430, 320);
		}
		else if (*turningLevel > 0) {
			Level* lp = panel->next;
			SDL_Rect re1 = { 200, 200 + *turningLevel * 3, 200, 200 }, re2 = { 200, -400 + *turningLevel * 3, 200, 200 };
			if (turningFlag == 0) {
				turningFlag = 1;
				Mix_FadeOutMusic(190);
				loadForLevel(lp);
			}
			lp->cover->draw(&re1);
			lp->nameFlm->draw(430, 200 + *turningLevel * 3);
			lp->byFlm->draw(430, 244 + *turningLevel * 3);
			lp->timeFlm->draw(430, 280 + *turningLevel * 3);
			if (lp->bestFlm) lp->bestFlm->draw(430, 320 + *turningLevel * 3);

			panel->cover->draw(&re2);
			panel->nameFlm->draw(430, -400 + *turningLevel * 3);
			panel->byFlm->draw(430, -350 + *turningLevel * 3);
			panel->timeFlm->draw(430, -320 + *turningLevel * 3);
			if (panel->bestFlm) panel->bestFlm->draw(430, -280 + *turningLevel * 3);

			*turningLevel -= getTimeDelta();
			if (*turningLevel <= 0) {

				if (lp != panel) unloadForLevel(panel);

				*turningLevel = 0;
				turningFlag = 0;
				Mix_PlayMusic(lp->sample, -1);
				*nowPanel = lp;
			}
		}
		else if (*turningLevel < 0) {
			Level* lp = panel->prev;
			SDL_Rect re1 = { 200, 200 + *turningLevel * 3, 200, 200 }, re2 = { 200, 800 + *turningLevel * 3, 200, 200 };
			if (turningFlag == 0) {
				turningFlag = 1;
				Mix_FadeOutMusic(190);
				loadForLevel(lp);
			}
			lp->cover->draw(&re1);
			lp->nameFlm->draw(430, 200 + *turningLevel * 3);
			lp->byFlm->draw(430, 244 + *turningLevel * 3);
			lp->timeFlm->draw(430, 280 + *turningLevel * 3);
			if (lp->bestFlm) lp->bestFlm->draw(430, 320 + *turningLevel * 3);

			panel->cover->draw(&re2);
			panel->nameFlm->draw(430, 800 + *turningLevel * 3);
			panel->byFlm->draw(430, 850 + *turningLevel * 3);
			panel->timeFlm->draw(430, 880 + *turningLevel * 3);
			if (panel->bestFlm) panel->bestFlm->draw(430, 920 + *turningLevel * 3);

			*turningLevel += getTimeDelta();
			if (*turningLevel >= 0) {
				if (lp != panel) unloadForLevel(panel);

				*turningLevel = 0;
				turningFlag = 0;
				Mix_PlayMusic(lp->sample, -1);
				*nowPanel = lp;
			}
		}
		menuClassButtonFlame->draw(275, 30);
		levelClass->nameFlm->draw_centered(475, 60);
	}
	else if (*turningClass > 0) {
		LevelClass* clp = levelClass->next;
		Level* lp = clp->levels;
		SDL_Rect re1 = { 200 + *turningClass * 3, 200, 200, 200 }, re2 = { -400 + *turningClass * 3,200, 200, 200 };
		if (turningFlag == 0) {
			turningFlag = 1;
			loadForClass(clp);
			Mix_FadeOutMusic(190);
			loadForLevel(lp);
		}
		menuClassButtonFlame->draw(275 + *turningClass * 3, 30);
		menuClassButtonFlame->draw(-325 + *turningClass * 3, 30);
		levelClass->nameFlm->draw_centered(-125 + *turningClass * 3, 60);
		clp->nameFlm->draw_centered(475 + *turningClass * 3, 60);

		lp->cover->draw(&re1);
		lp->nameFlm->draw(430 + *turningClass * 3, 200);
		lp->byFlm->draw(430 + *turningClass * 3, 244);
		lp->timeFlm->draw(430 + *turningClass * 3, 280);
		if (lp->bestFlm) lp->bestFlm->draw(430 + *turningClass * 3, 320);

		panel->cover->draw(&re2);
		panel->nameFlm->draw(-170 + *turningClass * 3, 200);
		panel->byFlm->draw(-170 + *turningClass * 3, 244);
		panel->timeFlm->draw(-170 + *turningClass * 3, 280);
		if (panel->bestFlm) panel->bestFlm->draw(-170 + *turningClass * 3, 320);

		titleLBGFlame->draw(0, -dt / 10);
		titleRBGFlame->draw(780, -dt / 10);
		menuBackButtonFlame->draw(0, 0);
		menuUpButtonFlame->draw(0, 100);
		menuDownButtonFlame->draw(0, 170);
		menuConfigButtonFlame->draw(0, 380);
		menuPlayButtonFlame->draw(0, 520);

		*turningClass -= getTimeDelta();
		if (*turningClass <= 0) {
			if (lp != panel) unloadForLevel(panel);
			if (clp != *nowClass)unloadForClass(*nowClass);
			*turningClass = 0;
			turningFlag = 0;
			Mix_PlayMusic(lp->sample, -1);
			*nowClass = clp;
			*nowPanel = lp;
		}
	}
	else if (*turningClass < 0) {
		LevelClass* clp = levelClass->prev;
		Level* lp = clp->levels;
		SDL_Rect re1 = { 200 + *turningClass * 3, 200, 200, 200 }, re2 = { 800 + *turningClass * 3, 200, 200, 200 };
		if (turningFlag == 0) {
			turningFlag = 1;
			loadForClass(clp);
			Mix_FadeOutMusic(190);
			loadForLevel(lp);
		}
		menuClassButtonFlame->draw(275 + *turningClass * 3, 30);
		menuClassButtonFlame->draw(875 + *turningClass * 3, 30);
		levelClass->nameFlm->draw_centered(1075 + *turningClass * 3, 60);
		clp->nameFlm->draw_centered(475 + *turningClass * 3, 60);

		lp->cover->draw(&re1);
		lp->nameFlm->draw(430 + *turningClass * 3, 200);
		lp->byFlm->draw(430 + *turningClass * 3, 244);
		lp->timeFlm->draw(430 + *turningClass * 3, 280);
		if (lp->bestFlm) lp->bestFlm->draw(430 + +*turningClass * 3, 320);

		panel->cover->draw(&re2);
		panel->nameFlm->draw(1030 + *turningClass * 3, 200);
		panel->byFlm->draw(1030 + *turningClass * 3, 244);
		panel->timeFlm->draw(1030 + *turningClass * 3, 280);
		if (panel->bestFlm) panel->bestFlm->draw(1030 + *turningClass * 3, 320);

		titleLBGFlame->draw(0, -dt / 10);
		titleRBGFlame->draw(780, -dt / 10);
		menuBackButtonFlame->draw(0, 0);
		menuUpButtonFlame->draw(0, 100);
		menuDownButtonFlame->draw(0, 170);
		menuConfigButtonFlame->draw(0, 380);
		menuPlayButtonFlame->draw(0, 520);

		*turningClass += getTimeDelta();
		if (*turningClass >= 0) {
			if (lp != panel) unloadForLevel(panel);
			if (clp != *nowClass)unloadForClass(*nowClass);
			*turningClass = 0;
			turningFlag = 0;
			Mix_PlayMusic(lp->sample, -1);
			*nowClass = clp;
			*nowPanel = lp;
		}
	}
	drawNumber(numberFPSFlame, fps, 750, 580);
}

void drawConfig(int keyread) {
	static int x = 0, y = 0;
	static int dt = 0;
	SDL_Rect prect = { 170, 0, 610, 600 };
	dt += getTimeDelta();
	dt %= 3290;
	titleBGFlame->draw(0, -dt / 10);
	SDL_RenderFillRect(musnakeRender, &prect);
	configBackButtonFlame->draw(0, 0);
	configOKButtonFlame->draw(0, 520);

	text_KeyConf_Flame->draw(200, 40);
	configSetKeyFlame->draw(170, 100);
	for (int i = 0;i < 4;i++)
		if(configKeyFlame[i]) configKeyFlame[i]->draw_centered(280 + 140 * i, 220);

	text_DeltaConf_Flame->draw(200,270);
	text_DeltaValue_Flame->draw(240, 350);
	configSetDeltaFlame->draw(170, 330);
	drawNumber(numberConfigDeltaFlame, noteDelta, 250 + text_DeltaValue_Flame->getW(), 350);
	configSetPointerFlame->draw_centered(490 + noteDelta / 4, 333);

	text_DeleteData_Flame->draw(200, 400);
	configDeleteButtonFlame->draw(400, 394);

	text_Tutorial_Flame->draw(200, 480);
	configTutorialButtonFlame->draw(400, 474);

	int al = (int)configFGMask->getAlpha();
	if (keyread >= 0) {
		if (al < 255) {
			al += getTimeDelta();
			if (al > 255) al = 255;
			configFGMask->setAlpha((unsigned char)al);
		}
		configFGMask->draw((x = 140 * keyread -420), (y = 0));
	}
	else if (al > 0) {
		al -= getTimeDelta();
		if (al < 0) al = 0;
		configFGMask->setAlpha(al);
		configFGMask->draw(x, y);
	}
	drawNumber(numberFPSFlame, fps, 750, 580);
}
