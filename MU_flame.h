#pragma once

#include "SDL.h"

namespace musnake {
	class Flame;
}

class musnake::Flame {
public:
	Flame();
	~Flame();

	void setTexture(SDL_Texture* texture);  // 设置帧的纹理
	void setDuration(int duration);  // 设置帧的持续时间
	void setNext(Flame* next);  // 设置下一帧

	Flame* getNext();  // 获取下一帧
	long long getDuration();  // 获取当前帧应持续的时间

	void draw(SDL_Renderer* render, SDL_Rect* rect);  // 在指定地点绘制

private:
	SDL_Texture* tex;  // 该帧的纹理图对象
	long long duration;  // 该帧要持续的时间，-1表示始终
	Flame* next;  // 下一帧
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
