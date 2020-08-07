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

