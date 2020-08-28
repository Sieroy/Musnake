#pragma once
#pragma warning(disable : 4996)

#include <fstream>

#include "json/json.h"
#include <SDL.h>
#include <SDL_ttf.h>
#include <SDL_image.h>
#include <SDL_mixer.h>

#include "MU_declaration.h"
#include "MU_path.h"
#include "MU_time.h"
#include "MU_flame.h"
#include "MU_gamestruct.h"
#include "MU_game.h"

inline void musnake::loadUserData() {
	Json::Reader reader;
	char tmpStr[256];
	catPath(tmpStr, (char*)"data/save.mu");
	std::ifstream ifs(tmpStr, std::ios::binary);
	reader.parse(ifs, userData);
	for(int i=0;i<4;i++)
		if (!userData["settings"]["key"][keyName[i]].empty()) {
			updateUserKeySetting(i, userData["settings"]["key"][keyName[i]].asInt());
			// 因为不想把下面的代码再抄过来了，就直接这样写吧
		}
	noteDelta = userData["settings"]["timeOffset"].asInt();
}

inline void musnake::updateUserScore(char* levelId, int rank, int score, int length) {
	if (rank >= 0) userData["record"][levelId]["rank"] = rank;
	if (score >= 0) userData["record"][levelId]["score"] = score;
	if (length) userData["record"][levelId]["length"] = length;
	if (userData["record"][levelId]["times"].empty())
		userData["record"][levelId]["times"] = 1;
	else
		userData["record"][levelId]["times"] = userData["record"][levelId]["times"].asInt() + 1;
}

inline void musnake::updateUserKeySetting(int keyType, SDL_Keycode keyCode){
	SDL_Color tmpColor = { 255,255,255,255 };
	Flame* fp;
	if (keyCode == SDLK_SPACE) 
		fp = loadFlameForText(configKeyFont, (char*)"SPACE", &tmpColor);
	else if ((keyCode >= SDLK_EXCLAIM && keyCode <= SDLK_AT)
		|| (keyCode >= SDLK_LEFTBRACKET && keyCode <= SDLK_BACKQUOTE)) {

		char s[2] = { (char)keyCode, 0 };
		fp = loadFlameForText(configKeyFont, s, &tmpColor);
	}
	else if (keyCode >= SDLK_a && keyCode <= SDLK_z) {
		char s[2] = { keyCode & 223, 0 };
		fp = loadFlameForText(configKeyFont, s, &tmpColor);
	}
	else if (keyCode == SDLK_KP_DIVIDE)
		fp = loadFlameForText(configKeyFont, (char*)"(/)", &tmpColor);
	else if (keyCode == SDLK_KP_MULTIPLY)
		fp = loadFlameForText(configKeyFont, (char*)"(*)", &tmpColor);
	else if (keyCode == SDLK_KP_MINUS)
		fp = loadFlameForText(configKeyFont, (char*)"(-)", &tmpColor);
	else if (keyCode == SDLK_KP_PLUS)
		fp = loadFlameForText(configKeyFont, (char*)"(+)", &tmpColor);
	else if (keyCode == SDLK_KP_0)
		fp = loadFlameForText(configKeyFont, (char*)"(0)", &tmpColor);
	else if (keyCode == SDLK_KP_PERIOD)
		fp = loadFlameForText(configKeyFont, (char*)"(.)", &tmpColor);
	else if (keyCode >= SDLK_KP_1 && keyCode <= SDLK_KP_9) {
		char s[4] = { '(', (char)(keyCode - SDLK_KP_1 + 49), ')', 0 };
		fp = loadFlameForText(configKeyFont, s, &tmpColor);
	}
	else {
		return;
	}
	if (configKeyFlame[keyType]) delete configKeyFlame[keyType];
	configKeyFlame[keyType] = fp;
	musnakeKey[keyType] = keyCode;
	userData["settings"]["key"][keyName[keyType]] = keyCode;
	for (int i = 0;i < 4;i++) {
		if (i == keyType) continue;
		if (musnakeKey[i] == keyCode) {
			delete configKeyFlame[i];
			musnakeKey[i] = 0;
			configKeyFlame[i] = nullptr;
			userData["settings"]["key"][keyName[keyType]] = 0;
		}
	}
}

inline void musnake::emptyUserData() {
	userData["record"].clear();
	userData["settings"]["timeOffset"] = noteDelta = 0;
	userData["settings"]["key"].clear();
	for (int i = 0;i < 4;i++) {
		musnakeKey[i] = 0;
		if (configKeyFlame[i]) {
			delete configKeyFlame[i];
			configKeyFlame[i] = nullptr;
		}
	}
	flushUserData();
}

inline void musnake::flushUserData() {
	Json::FastWriter writer;
	char tmpStr[256];
	catPath(tmpStr, (char*)"data/save.mu");
	std::ofstream ofs(tmpStr, std::ios::out);
	ofs << writer.write(userData);
	ofs.close();
}


inline void musnake::loadFonts() {
	char tmpPath[256];

	catPath(tmpPath, (char*)"font/SHOWG.TTF");
	titleMusnakeFont = TTF_OpenFont(tmpPath, 144);

	gamePauseTitleFont = TTF_OpenFont(tmpPath, 80);

	numberTotalFont =
	gameWinScorelabelFont = TTF_OpenFont(tmpPath, 50);

	numberHitsFont =
	numberScoreFont = TTF_OpenFont(tmpPath, 40);

	menuSongBestDataFont = TTF_OpenFont(tmpPath, 20);

	gameCombolabelFont = TTF_OpenFont(tmpPath, 14);

	catPath(tmpPath, (char*)"font/msyhbd.ttc");
	titleAuthorFont =
	menuSongbySmallFont =
	configKeyFont =
	numberFPSFont = TTF_OpenFont(tmpPath, 16);

	menuSongtimeFont =
	menuSongbyFont =
	menuSongBestTextFont =
	configLabelFont =
	gameScorelabelFont =
	numberConfigDeltaFont = TTF_OpenFont(tmpPath, 20);

	menuSongnameSmallFont = TTF_OpenFont(tmpPath, 24);

	menuSongnameFont =
	configSettingFont =
	gameToastFont =
	gameWinScorelabelFont =
	gameWinLengthlabelFont =
	gameWinNewBestFont =
	menuClassNameFont = TTF_OpenFont(tmpPath, 30);
}


