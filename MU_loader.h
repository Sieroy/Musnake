#pragma once
#pragma warning(disable : 4996)

#include <fstream>

#include "json/json.h"
#include "SDL.h"
#include "SDL_ttf.h"
#include "SDL_image.h"
#include "SDL_mixer.h"

#include "MU_declaration.h"
#include "MU_path.h"
#include "MU_flame.h"
#include "MU_game.h"
#include "MU_time.h"

inline void musnake::loadUserData() {
	Json::Reader reader;
	char tmpStr[256];
	catPath(tmpStr, (char*)"data\\save.mu");
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

		char s[2] = { keyCode, 0 };
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
		char s[4] = { '(', (keyCode - SDLK_KP_1 + 49), ')', 0 };
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

inline void musnake::flushUserData() {
	Json::FastWriter writer;
	char tmpStr[256];
	catPath(tmpStr, (char*)"data\\save.mu");
	std::ofstream ofs(tmpStr, std::ios::out);
	ofs << writer.write(userData);
	ofs.close();
}


inline void musnake::loadFonts() {
	char tmpPath[256];

	catPath(tmpPath, (char*)"font\\SHOWG.TTF");
	titleMusnakeFont = TTF_OpenFont(tmpPath, 144);

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
}

void musnake::loadImages() {
	char tmpPath[256];
	SDL_Surface* picSurf;
	SDL_Rect tmpRect = { 0,0,170,1000 };

	// 初始页面背景以及修边的载入
	catPath(tmpPath, (char*)"image\\bg_main.png");
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

	configFGMask = loadFlameFromFile((char*)"image\\mask_configkey.png");
	configFGMask->setAlpha(0);
	gamePauseBGMask = loadFlameFromFile((char*)"image\\mask_gamepause.png");
	gamewinBGFlame = loadFlameFromFile((char*)"image\\bg_gamewin.png");
	gameloseBGFlame = loadFlameFromFile((char*)"image\\bg_gamelose.png");
	gameloseFGFlame = loadFlameFromFile((char*)"image\\fg_gamelose.png");
}

void musnake::loadSound() {
	char tmpPath[256];

	catPath(tmpPath, (char*)"sound\\bgm_title.mp3");
	titleBGM = Mix_LoadMUS(tmpPath);

	catPath(tmpPath, (char*)"sound\\bgm_config.mp3");
	configBGM = Mix_LoadMUS(tmpPath);
}

void musnake::loadText() {
	char tmpStr[2];
	SDL_Color tmpColor = { 255,255,255,255 };

	titleMusnakeFlame = loadFlameForText(titleMusnakeFont, (char*)"MUSNAKE", &tmpColor);
	titleAuthorFlame = loadFlameForText(titleAuthorFont, (char*)"By Sieroy & StdCat", &tmpColor);

	text_KeyConf_Flame = loadFlameForUTF8(configSettingFont, (char*)"\xe6\xb8\xb8\xe6\x88\x8f\xe6\x8c\x89\xe9\x94\xae\xe8\xae\xbe\xe7\xbd\xae", &tmpColor);
	text_DeltaConf_Flame = loadFlameForUTF8(configSettingFont, (char*)"\xe8\x8a\x82\xe5\xa5\x8f\xe5\x81\x8f\xe7\xa7\xbb\xe8\xae\xbe\xe7\xbd\xae", &tmpColor);
	text_DeltaValue_Flame = loadFlameForUTF8(configDeltaLabelFont, (char*)"\xe5\x81\x8f\xe7\xa7\xbb\xe5\x80\xbc: ", &tmpColor);
	text_Score_Flame = loadFlameForUTF8(gameScorelabelFont, (char*)"\xe5\xbe\x97\xe5\x88\x86", &tmpColor);
	text_Hits_Flame = loadFlameForText(gameCombolabelFont, (char*)"Hits!", &tmpColor);
	text_TotalScore_Flame = loadFlameForUTF8(gameWinScorelabelFont, (char*)"\xe5\x88\x86\xe6\x95\xb0", &tmpColor);
	text_TotalLength_Flame = loadFlameForUTF8(gameWinLengthlabelFont, (char*)"\xe9\x95\xbf\xe5\xba\xa6", &tmpColor);

	tmpStr[1] = 0;
	for (char i = 0;i < 10;i++) {
		tmpStr[0] = i + '0';
		numberConfigDeltaFlame[i] = loadFlameForText(numberConfigDeltaFont, tmpStr, &tmpColor);
		numberFPSFlame[i] = loadFlameForText(numberFPSFont, tmpStr, &tmpColor);
		numberHitsFlame[i] = loadFlameForText(numberHitsFont, tmpStr, &tmpColor);
		numberScoreFlame[i] = loadFlameForText(numberScoreFont, tmpStr, &tmpColor);
		numberTotalFlame[i] = loadFlameForText(numberTotalFont, tmpStr, &tmpColor);
	}
	numberConfigDeltaFlame[10] = loadFlameForText(numberConfigDeltaFont, (char*)"-", &tmpColor);

	gamePauseTitleFlame = loadFlameForText(gamePauseTitleFont, (char*)"- PAUSE -", &tmpColor);

	gamewinNewBestFlame = loadFlameForUTF8(gameWinNewBestFont, (char*)"\xe6\x96\xb0\xe7\xba\xaa\xe5\xbd\x95\xef\xbc\x81\xef\xbc\x81", &tmpColor);

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
	catPath(tmpStr, (char*)"data\\list.mu");
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
			char ss[64];
			if (!lp)
				lp = clp->levels = new Level;
			else {
				lp->next = new Level;
				lp->next->prev = lp;
				lp = lp->next;
			}
			lp->bestFlm = nullptr;
			*(lp->id + levelRoot["classes"][i]["items"][j]["id"].asString().copy(lp->id, 3, 0)) = 0;
			if (!userData["record"][lp->id].empty()) {
				updateLevelBestFlame(lp);
			}

			*(tmpStr + levelRoot["classes"][i]["items"][j]["name"].asString().copy(tmpStr, 31, 0)) = 0;
			lp->nameFlm = loadFlameForUTF8(menuSongnameFont, tmpStr, &tmpColor);

			*tmpStr = 'B';  *(tmpStr + 1) = 'y';  *(tmpStr + 2) = ' ';
			*(tmpStr + levelRoot["classes"][i]["items"][j]["by"].asString().copy(tmpStr + 3, 28, 0) + 3) = 0;
			lp->byFlm = loadFlameForUTF8(menuSongbyFont, tmpStr, &tmpColor);

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

			SDL_strlcpy(ss, "level\\", 48);
			SDL_strlcat(ss, lp->id, 48);
			SDL_strlcat(ss, "\\sample.mp3", 48);
			catPath(tmpStr, ss);
			lp->sample = Mix_LoadMUS(tmpStr);
			SDL_strlcpy(ss, "level\\", 48);
			SDL_strlcat(ss, lp->id, 48);
			SDL_strlcat(ss, "\\cover.png", 48);
			lp->cover = loadFlameFromFile(ss);
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
	*(lp->id + levelRoot["bonus"]["tutorial"]["id"].asString().copy(lp->id, 3, 0)) = 0;
	if (!userData["record"][lp->id].empty()) updateLevelBestFlame(lp);
	*(tmpStr + levelRoot["bonus"]["tutorial"]["name"].asString().copy(tmpStr, 31, 0)) = 0;
	lp->nameFlm = loadFlameForUTF8(menuSongnameFont, tmpStr, &tmpColor);
	lp->timev = levelRoot["bonus"]["tutorial"]["timeVal"].asInt();

	lp = bonusInfoLevel = new Level;
	lp->bestFlm = nullptr;
	lp->byFlm = nullptr;
	lp->timeFlm = nullptr;
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
	catPath(tmpStr, (char*)"data\\maps.mu");
	std::ifstream ifs(tmpStr, std::ios::binary);

	reader.parse(ifs, levelRoot);
	int tailNum = levelRoot[levelinfo->id]["tailNum"].asInt();
	length = tailNum;
	Snake *oldSnake, *newSnake;
	int oldx, oldy;
	oldx = oldy = -1;
	oldSnake = newSnake = nullptr;
	for( int i = 0; i < tailNum; i++ ){
		int x, y;
		x = levelRoot[levelinfo->id]["tailPos"][i]["x"].asInt();
		y = levelRoot[levelinfo->id]["tailPos"][i]["y"].asInt();
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
	//int foodx, foody;
	//foodx = levelRoot[levelinfo->id]["foodPos"]["x"].asInt();
	//foody = levelRoot[levelinfo->id]["foodPos"]["y"].asInt();
	//if( foodx == -1 && foody == -1 ) return;
	//food = new Food;
	//food->setFlame(foodFlame[0]);
	//gameMap[foodx][foody]->setFood(food);

	ifs.close();
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
	SDL_strlcat(levelFile, "\\toast.mu", 64);
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
