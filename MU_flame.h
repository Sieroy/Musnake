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

	void setTexture(SDL_Texture* texture);  // ����֡������
	void setDuration(int duration);  // ����֡�ĳ���ʱ��
	void setNext(Flame* next);  // ������һ֡
	void setGroupId(int id);  // ���֡��ţ�֡���������terminateʱ���ȶ����ͷ���Щ֡

	Flame* getNext();  // ��ȡ��һ֡
	long long getDuration();  // ��ȡ��ǰ֡Ӧ������ʱ��

	void draw(SDL_Renderer* render, SDL_Rect* rect);  // ��ָ���ص����

private:
	int groupId;
	SDL_Texture* tex;  // ��֡������ͼ����
	long long duration;  // ��֡Ҫ������ʱ�䣬-1��ʾʼ��
	Flame* next;  // ��һ֡
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
	if (next != nullptr && next->groupId == groupId) delete next;  // �ݹ��ͬ���֡ȫ�е�
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
