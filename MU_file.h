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
		*(clp->name + levelRoot["classes"][i]["name"].asString().copy(clp->name, 31, 0)) = 0;
		tmpSurf = TTF_RenderUTF8_Blended(menuClassNameFont, clp->name, tmpColor);
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
			*(lp->name + levelRoot["classes"][i]["items"][j]["name"].asString().copy(lp->name, 31, 0)) = 0;
			*(lp->time + levelRoot["classes"][i]["items"][j]["time"].asString().copy(lp->time, 31, 0)) = 0;
			lp->timev = levelRoot["classes"][i]["items"][j]["timeVal"].asInt();
			tmpSurf = TTF_RenderUTF8_Blended(menuSongnameFont, lp->name, tmpColor);
			lp->nameFlm = new Flame(tmpSurf, NULL, -1);
			SDL_FreeSurface(tmpSurf);
			tmpSurf = TTF_RenderText_Blended(menuSongtimeFont, lp->time, tmpColor);
			lp->timeFlm = new Flame(tmpSurf, NULL, -1);
			SDL_FreeSurface(tmpSurf);
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
}

void musnake::Game::loadMap() {
	Json::Reader reader;
	Json::Value levelRoot;
	char tmpStr[256];
	catPath(tmpStr, (char*)"map\\maps.mu");
	std::ifstream ifs(tmpStr, std::ios::binary);

	reader.parse(ifs, levelRoot);
	int tailNum = levelRoot[levelinfo->id]["tailNum"].asInt();
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


}