void musnake::loadForPublic() {
	char tmpStr[2];
	SDL_Color tmpColor = { 255,255,255,255 };

	tmpStr[1] = 0;
	for (char i = 0;i < 10;i++) {
		tmpStr[0] = i + '0';
		numberFPSFlame[i] = loadFlameForText(numberFPSFont, tmpStr, &tmpColor);
	}

	titleBGFlame = loadFlameFromFile((char*)"image/bg_main.png");
	menuBackButtonFlame =
	configBackButtonFlame =
	gameOverBackButtonFlame = loadFlameFromFile((char*)"image/button_back.png");
	configOKButtonFlame =
	gameOverOKButtonFlame = loadFlameFromFile((char*)"image/button_ok.png");
}

void musnake::unloadForPublic() {
	for (char i = 0;i < 10;i++) delete numberFPSFlame[i];
	delete titleBGFlame, gameOverBackButtonFlame, gameOverOKButtonFlame;
}

void musnake::loadForTitle() {
	if (!loadingList[MU_LOAD_TITLE]) {
		char tmpPath[256];
		SDL_Color tmpColor = { 255,255,255,255 };

		titleEnterButtonFlame = loadFlameFromFile((char*)"image/button_enter.png");

		titleMusnakeFlame = loadFlameForText(titleMusnakeFont, (char*)"MUSNAKE", &tmpColor);
		titleAuthorFlame = loadFlameForText(titleAuthorFont, (char*)"By Sieroy & StdCat", &tmpColor);

		catPath(tmpPath, (char*)"sound/bgm_title.mp3");
		titleBGM = Mix_LoadMUS(tmpPath);

		loadingList[MU_LOAD_TITLE] = true;
	}
}

void musnake::unloadForTitle() {
	if (loadingList[MU_LOAD_TITLE]) {
		delete titleEnterButtonFlame;
		delete titleMusnakeFlame;
		delete titleAuthorFlame;
		Mix_FreeMusic(titleBGM);

		titleEnterButtonFlame = titleMusnakeFlame = titleAuthorFlame = nullptr;
		titleBGM = nullptr;

		loadingList[MU_LOAD_TITLE] = false;
	}
}

void musnake::loadForMenu() {
	if (!loadingList[MU_LOAD_MENU]) {
		char tmpPath[256];
		SDL_Surface* picSurf;
		SDL_Rect tmpRect = { 0,0,170,1000 };

		// 初始页面背景以及修边的载入
		catPath(tmpPath, (char*)"image/bg_main.png");
		picSurf = IMG_Load(tmpPath);
		titleLBGFlame = new Flame(picSurf, &tmpRect, -1);
		tmpRect.x = 780;
		tmpRect.w = 20;
		titleRBGFlame = new Flame(picSurf, &tmpRect, -1);
		SDL_FreeSurface(picSurf);

		menuConfigButtonFlame = loadFlameFromFile((char*)"image/button_config.png");
		menuUpButtonFlame = loadFlameFromFile((char*)"image/button_up.png");
		menuDownButtonFlame = loadFlameFromFile((char*)"image/button_down.png");
		menuPlayButtonFlame = loadFlameFromFile((char*)"image/button_play.png");
		menuClassButtonFlame = loadFlameFromFile((char*)"image/button_class.png");

		loadingList[MU_LOAD_MENU] = true;
	}
}

void musnake::unloadForMenu() {
	if (loadingList[MU_LOAD_MENU]) {
		delete titleLBGFlame;
		delete titleRBGFlame;
		delete menuConfigButtonFlame;
		delete menuUpButtonFlame;
		delete menuDownButtonFlame;
		delete menuPlayButtonFlame;
		delete menuClassButtonFlame;

		titleLBGFlame =
		titleRBGFlame =
		menuConfigButtonFlame =
		menuUpButtonFlame =
		menuDownButtonFlame =
		menuPlayButtonFlame =
		menuClassButtonFlame = nullptr;

		loadingList[MU_LOAD_MENU] = false;
	}
}

