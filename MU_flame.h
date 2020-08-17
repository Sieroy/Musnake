#pragma once

#include "SDL.h"

#include "MU_declaration.h"

class musnake::Flame {
public:
	Flame();
	Flame(SDL_Texture* texture, int duration);
	Flame(SDL_Surface* surf, SDL_Rect* blitrect, int duration);
	~Flame();

	void setAlpha(Uint8 alpha);  // 设置纹理的alpha值
	void setTexture(SDL_Texture* texture);  // 设置帧的纹理
	void setDuration(int duration);  // 设置帧的持续时间
	void setNext(Flame* next);  // 设置下一帧，如果next非空，则转为对next进行setNext
	void setNext(Flame* next, int forced);  // 强制设置this的下一帧，注意内存泄漏问题
	void setGroupId(int id);  // 标记帧组号，帧组号用来在terminate时能稳定地释放这些帧
	void anchorCenter(int* x, int* y);  // 将给定的中心坐标偏移至Flame顶点位置

	Flame* getNext();  // 获取下一帧
	long long getDuration();  // 获取当前帧应持续的时间

	void draw(SDL_Renderer* render, SDL_Rect* rect);  // 在指定地点绘制
	void draw(SDL_Renderer* render, int x, int y);  // 指定基点绘制

	void draw_centered(SDL_Renderer* render, int x, int y, double angle=0);  // 指定中心点旋转绘制

private:
	int w = 0;
	int h = 0;
	int groupId = -1;
	SDL_Texture* tex = nullptr;  // 该帧的纹理图对象
	long long duration = -1;  // 该帧要持续的时间，-1表示始终
	Flame* next = nullptr;  // 下一帧
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
		texture = SDL_CreateTextureFromSurface(render, surf);
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
		texture = SDL_CreateTextureFromSurface(render, tmpSurf);
		w = tmpSurf->w;
		h = tmpSurf->h;
		SDL_FreeSurface(tmpSurf);
	}
	setTexture(texture);
	setDuration(duration);
}

musnake::Flame::~Flame() {
	if (next != nullptr && next->groupId == groupId) delete next;  // 递归把同组的帧全毙掉
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

inline musnake::Flame* musnake::Flame::getNext(){
	return next;
}

inline long long musnake::Flame::getDuration() {
	return duration;
}

inline void musnake::Flame::draw(SDL_Renderer* render, SDL_Rect* rect){
	SDL_RenderCopy(render, tex, NULL, rect);
}

inline void musnake::Flame::draw(SDL_Renderer* render, int x, int y) {
	SDL_Rect r = { x, y, w, h };
	SDL_RenderCopy(render, tex, NULL, &r);
}

inline void musnake::Flame::draw_centered(SDL_Renderer* render, int x, int y, double angle) {
	if (!angle) {
		draw(render, x - w / 2, y - h / 2);
	}
	else {
		SDL_Rect r = { x - w / 2, y - h / 2, w, h };
		SDL_RenderCopyEx(render, tex, NULL, &r, angle, NULL, SDL_FLIP_NONE);
	}
}
