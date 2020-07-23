#pragma once

#include "SDL.h"

namespace musnake {
	class Flame;
}

class musnake::Flame {
public:
	Flame();
	Flame(SDL_Texture* texture, int duration);
	~Flame();

	void setTexture(SDL_Texture* texture);  // 设置帧的纹理
	void setDuration(int duration);  // 设置帧的持续时间
	void setNext(Flame* next);  // 设置下一帧
	void setGroupId(int id);  // 标记帧组号，帧组号用来在terminate时能稳定地释放这些帧

	Flame* getNext();  // 获取下一帧
	long long getDuration();  // 获取当前帧应持续的时间

	void draw(SDL_Renderer* render, SDL_Rect* rect);  // 在指定地点绘制

private:
	int groupId;
	SDL_Texture* tex;  // 该帧的纹理图对象
	long long duration;  // 该帧要持续的时间，-1表示始终
	Flame* next;  // 下一帧
};

musnake::Flame::Flame() {
	tex = nullptr;
	duration = 0;
	next = nullptr;
}

musnake::Flame::Flame(SDL_Texture* texture, int duration) {
	tex = texture;
	Flame::duration = duration;
	next = nullptr;
}

musnake::Flame::~Flame() {
	if (next != nullptr && next->groupId == groupId) delete next;  // 递归把同组的帧全毙掉
	if (tex) SDL_DestroyTexture(tex);
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

inline void musnake::Flame::setGroupId(int id) {
	groupId = id;
}

inline musnake::Flame* musnake::Flame::getNext(){
	return next;
}

inline void musnake::Flame::draw(SDL_Renderer* render, SDL_Rect* rect){
	SDL_RenderCopy(render, tex, NULL, rect);
}