void musnake::loadForConfig() {
	if (!loadingList[MU_LOAD_CONFIG]) {
		char tmpPath[256];
		char tmpStr[2];
		SDL_Color tmpColor = { 255,255,255,255 };

		configSetKeyFlame = loadFlameFromFile((char*)"image/button_settingkey.png");
		configSetDeltaFlame = loadFlameFromFile((char*)"image/button_settingbar.png");
		configSetPointerFlame = loadFlameFromFile((char*)"image/button_settingpin.png");
		configDeleteButtonFlame = loadFlameFromFile((char*)"image/button_delete.png");
		configTutorialButtonFlame = loadFlameFromFile((char*)"image/button_tutorial.png");
		configFGMask = loadFlameFromFile((char*)"image/mask_configkey.png");
		configFGMask->setAlpha(0);

		text_KeyConf_Flame = loadFlameForUTF8(configSettingFont, (char*)"\xe6\xb8\xb8\xe6\x88\x8f\xe6\x8c\x89\xe9\x94\xae\xe8\xae\xbe\xe7\xbd\xae", &tmpColor);
		text_DeltaConf_Flame = loadFlameForUTF8(configSettingFont, (char*)"\xe9\x9f\xb3\xe4\xb9\x90\xe5\x81\x8f\xe7\xa7\xbb\xe8\xae\xbe\xe7\xbd\xae", &tmpColor);
		text_DeltaValue_Flame = loadFlameForUTF8(configLabelFont, (char*)"\xe5\x81\x8f\xe7\xa7\xbb\xe5\x80\xbc: ", &tmpColor);
		text_DeleteData_Flame = loadFlameForUTF8(configSettingFont, (char*)"\xe5\x88\xa0\xe9\x99\xa4\xe7\x94\xa8\xe6\x88\xb7\xe6\x95\xb0\xe6\x8d\xae", &tmpColor);
		text_Tutorial_Flame = loadFlameForUTF8(configSettingFont, (char*)"\xe9\x87\x8d\xe6\xb8\xa9\xe6\x96\xb0\xe6\x89\x8b\xe6\x95\x99\xe5\xad\xa6", &tmpColor);

		tmpStr[1] = 0;
		for (char i = 0;i < 10;i++) {
			tmpStr[0] = i + '0';
			numberConfigDeltaFlame[i] = loadFlameForText(numberConfigDeltaFont, tmpStr, &tmpColor);
		}
		numberConfigDeltaFlame[10] = loadFlameForText(numberConfigDeltaFont, (char*)"-", &tmpColor);

		catPath(tmpPath, (char*)"sound/bgm_config.mp3");
		configBGM = Mix_LoadMUS(tmpPath);

		loadingList[MU_LOAD_CONFIG] = true;
	}
}

void musnake::unloadForConfig() {
	if (loadingList[MU_LOAD_CONFIG]) {
		delete configSetKeyFlame;
		delete configSetDeltaFlame;
		delete configSetPointerFlame;
		delete configDeleteButtonFlame;
		delete configTutorialButtonFlame;
		delete configFGMask;
		delete text_KeyConf_Flame;
		delete text_DeltaConf_Flame;
		delete text_DeltaValue_Flame;
		delete text_DeleteData_Flame;
		delete text_Tutorial_Flame;
		delete numberConfigDeltaFlame[0];
		delete numberConfigDeltaFlame[1];
		delete numberConfigDeltaFlame[2];
		delete numberConfigDeltaFlame[3];
		delete numberConfigDeltaFlame[4];
		delete numberConfigDeltaFlame[5];
		delete numberConfigDeltaFlame[6];
		delete numberConfigDeltaFlame[7];
		delete numberConfigDeltaFlame[8];
		delete numberConfigDeltaFlame[9];
		delete numberConfigDeltaFlame[10];

		configSetKeyFlame =
		configSetDeltaFlame =
		configSetPointerFlame =
		configDeleteButtonFlame =
		configTutorialButtonFlame =
		configFGMask =
		text_KeyConf_Flame =
		text_DeltaConf_Flame =
		text_DeltaValue_Flame =
		text_DeleteData_Flame =
		text_Tutorial_Flame =
		numberConfigDeltaFlame[0] =
		numberConfigDeltaFlame[1] =
		numberConfigDeltaFlame[2] =
		numberConfigDeltaFlame[3] =
		numberConfigDeltaFlame[4] =
		numberConfigDeltaFlame[5] =
		numberConfigDeltaFlame[6] =
		numberConfigDeltaFlame[7] =
		numberConfigDeltaFlame[8] =
		numberConfigDeltaFlame[9] =
		numberConfigDeltaFlame[10] = nullptr;

		loadingList[MU_LOAD_CONFIG] = false;
	}
}

