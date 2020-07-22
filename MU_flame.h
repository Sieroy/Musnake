#pragma once

#include "SDL.h"

namespace musnake {
	class Flame;
}

class musnake::Flame {
public:
	Flame();
	~Flame();

	void setTexture(SDL_Texture* texture);  // ����֡������
	void setDuration(int duration);  // ����֡�ĳ���ʱ��
	void setNext(Flame* next);  // ������һ֡

	Flame* getNext();  // ��ȡ��һ֡
	long long getDuration();  // ��ȡ��ǰ֡Ӧ������ʱ��

	void draw(SDL_Renderer* render, SDL_Rect* rect);  // ��ָ���ص����

private:
	SDL_Texture* tex;  // ��֡������ͼ����
	long long duration;  // ��֡Ҫ������ʱ�䣬-1��ʾʼ��
	Flame* next;  // ��һ֡
};

musnake::Flame::Flame() {
	tex = nullptr;
	duration = 0;
	next = nullptr;
}

musnake::Flame::~Flame() {
	if(tex) SDL_DestroyTexture(tex);
}

inline void musnake::Flame::setTexture(SDL_Texture* texture){
	tex = texture;
}

inline void musnake::Flame::setDuration(int duration){
	Flame::duration = duration;
}

inline void musnake::Flame::setNext(Flame* next){
	Flame::next = next;
}

inline musnake::Flame* musnake::Flame::getNext(){
	return next;
}

inline void musnake::Flame::draw(SDL_Renderer* render, SDL_Rect* rect){
	SDL_RenderCopy(render, tex, NULL, rect);
}
