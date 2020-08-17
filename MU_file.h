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
#include "MU_Flame.h"
#include "MU_game.h"
#include "MU_time.h"

void musnake::loadUserData() {
	Json::Reader reader;
	char tmpStr[256];
	catPath(tmpStr, (char*)"data\\save.mu");
	std::ifstream ifs(tmpStr, std::ios::binary);
	reader.parse(ifs, userData);
}

void musnake::updateUserScore(char* levelId, int rank, int score, int length) {
	if (rank >= 0) userData["record"][levelId]["rank"] = rank;
	if (score >= 0) userData["record"][levelId]["score"] = score;
	if (length) userData["record"][levelId]["length"] = length;
	if (userData["record"][levelId]["times"].empty())
		userData["record"][levelId]["times"] = 1;
	else
		userData["record"][levelId]["times"] = userData["record"][levelId]["times"].asInt() + 1;
}

void musnake::updateUserKeySetting(char* keyType, char* keyName, char* keyCode){
	char keys[16];
	SDL_strlcpy(keys, keyType, 15);
	SDL_strlcat(keys, "KeyCode", 15);
	userData["settings"]["key"][keys] = keyCode;
	SDL_strlcpy(keys, keyType, 15);
	SDL_strlcat(keys, "KeyName", 15);
	userData["settings"]["key"][keys] = keyName;
}

void musnake::flushUserData() {
	Json::FastWriter writer;
	char tmpStr[256];
	catPath(tmpStr, (char*)"data\\save.mu");
	std::ofstream ofs(tmpStr, std::ios::out);
	ofs << writer.write(userData);
	ofs.close();
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

	parseRankStr(tmpStr, userData["record"][lp->id]["rank"].asInt());
	textSurf = TTF_RenderText_Blended(menuSongBestNumFont, tmpStr, tmpColor);
	rect.x = 20;  rect.y = 30;  rect.w = textSurf->w; rect.h = textSurf->h;
	SDL_BlitSurface(textSurf, NULL, tmpSurf, &rect);
	SDL_FreeSurface(textSurf);

	int2str(tmpStr, userData["record"][lp->id]["score"].asInt());
	SDL_strlcat(tmpStr, " pt", 32);
	textSurf = TTF_RenderText_Blended(menuSongBestNumFont, tmpStr, tmpColor);
	rect.x = 84;  rect.y = 30;  rect.w = textSurf->w; rect.h = textSurf->h;
	SDL_BlitSurface(textSurf, NULL, tmpSurf, &rect);
	SDL_FreeSurface(textSurf);
	
	int2str(tmpStr, userData["record"][lp->id]["length"].asInt());
	SDL_strlcat(tmpStr, " m", 32);
	textSurf = TTF_RenderText_Blended(menuSongBestNumFont, tmpStr, tmpColor);
	rect.x = 200;  rect.y = 30;  rect.w = textSurf->w; rect.h = textSurf->h;
	SDL_BlitSurface(textSurf, NULL, tmpSurf, &rect);
	SDL_FreeSurface(textSurf);

	if (lp->bestFlm) delete lp->bestFlm;
	lp->bestFlm = new Flame(tmpSurf, NULL, -1);
}

void musnake::loadLevels() {
	SDL_Surface* tmpSurf;
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
		tmpSurf = TTF_RenderUTF8_Blended(menuClassNameFont, tmpStr, tmpColor);
		clp->nameFlm = new Flame(tmpSurf, NULL, -1);
		SDL_FreeSurface(tmpSurf);

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
			tmpSurf = TTF_RenderUTF8_Blended(menuSongnameFont, tmpStr, tmpColor);
			lp->nameFlm = new Flame(tmpSurf, NULL, -1);
			SDL_FreeSurface(tmpSurf);

			*(tmpStr + levelRoot["classes"][i]["items"][j]["time"].asString().copy(tmpStr, 31, 0)) = 0;
			tmpSurf = TTF_RenderText_Blended(menuSongtimeFont, tmpStr, tmpColor);
			lp->timeFlm = new Flame(tmpSurf, NULL, -1);
			SDL_FreeSurface(tmpSurf);

			*tmpStr = 'B';  *(tmpStr + 1) = 'y';  *(tmpStr + 2) = ' ';
			*(tmpStr + levelRoot["classes"][i]["items"][j]["by"].asString().copy(tmpStr + 3, 28, 0) + 3) = 0;
			tmpSurf = TTF_RenderUTF8_Blended(menuSongbyFont, tmpStr, tmpColor);
			lp->byFlm = new Flame(tmpSurf, NULL, -1);
			SDL_FreeSurface(tmpSurf);

			lp->timev = levelRoot["classes"][i]["items"][j]["timeVal"].asInt();

			SDL_strlcpy(ss, "level\\", 48);
			SDL_strlcat(ss, lp->id, 48);
			SDL_strlcat(ss, "\\sample.mp3", 48);
			catPath(tmpStr, ss);
			lp->sample = Mix_LoadMUS(tmpStr);
			SDL_strlcpy(ss, "level\\", 48);
			SDL_strlcat(ss, lp->id, 48);
			SDL_strlcat(ss, "\\cover.png", 48);
			catPath(tmpStr, ss);
			tmpSurf = IMG_Load(tmpStr);
			lp->cover = new Flame(SDL_CreateTextureFromSurface(render, tmpSurf), -1);
			SDL_FreeSurface(tmpSurf);
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
	tmpSurf = TTF_RenderUTF8_Blended(menuSongnameFont, tmpStr, tmpColor);
	lp->nameFlm = new Flame(tmpSurf, NULL, -1);
	SDL_FreeSurface(tmpSurf);
	lp->timev = levelRoot["bonus"]["tutorial"]["timeVal"].asInt();

	lp = bonusInfoLevel = new Level;
	lp->bestFlm = nullptr;
	lp->byFlm = nullptr;
	lp->timeFlm = nullptr;
	*(lp->id + levelRoot["bonus"]["info"]["id"].asString().copy(lp->id, 3, 0)) = 0;
	if (!userData["record"][lp->id].empty()) updateLevelBestFlame(lp);
	*(tmpStr + levelRoot["bonus"]["info"]["name"].asString().copy(tmpStr, 31, 0)) = 0;
	tmpSurf = TTF_RenderUTF8_Blended(menuSongnameFont, tmpStr, tmpColor);
	lp->nameFlm = new Flame(tmpSurf, NULL, -1);
	SDL_FreeSurface(tmpSurf);
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

	tp->flame->draw(render, tp->x, tp->y);
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
			SDL_Surface* tmpSurf;
			*(tmpStr + root[i]["str"].asString().copy(tmpStr, 256)) = 0;
			tmpSurf = TTF_RenderUTF8_Blended(gameToastFont, tmpStr, tmpColor);
			tp->flame = new Flame(tmpSurf, NULL, -1);
			SDL_FreeSurface(tmpSurf);
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
