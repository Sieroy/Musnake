#pragma once
#pragma warning(disable : 4996)

#include <fstream>
#include <iostream>
#include "json/json.h"
#include "SDL.h"
#include "SDL_ttf.h"
#include "SDL_image.h"
#include "SDL_mixer.h"

#include "MU_declaration.h"
#include "MU_path.h"
#include "MU_Flame.h"
#include "MU_game.h"

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

void musnake::loadLevels() {
	SDL_Surface* tmpSurf;
	SDL_Color tmpColor = { 255, 255, 255, 255 };
	Json::Reader reader;
	Json::Value levelRoot;
	char tmpStr[256];
	catPath(tmpStr, (char*)"level\\list.mu");
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
			*(lp->id + levelRoot["classes"][i]["items"][j]["id"].asString().copy(lp->id, 3, 0)) = 0;
			if (!userData["record"][lp->id].empty()) {
				*(lp->bestRank + userData["record"][lp->id]["rank"].asString().copy(lp->bestRank, 3, 0)) = 0;
				lp->bestScore = userData["record"][lp->id]["score"].asInt();
				lp->bestLength = userData["record"][lp->id]["length"].asInt();
			}
			else {
				*(lp->bestRank) = 0;
				lp->bestScore = -1;
				lp->bestLength = 0;
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
	int foodx, foody;
	foodx = levelRoot[levelinfo->id]["foodPos"]["x"].asInt();
	foody = levelRoot[levelinfo->id]["foodPos"]["y"].asInt();
	if( foodx == -1 && foody == -1 ) return;
	food = new Food;
	food->setFlame(foodFlame[0]);
	gameMap[foodx][foody]->setFood(food);

	ifs.close();
}
