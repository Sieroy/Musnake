#pragma once

#include <SDL.h>
#include <SDL_image.h>

#include "MU_declaration.h"
#include "MU_path.h"

class musnake::Flame {
public:
	Flame();
	Flame(SDL_Texture* texture, int duration);
	Flame(SDL_Surface* surf, SDL_Rect* blitrect, int duration);
	~Flame();

	void setAlpha(Uint8 alpha);
	void setTexture(SDL_Texture* texture);
	void setDuration(int duration);
	void setNext(Flame* next);
	void setNext(Flame* next, int forced);
	void setGroupId(int id);
	void anchorCenter(int* x, int* y);

	unsigned getAlpha();
	int getW();
	int getH();
	Flame* getNext();
	long long getDuration();

	void draw(SDL_Rect* rect);
	void draw(int x, int y);

	void draw_centered(int x, int y, double angle=0);

private:
	int w = 0;
	int h = 0;
	int groupId = -1;
	SDL_Texture* tex = nullptr;
	long long duration = -1;
	Flame* next = nullptr;
};

musnake::Flame::Flame() {

}

musnake::Flame::Flame(SDL_Texture* texture, int duration) {
	tex = texture;
	Flame::duration = duration;
}

musnake::Flame::Flame(SDL_Surface* surf, SDL_Rect* blitrect, int duration) {
	SDL_Texture* texture;
	if (!blitrect) {
		texture = SDL_CreateTextureFromSurface(musnakeRender, surf);
		w = surf->w;
		h = surf->h;
	}
	else {
		SDL_Surface* tmpSurf;
#if SDL_BYTEORDER == SDL_BIG_ENDIAN
		tmpSurf = SDL_CreateRGBSurface(0, blitrect->w, blitrect->h, 32, 0xff000000, 0x00ff0000, 0x0000ff00, 0x000000ff);
#else
		tmpSurf = SDL_CreateRGBSurface(0, blitrect->w, blitrect->h, 32, 0x000000ff, 0x0000ff00, 0x00ff0000, 0xff000000);
#endif
		SDL_BlitSurface(surf, blitrect, tmpSurf, NULL);
		texture = SDL_CreateTextureFromSurface(musnakeRender, tmpSurf);
		w = tmpSurf->w;
		h = tmpSurf->h;
		SDL_FreeSurface(tmpSurf);
	}
	setTexture(texture);
	setDuration(duration);
}

musnake::Flame::~Flame() {
	if (next != nullptr && next->groupId == groupId) delete next;
	if (tex) SDL_DestroyTexture(tex);
}

inline void musnake::Flame::setAlpha(Uint8 alpha) {
	if (tex) SDL_SetTextureAlphaMod(tex, alpha);
}

inline void musnake::Flame::setTexture(SDL_Texture* texture){
	tex = texture;
}

inline void musnake::Flame::setDuration(int duration){
	Flame::duration = duration;
}

inline void musnake::Flame::setNext(Flame* next) {
	if (Flame::next != nullptr)Flame::next->setNext(next);
	else Flame::next = next;
}

inline void musnake::Flame::setNext(Flame* next, int forced) {
	Flame::next = next;
}

inline void musnake::Flame::setGroupId(int id) {
	groupId = id;
}

inline void musnake::Flame::anchorCenter(int* x, int* y) {
	*x -= w / 2;
	*y -= h / 2;
}

inline unsigned musnake::Flame::getAlpha() {
	Uint8 al;
	SDL_GetTextureAlphaMod(tex, &al);
	return al;
}

inline int musnake::Flame::getW() {
	return w;
}

inline int musnake::Flame::getH() {
	return h;
}

inline musnake::Flame* musnake::Flame::getNext(){
	return next;
}

inline long long musnake::Flame::getDuration() {
	return duration;
}

inline void musnake::Flame::draw(SDL_Rect* rect){
	SDL_RenderCopy(musnakeRender, tex, NULL, rect);
}

inline void musnake::Flame::draw(int x, int y) {
	SDL_Rect r = { x, y, w, h };
	SDL_RenderCopy(musnakeRender, tex, NULL, &r);
}

inline void musnake::Flame::draw_centered(int x, int y, double angle) {
	if (!angle) {
		draw(x - w / 2, y - h / 2);
	}
	else {
		SDL_Rect r = { x - w / 2, y - h / 2, w, h };
		SDL_RenderCopyEx(musnakeRender, tex, NULL, &r, angle, NULL, SDL_FLIP_NONE);
	}
}

inline musnake::Flame* musnake::loadFlameFromFile(char* relapath) {
	char tmpPath[256];
	catPath(tmpPath, relapath);
	SDL_Surface* tmpSurf = IMG_Load(tmpPath);
	Flame* fp = new Flame(tmpSurf, NULL, -1);
	SDL_FreeSurface(tmpSurf);
	return fp;
}

inline musnake::Flame* musnake::loadFlameForText(TTF_Font* font, char* text, SDL_Color* color) {
	SDL_Surface* tmpSurf = TTF_RenderText_Blended(font, text, *color);
	Flame* fp = new Flame(tmpSurf, NULL, -1);
	SDL_FreeSurface(tmpSurf);
	return fp;
}

inline musnake::Flame* musnake::loadFlameForUTF8(TTF_Font* font, char* text, SDL_Color* color) {
	if (!*text) return nullptr;
	SDL_Surface* tmpSurf = TTF_RenderUTF8_Blended(font, text, *color);
	Flame* fp = new Flame(tmpSurf, NULL, -1);
	SDL_FreeSurface(tmpSurf);
	return fp;
}

inline void musnake::drawNumber(Flame** flames, int num, int x, int y) {
	char ns[16], * p = ns;
	int2str(ns, num);
	while (*p) {
		if (*p == '-') {
			flames[10]->draw(x, y);
			x += flames[10]->getW();
		}
		else {
			flames[*p - '0']->draw(x, y);
			x += flames[*p - '0']->getW();
		}
		p++;
	}
}

inline void musnake::drawNumber_Centered(Flame** flames, int num, int x, int y) {
	char ns[16], * p = ns;
	int w = 0;
	int2str(ns, num);
	while (*p) w += flames[*(p++) - '0']->getW();
	x -= w / 2;
	p = ns;
	while (*p) {
		flames[*p - '0']->draw(x, y);
		x += flames[*p - '0']->getW();
		p++;
	}
}