void musnake::loadForGame() {
	if (!loadingList[MU_LOAD_GAME]) {
		char tmpPath[256];
		char tmpStr[2];
		SDL_Surface* picSurf;
		SDL_Color tmpColor = { 255,255,255,255 };

		catPath(tmpPath, (char*)"sound/bgm_gamelose.mp3");
		gameloseBGM = Mix_LoadMUS(tmpPath);
		catPath(tmpPath, (char*)"sound/bgm_gamewin.mp3");
		gamewinBGM = Mix_LoadMUS(tmpPath);

		gamePauseResumeButtonFlame[0] = loadFlameFromFile((char*)"image/button_resume_nc.png");
		gamePauseResumeButtonFlame[1] = loadFlameFromFile((char*)"image/button_resume_c.png");
		gamePauseRetryButtonFlame[0] = loadFlameFromFile((char*)"image/button_retry_nc.png");
		gamePauseRetryButtonFlame[1] = loadFlameFromFile((char*)"image/button_retry_c.png");
		gamePauseBackButtonFlame[0] = loadFlameFromFile((char*)"image/button_back_nc.png");
		gamePauseBackButtonFlame[1] = loadFlameFromFile((char*)"image/button_back_c.png");
		gameOverRetryButtonFlame = loadFlameFromFile((char*)"image/button_retry.png");
		notesignFlame[0] = loadFlameFromFile((char*)"image/notesign.png");
		notesignFlame[1] = loadFlameFromFile((char*)"image/notesign_fever.png");
		notesignFlame[2] = loadFlameFromFile((char*)"image/notesign_edge.png");

		catPath(tmpPath, (char*)"image/hp.png");
		picSurf = IMG_Load(tmpPath);
		for (int i = 0;i < 6;i++) {
			SDL_Rect srect = { i * 20, 0, 20, 20 };

			hpFlame[i / 2][i & 1] = new Flame(picSurf, &srect, -1);
			hpFlame[i / 2][i & 1]->setNext(nullptr);
		}
		SDL_FreeSurface(picSurf);

		foodFlame[0] = loadFlameFromFile((char*)"image/food_0.png");
		foodFlame[1] = loadFlameFromFile((char*)"image/food_1.png");
		foodFlame[2] = loadFlameFromFile((char*)"image/food_2.png");

		// 读取食物指示图
		foodPointerFlame[0][0] = loadFlameFromFile((char*)"image/foodpointer_0.png");
		foodPointerFlame[1][0] = loadFlameFromFile((char*)"image/foodpointer_1.png");
		foodPointerFlame[2][0] = loadFlameFromFile((char*)"image/foodpointer_2.png");
		foodPointerFlame[0][1] = loadFlameFromFile((char*)"image/foodpointer_0L.png");
		foodPointerFlame[1][1] = loadFlameFromFile((char*)"image/foodpointer_1L.png");
		foodPointerFlame[2][1] = loadFlameFromFile((char*)"image/foodpointer_2L.png");

		// 读取地格图像
		gridFlame = loadFlameFromFile((char*)"image/grid_earth.png");
		gridWaterFlame = loadFlameFromFile((char*)"image/grid_water.png");
		gridDarkFlame = loadFlameFromFile((char*)"image/grid_none.png");
		gridBlockFlame = loadFlameFromFile((char*)"image/grid_block.png");

		catPath(tmpPath, (char*)"image/snake_0_head.png");
		picSurf = IMG_Load(tmpPath);
		for (int i = 0;i < 16;i++) {
			Flame* flames[8];
			for (int j = 0;j < 8;j++) {
				SDL_Rect srect = { j * 20, i * 20, 20, 20 };

				flames[j] = new Flame(picSurf, &srect, 9);
				flames[j]->setGroupId(MU_SNAKE_FLAME_HEAD_0toUP + i);
				if (j)flames[j - 1]->setNext(flames[j]);
			}
			snakeFlame[MU_SNAKE_FLAME_HEAD_0toUP + i] = flames[0];
		}
		SDL_FreeSurface(picSurf);

		catPath(tmpPath, (char*)"image/snake_0_tail.png");
		picSurf = IMG_Load(tmpPath);
		for (int i = 0;i < 20;i++) {
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

		catPath(tmpPath, (char*)"image/snake_0_body.png");
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

		gamePauseBGMask = loadFlameFromFile((char*)"image/mask_gamepause.png");
		gamewinBGFlame = loadFlameFromFile((char*)"image/bg_gamewin.png");
		gameloseBGFlame = loadFlameFromFile((char*)"image/bg_gamelose.png");
		gameloseFGFlame = loadFlameFromFile((char*)"image/fg_gamelose.png");
		rankFlame[0] = loadFlameFromFile((char*)"image/fg_gamewin_SSS.png");
		rankFlame[1] = loadFlameFromFile((char*)"image/fg_gamewin_SS.png");
		rankFlame[2] = loadFlameFromFile((char*)"image/fg_gamewin_S.png");
		rankFlame[3] = loadFlameFromFile((char*)"image/fg_gamewin_A.png");
		rankFlame[4] = loadFlameFromFile((char*)"image/fg_gamewin_B.png");
		rankFlame[5] = loadFlameFromFile((char*)"image/fg_gamewin_C.png");
		rankFlame[6] = loadFlameFromFile((char*)"image/fg_gamewin_D.png");
		
		text_Score_Flame = loadFlameForUTF8(gameScorelabelFont, (char*)"\xe5\xbe\x97\xe5\x88\x86", &tmpColor);
		text_Hits_Flame = loadFlameForText(gameCombolabelFont, (char*)"Hits!", &tmpColor);
		text_TotalScore_Flame = loadFlameForUTF8(gameWinScorelabelFont, (char*)"\xe5\x88\x86\xe6\x95\xb0", &tmpColor);
		text_TotalLength_Flame = loadFlameForUTF8(gameWinLengthlabelFont, (char*)"\xe9\x95\xbf\xe5\xba\xa6", &tmpColor);
		gamePauseTitleFlame = loadFlameForText(gamePauseTitleFont, (char*)"- PAUSE -", &tmpColor);
		gamewinNewBestFlame = loadFlameForUTF8(gameWinNewBestFont, (char*)"\xe6\x96\xb0\xe7\xba\xaa\xe5\xbd\x95\xef\xbc\x81\xef\xbc\x81", &tmpColor);

		tmpStr[1] = 0;
		for (char i = 0;i < 10;i++) {
			tmpStr[0] = i + '0';
			numberHitsFlame[i] = loadFlameForText(numberHitsFont, tmpStr, &tmpColor);
			numberScoreFlame[i] = loadFlameForText(numberScoreFont, tmpStr, &tmpColor);
			numberTotalFlame[i] = loadFlameForText(numberTotalFont, tmpStr, &tmpColor);
		}
		loadingList[MU_LOAD_GAME] = true;
	}
}

void musnake::unloadForGame() {
	if (loadingList[MU_LOAD_GAME]) {
		Mix_FreeMusic(gameloseBGM);
		delete hpFlame[0][0];
		delete hpFlame[0][1];
		delete hpFlame[1][0];
		delete hpFlame[1][1];
		delete hpFlame[2][0];
		delete hpFlame[2][1];
		delete notesignFlame[0];
		delete notesignFlame[1];
		delete notesignFlame[2];
		delete foodPointerFlame[0][0];
		delete foodPointerFlame[1][0];
		delete foodPointerFlame[2][0];
		delete foodPointerFlame[0][1];
		delete foodPointerFlame[1][1];
		delete foodPointerFlame[2][1];
		delete text_Score_Flame;
		delete text_Hits_Flame;
		delete foodFlame[0];
		delete foodFlame[1];
		delete foodFlame[2];
		delete gridFlame;
		delete gridWaterFlame;
		delete gridDarkFlame;
		delete gridBlockFlame;
		delete gamePauseResumeButtonFlame[0];
		delete gamePauseResumeButtonFlame[1];
		delete gamePauseRetryButtonFlame[0];
		delete gamePauseRetryButtonFlame[1];
		delete gamePauseBackButtonFlame[0];
		delete gamePauseBackButtonFlame[1];
		delete gamePauseTitleFlame;
		delete gamePauseBGMask;
		delete gameOverRetryButtonFlame;
		delete gamewinBGFlame;
		delete gamewinNewBestFlame;
		delete gameloseBGFlame;
		delete gameloseFGFlame;
		delete rankFlame[0];
		delete rankFlame[1];
		delete rankFlame[2];
		delete rankFlame[3];
		delete rankFlame[4];
		delete rankFlame[5];
		delete rankFlame[6];
		delete text_TotalScore_Flame;
		delete text_TotalLength_Flame;

		gamewinBGM = gameloseBGM = nullptr;

		hpFlame[0][0] =
			hpFlame[0][1] =
			hpFlame[1][0] =
			hpFlame[1][1] =
			hpFlame[2][0] =
			hpFlame[2][1] =
			notesignFlame[0] =
			notesignFlame[1] =
			notesignFlame[2] =
			foodPointerFlame[0][0] =
			foodPointerFlame[1][0] =
			foodPointerFlame[2][0] =
			foodPointerFlame[0][1] =
			foodPointerFlame[1][1] =
			foodPointerFlame[2][1] =
			text_Score_Flame =
			text_Hits_Flame =
			foodFlame[0] =
			foodFlame[1] =
			foodFlame[2] =
			gridFlame =
			gridWaterFlame =
			gridDarkFlame =
			gridBlockFlame =
			gamePauseResumeButtonFlame[0] =
			gamePauseResumeButtonFlame[1] =
			gamePauseRetryButtonFlame[0] =
			gamePauseRetryButtonFlame[1] =
			gamePauseBackButtonFlame[0] =
			gamePauseBackButtonFlame[1] =
			gamePauseTitleFlame =
			gamePauseBGMask =
			gameOverRetryButtonFlame =
			gamewinBGFlame =
			gamewinNewBestFlame =
			gameloseBGFlame =
			gameloseFGFlame =
			rankFlame[0] =
			rankFlame[1] =
			rankFlame[2] =
			rankFlame[3] =
			rankFlame[4] =
			rankFlame[5] =
			rankFlame[6] =
			text_TotalScore_Flame =
			text_TotalLength_Flame = nullptr;

		for (char i = 0;i < 10;i++) {
			delete numberHitsFlame[i];
			delete numberScoreFlame[i];
			delete numberTotalFlame[i];
			numberHitsFlame[i], numberScoreFlame[i], numberTotalFlame[i] = nullptr;
		}

		for (char i = MU_SNAKE_FLAME_HEAD_0toUP;i < 50;i++) {
			delete snakeFlame[i];
			snakeFlame[i] = nullptr;
		}

		for (char i = 0;i < MU_SNAKE_FLAME_HEAD_0toUP;i++) {
			delete snakeFlame[i];
			snakeFlame[i] = nullptr;
		}

		loadingList[MU_LOAD_GAME] = false;
	}
}

void musnake::loadForLevel(Level* lp) {
	char tmpStr[256];
	char ss[64];
	SDL_Color tmpColor = { 255, 255, 255, 255 };

	if (!userData["record"][lp->id].empty()) updateLevelBestFlame(lp);
	
	if (!(lp->sample)) {
		SDL_strlcpy(ss, "level/", 48);
		SDL_strlcat(ss, lp->id, 48);
		SDL_strlcat(ss, "/sample.mp3", 48);
		catPath(tmpStr, ss);
		lp->sample = Mix_LoadMUS(tmpStr);
	}
	if (!(lp->cover)) {
		SDL_strlcpy(ss, "level/", 48);
		SDL_strlcat(ss, lp->id, 48);
		SDL_strlcat(ss, "/cover.png", 48);
		lp->cover = loadFlameFromFile(ss);
	}
}

void musnake::unloadForLevel(Level* lp) {
	delete lp->bestFlm;
	delete lp->cover;
	Mix_FreeMusic(lp->sample);

	lp->bestFlm = lp->cover = nullptr;
	lp->sample = nullptr;
}


inline void musnake::parseRankStr(char* str, int rankv) {
	switch (rankv / 10) {
	case 10:
		SDL_strlcpy(str, "SSS", 4);
		break;
	case 9:
		if (rankv >= 95)
			SDL_strlcpy(str, "SS", 3);
		else
			SDL_strlcpy(str, "S", 3);
		break;
	case 8:
		SDL_strlcpy(str, "A", 3);
		break;
	case 7:
		SDL_strlcpy(str, "B", 3);
		break;
	case 6:
		SDL_strlcpy(str, "C", 3);
		break;
	default:
		SDL_strlcpy(str, "D", 3);
	}
}

void musnake::updateLevelBestFlame(Level* lp) {
	SDL_Surface* textSurf;
	SDL_Surface* tmpSurf;
	SDL_Color tmpColor = { 255, 255, 255, 255 };
	SDL_Rect rect;

	char tmpStr[32];

#if SDL_BYTEORDER == SDL_BIG_ENDIAN
	tmpSurf = SDL_CreateRGBSurface(0, 300, 60, 32, 0xff000000, 0x00ff0000, 0x0000ff00, 0x000000ff);
#else
	tmpSurf = SDL_CreateRGBSurface(0, 300, 60, 32, 0x000000ff, 0x0000ff00, 0x00ff0000, 0xff000000);
#endif

	textSurf = TTF_RenderUTF8_Blended(menuSongBestTextFont, "\xe6\x9c\x80\xe4\xbd\xb3\xe8\xae\xb0\xe5\xbd\x95\xef\xbc\x9a", tmpColor);
	rect.x = 0;  rect.y = 0;  rect.w = textSurf->w; rect.h = textSurf->h;
	SDL_BlitSurface(textSurf, NULL, tmpSurf, &rect);
	SDL_FreeSurface(textSurf);

	if (lp->interval <= 0) {
		parseRankStr(tmpStr, userData["record"][lp->id]["rank"].asInt());
		textSurf = TTF_RenderText_Blended(menuSongBestDataFont, tmpStr, tmpColor);
		rect.x = 20;  rect.y = 30;  rect.w = textSurf->w; rect.h = textSurf->h;
		SDL_BlitSurface(textSurf, NULL, tmpSurf, &rect);
		SDL_FreeSurface(textSurf);

		int2str(tmpStr, userData["record"][lp->id]["score"].asInt());
		SDL_strlcat(tmpStr, " pt", 32);
		textSurf = TTF_RenderText_Blended(menuSongBestDataFont, tmpStr, tmpColor);
		rect.x = 84;  rect.y = 30;  rect.w = textSurf->w; rect.h = textSurf->h;
		SDL_BlitSurface(textSurf, NULL, tmpSurf, &rect);
		SDL_FreeSurface(textSurf);
	}

	int2str(tmpStr, userData["record"][lp->id]["length"].asInt());
	SDL_strlcat(tmpStr, " m", 32);
	textSurf = TTF_RenderText_Blended(menuSongBestDataFont, tmpStr, tmpColor);
	rect.x = lp->interval > 0 ? 20 : 200;  rect.y = 30;  rect.w = textSurf->w; rect.h = textSurf->h;
	SDL_BlitSurface(textSurf, NULL, tmpSurf, &rect);
	SDL_FreeSurface(textSurf);

	if (lp->bestFlm) delete lp->bestFlm;
	lp->bestFlm = new Flame(tmpSurf, NULL, -1);
}

void musnake::loadLevels() {
	SDL_Color tmpColor = { 255, 255, 255, 255 };
	Json::Reader reader;
	Json::Value levelRoot;
	char tmpStr[256];
	catPath(tmpStr, (char*)"data/list.mu");
	std::ifstream ifs(tmpStr, std::ios::binary);

	reader.parse(ifs, levelRoot);

	int classCount = levelRoot["classes"].size();
	LevelClass* clp = nullptr;
	for (int i = 0; i < classCount; i++) {
		if (!clp)
			clp = levelClasses = new LevelClass;
		else {
			clp->next = new LevelClass;
			clp->next->prev = clp;
			clp = clp->next;
		}
		*(tmpStr + levelRoot["classes"][i]["name"].asString().copy(tmpStr, 31, 0)) = 0;
		clp->nameFlm = loadFlameForUTF8(menuClassNameFont, tmpStr, &tmpColor);

		int itemCount = levelRoot["classes"][i]["items"].size();
		Level* lp = nullptr;
		for (int j = 0;j < itemCount;j++) {
			if (!lp)
				lp = clp->levels = new Level;
			else {
				lp->next = new Level;
				lp->next->prev = lp;
				lp = lp->next;
			}
			lp->bestFlm = nullptr;
			lp->cover = nullptr;
			lp->sample = nullptr;

			*(lp->id + levelRoot["classes"][i]["items"][j]["id"].asString().copy(lp->id, 3, 0)) = 0;

			*(tmpStr + levelRoot["classes"][i]["items"][j]["name"].asString().copy(tmpStr, 255, 0)) = 0;
			lp->nameFlm = loadFlameForUTF8(SDL_strlen(tmpStr) < 32 ? menuSongnameFont : menuSongnameSmallFont, tmpStr, &tmpColor);

			*tmpStr = 'B';  *(tmpStr + 1) = 'y';  *(tmpStr + 2) = ' ';
			*(tmpStr + levelRoot["classes"][i]["items"][j]["by"].asString().copy(tmpStr + 3, 250, 0) + 3) = 0;
			lp->byFlm = loadFlameForUTF8(SDL_strlen(tmpStr) < 32 ? menuSongbyFont : menuSongbySmallFont, tmpStr, &tmpColor);

			if (levelRoot["classes"][i]["items"][j]["time"].empty()) {
				lp->timev = 0;

				SDL_strlcpy(tmpStr, "\xe9\x80\x9f\xe5\xba\xa6\xef\xbc\x9a", 16);
				*(tmpStr + 9 + levelRoot["classes"][i]["items"][j]["bpm"].asString().copy(tmpStr + 9, 31, 0)) = 0;
				lp->timeFlm = loadFlameForUTF8(menuSongtimeFont, tmpStr, &tmpColor);

				lp->interval = (int)levelRoot["classes"][i]["items"][j]["interval"].asFloat();
			}
			else {
				lp->interval = 0;

				SDL_strlcpy(tmpStr, "\xe6\x97\xb6\xe9\x95\xbf\xef\xbc\x9a", 16);
				*(tmpStr + 9 + levelRoot["classes"][i]["items"][j]["time"].asString().copy(tmpStr + 9, 31, 0)) = 0;
				lp->timeFlm = loadFlameForUTF8(menuSongtimeFont, tmpStr, &tmpColor);

				lp->timev = levelRoot["classes"][i]["items"][j]["timeVal"].asInt();
			}

		}
		lp->next = clp->levels;
		clp->levels->prev = lp;
	}
	clp->next = levelClasses;
	levelClasses->prev = clp;

	Level* lp = bonusTutorialLevel = new Level;
	lp->bestFlm = nullptr;
	lp->byFlm = nullptr;
	lp->timeFlm = nullptr;
	lp->cover = nullptr;
	*(lp->id + levelRoot["bonus"]["tutorial"]["id"].asString().copy(lp->id, 3, 0)) = 0;
	if (!userData["record"][lp->id].empty()) updateLevelBestFlame(lp);
	*(tmpStr + levelRoot["bonus"]["tutorial"]["name"].asString().copy(tmpStr, 31, 0)) = 0;
	lp->nameFlm = loadFlameForUTF8(menuSongnameFont, tmpStr, &tmpColor);
	lp->timev = levelRoot["bonus"]["tutorial"]["timeVal"].asInt();

	lp = bonusInfoLevel = new Level;
	lp->bestFlm = nullptr;
	lp->byFlm = nullptr;
	lp->timeFlm = nullptr;
	lp->cover = nullptr;
	*(lp->id + levelRoot["bonus"]["info"]["id"].asString().copy(lp->id, 3, 0)) = 0;
	if (!userData["record"][lp->id].empty()) updateLevelBestFlame(lp);
	*(tmpStr + levelRoot["bonus"]["info"]["name"].asString().copy(tmpStr, 31, 0)) = 0;
	lp->nameFlm = loadFlameForUTF8(menuSongnameFont, tmpStr, &tmpColor);
	lp->timev = levelRoot["bonus"]["info"]["timeVal"].asInt();

	ifs.close();
}

void musnake::Game::loadMap() {
	Json::Reader reader;
	Json::Value levelRoot;
	char tmpStr[256];
	catPath(tmpStr, (char*)"level/");
	strcat(tmpStr, (char*)levelinfo->id);
	strcat(tmpStr, (char*)"/map.mu");
	std::ifstream ifs(tmpStr, std::ios::binary);

	reader.parse(ifs, levelRoot);
	
	int darkNum = levelRoot["darkPos"].size();
	int darkx, darky;
	for( int i = 0; i< darkNum; i++ ){
		darkx = levelRoot["darkPos"][i]["x"].asInt();
		darky = levelRoot["darkPos"][i]["y"].asInt();
		Grid* map = gameMap[darkx][darky];
		map->setFlame(gridDarkFlame);
		map->objType = MU_GRID_OBJECT_TYPE_DARK;
	}

	int noneNum = levelRoot["none"].size();
	int nonex, noney;
	for( int i = 0; i< noneNum; i++ ){
		nonex = levelRoot["none"][i]["x"].asInt();
		noney = levelRoot["none"][i]["y"].asInt();
		Grid* map = gameMap[nonex][noney];
		map->setFlame(gridWaterFlame);
		map->objType = MU_GRID_OBJECT_TYPE_NONE;
	}

	int tailNum = levelRoot["snakePos"].size();
	length = tailNum;
	Snake *oldSnake, *newSnake;
	int oldx, oldy;
	oldx = oldy = -1;
	oldSnake = newSnake = nullptr;
	for( int i = 0; i < tailNum; i++ ){
		int x, y;
		x = levelRoot["snakePos"][i]["x"].asInt();
		y = levelRoot["snakePos"][i]["y"].asInt();
		gameMap[x][y]->setSnake( newSnake = new Snake );
		if( oldSnake ) {
			if( x > oldx ) {
				newSnake->setHeadDir(MU_SNAKE_DIRECT_LEFT);
				oldSnake->setTailDir(MU_SNAKE_DIRECT_RIGHT);
				Snake *oldoldSnake = oldSnake->getPrev();
				if( !oldoldSnake ) {
					oldSnake->setFlame(snakeFlame[MU_SNAKE_FLAME_HEAD_RIGHT]);
				}
				else {
					if( oldoldSnake->getTailDir() == MU_SNAKE_DIRECT_DOWN ){
						oldSnake->setFlame(snakeFlame[MU_SNAKE_FLAME_BODY_UPRIGHT]);
					}
					else if( oldoldSnake->getTailDir() == MU_SNAKE_DIRECT_UP ){
						oldSnake->setFlame(snakeFlame[MU_SNAKE_FLAME_BODY_RIGHTDOWN]);
					}
					else {
						oldSnake->setFlame(snakeFlame[MU_SNAKE_FLAME_BODY_RIGHTLEFT]);
					}
				}
				
			}
			else if( x < oldx ) {
				newSnake->setHeadDir(MU_SNAKE_DIRECT_RIGHT);
				oldSnake->setTailDir(MU_SNAKE_DIRECT_LEFT);
				Snake *oldoldSnake = oldSnake->getPrev();
				if( !oldoldSnake ) {
					oldSnake->setFlame(snakeFlame[MU_SNAKE_FLAME_HEAD_LEFT]);
				}
				else {
					if( oldoldSnake->getTailDir() == MU_SNAKE_DIRECT_DOWN ){
						oldSnake->setFlame(snakeFlame[MU_SNAKE_FLAME_BODY_UPLEFT]);
					}
					else if( oldoldSnake->getTailDir() == MU_SNAKE_DIRECT_UP ){
						oldSnake->setFlame(snakeFlame[MU_SNAKE_FLAME_BODY_DOWNLEFT]);
					}
					else {
						oldSnake->setFlame(snakeFlame[MU_SNAKE_FLAME_BODY_RIGHTLEFT]);
					}
				}
			}
			else if( y > oldy ) {
				newSnake->setHeadDir(MU_SNAKE_DIRECT_UP);
				oldSnake->setTailDir(MU_SNAKE_DIRECT_DOWN);
				Snake *oldoldSnake = oldSnake->getPrev();
				if( !oldoldSnake ) {
					oldSnake->setFlame(snakeFlame[MU_SNAKE_FLAME_HEAD_DOWN]);
				}
				else {
					if( oldoldSnake->getTailDir() == MU_SNAKE_DIRECT_RIGHT ){
						oldSnake->setFlame(snakeFlame[MU_SNAKE_FLAME_BODY_DOWNLEFT]);
					}
					else if( oldoldSnake->getTailDir() == MU_SNAKE_DIRECT_LEFT ){
						oldSnake->setFlame(snakeFlame[MU_SNAKE_FLAME_BODY_RIGHTDOWN]);
					}
					else {
						oldSnake->setFlame(snakeFlame[MU_SNAKE_FLAME_BODY_UPDOWN]);
					}
				}
			}
			else if( y < oldy ) {
				newSnake->setHeadDir(MU_SNAKE_DIRECT_DOWN);
				oldSnake->setTailDir(MU_SNAKE_DIRECT_UP);
				Snake *oldoldSnake = oldSnake->getPrev();
				if( !oldoldSnake ) {
					oldSnake->setFlame(snakeFlame[MU_SNAKE_FLAME_HEAD_UP]);
				}
				else {
					if( oldoldSnake->getTailDir() == MU_SNAKE_DIRECT_LEFT ){
						oldSnake->setFlame(snakeFlame[MU_SNAKE_FLAME_BODY_UPRIGHT]);
					}
					else if( oldoldSnake->getTailDir() == MU_SNAKE_DIRECT_RIGHT ){
						oldSnake->setFlame(snakeFlame[MU_SNAKE_FLAME_BODY_UPLEFT]);
					}
					else {
						oldSnake->setFlame(snakeFlame[MU_SNAKE_FLAME_BODY_UPDOWN]);
					}
				}
			}

			newSnake->setPrev(oldSnake);
			oldSnake->setNext(newSnake);
		}
		else
		{
			setSnakeHead(newSnake);
			base.x = -( x - 10 )*40;
			base.y = -( y - 7)*40;
		}
		
		oldSnake = newSnake;
		oldx = x;
		oldy = y;
	}
	int lastDir = newSnake->getHeadDir();
	if( lastDir == MU_SNAKE_DIRECT_DOWN )
		newSnake->setFlame(snakeFlame[MU_SNAKE_FLAME_TAIL_DOWN]);
	else if( lastDir == MU_SNAKE_DIRECT_UP )
		newSnake->setFlame(snakeFlame[MU_SNAKE_FLAME_TAIL_UP]);
	else if( lastDir == MU_SNAKE_DIRECT_LEFT )
		newSnake->setFlame(snakeFlame[MU_SNAKE_FLAME_TAIL_LEFT]);
	else if( lastDir == MU_SNAKE_DIRECT_RIGHT )
		newSnake->setFlame(snakeFlame[MU_SNAKE_FLAME_TAIL_RIGHT]);
	setSnakeTail(newSnake);


	int foodx, foody;
	for( int i = 0; i<3; i++ ){
		foodx = levelRoot["food"][i]["x"].asInt();
		foody = levelRoot["food"][i]["y"].asInt();
		if( foodx == -1 && foody == -1 ) continue;
		food[i] = new Food(i);
		gameMap[foodx][foody]->setFood(food[i]);
	}

	int blockNum = levelRoot["blockPos"].size();
	int blockx, blocky;
	for( int i = 0; i< blockNum; i++ ){
		blockx = levelRoot["blockPos"][i]["x"].asInt();
		blocky = levelRoot["blockPos"][i]["y"].asInt();
		Grid* map = gameMap[blockx][blocky];
		map->setFlame(gridBlockFlame);
		map->objType = MU_GRID_OBJECT_TYPE_BLOCK;
	}

	ifs.close();
}

void musnake::Game::loadNotes(){
	char tmpStr[256];
	char levelFile[64];

	SDL_strlcpy(levelFile, levelPath, 64);
	SDL_strlcat(levelFile, "/notes.mu", 64);
	catPath(tmpStr, levelFile);

	std::ifstream ifs(tmpStr, std::ios::binary);
	if (ifs.is_open()) {
		Json::Reader reader;
		Json::Value root;
		Note* endn = nullptr;

		reader.parse(ifs, root);

		noteCount = root.size();
		for (int i = 0;i < noteCount;i++) {
			Note* np = newNote(root[i].asInt64());

			if (!endn) {
				endn = note = np;
			}
			else {
				endn->next = np;
				endn = endn->next;
			}
		}
		ifs.close();
	}
}

void preSetToast(unsigned long arg) {
	void showToast(unsigned long arg);

	musnake::addDelayFunc(&(musnake::thisGame->toastQueue), &showToast, arg, 0);
}

void showToast(unsigned long arg) {
	using namespace musnake;
	Toast* tp = (Toast*)arg;
	if (tp->pre < 255) {
		tp->pre += getTimeDelta();
		if (tp->pre > 255) tp->pre = 255;
		tp->flame->setAlpha(tp->pre);
	}
	else if (tp->duration < 255) tp->flame->setAlpha(tp->duration);

	tp->flame->draw(tp->x, tp->y);
	tp->duration -= getTimeDelta();
	if (tp->duration <= 0) {
		delete tp->flame;
		delete tp;
	}
	else {
		addDelayFunc(&(thisGame->timingFunc), &preSetToast, arg, 0);
	}
}

void musnake::Game::loadToast() {
	char tmpStr[256];
	char levelFile[64];

	SDL_strlcpy(levelFile, levelPath, 64);
	SDL_strlcat(levelFile, "/toast.mu", 64);
	catPath(tmpStr, levelFile);

	std::ifstream ifs(tmpStr, std::ios::binary);
	if (ifs.is_open()) {
		Json::Reader reader;
		Json::Value root;
		SDL_Color tmpColor = { 255, 255, 255, 255 };

		reader.parse(ifs, root);

		int rs = root.size();
		for (int i = 0;i < rs;i++) {
			Toast* tp = new Toast;
			*(tmpStr + root[i]["str"].asString().copy(tmpStr, 256)) = 0;
			tp->flame = loadFlameForUTF8(gameToastFont, tmpStr, &tmpColor);
			tp->duration = root[i]["duration"].asInt();
			tp->x = root[i]["x"].asInt();
			tp->y = root[i]["y"].asInt();
			if (!root[i]["centered"].empty()) tp->flame->anchorCenter(&tp->x, &tp->y);
			tp->pre = 0;

			addDelayFunc(&toastQueue, &showToast, (unsigned long)tp, root[i]["time"].asInt());
		}
		ifs.close();
	}
	else {

	}
}

void musnake::Game::unloadToast() {
	DelayFunc* np;

	while (np = toastQueue) {
		toastQueue = np->next;
		delete ((Toast*)np->arg)->flame;
		delete np;
	}
}
