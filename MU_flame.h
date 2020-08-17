#pragma once

#include "SDL.h"

#include "MU_declaration.h"

class musnake::Flame {
public:
	Flame();
	Flame(SDL_Texture* texture, int duration);
	Flame(SDL_Surface* surf, SDL_Rect* blitrect, int duration);
	~Flame();

	void setAlpha(Uint8 alpha);  // ���������alphaֵ
	void setTexture(SDL_Texture* texture);  // ����֡������
	void setDuration(int duration);  // ����֡�ĳ���ʱ��
	void setNext(Flame* next);  // ������һ֡�����next�ǿգ���תΪ��next����setNext
	void setNext(Flame* next, int forced);  // ǿ������this����һ֡��ע���ڴ�й©����
	void setGroupId(int id);  // ���֡��ţ�֡���������terminateʱ���ȶ����ͷ���Щ֡
	void anchorCenter(int* x, int* y);  // ����������������ƫ����Flame����λ��

	Flame* getNext();  // ��ȡ��һ֡
	long long getDuration();  // ��ȡ��ǰ֡Ӧ������ʱ��

	void draw(SDL_Renderer* render, SDL_Rect* rect);  // ��ָ���ص����
	void draw(SDL_Renderer* render, int x, int y);  // ָ���������

	void draw_centered(SDL_Renderer* render, int x, int y, double angle=0);  // ָ�����ĵ���ת����

private:
	int w = 0;
	int h = 0;
	int groupId = -1;
	SDL_Texture* tex = nullptr;  // ��֡������ͼ����
	long long duration = -1;  // ��֡Ҫ������ʱ�䣬-1��ʾʼ��
	Flame* next = nullptr;  // ��һ֡
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
	if (next != nullptr && next->groupId == groupId) delete next;  // �ݹ��ͬ���֡ȫ�е�
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
