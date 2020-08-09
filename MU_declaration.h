#pragma once

#include <random>
#include <time.h>
#include "SDL.h"
#include "SDL_ttf.h"

namespace musnake {
	// ����״̬ö��
	enum MusnakeState {
		MU_STATE_OVER,
		MU_STATE_RUNNING,
		MU_STATE_GAMING,
	};
	// ����ǰ״̬
	int musnakeState;

	

	// ================================ MU_flame.h =================================

	class Flame;


	// == MU_path.h

	// ��������·��
	char PATH[256];

	// ��ʼ��·��
	void initPath(char* path);
	// �����·����ȡ����·��
	void catPath(char* dest, char* relative);


	// ================================ MU_time.h =================================

	typedef struct _DelayFunc {
		void (*func)(unsigned long);
		unsigned long arg;  // ����Ҫ�õĲ��������һ������������ָ����棬�ǵ�ǿת+�ͷ�
		unsigned long long time;
		struct _DelayFunc* next;
	}DelayFunc;

	/* ���������뵽ָ������ʱ�������� */
	void addDelayFunc(DelayFunc** list, void (*func)(unsigned long), unsigned long arg, long long delay);
	/* ������������ʱ�����ʱ���� */
	void triggerDelayFunc(DelayFunc** list);
	/* ���Ŀ����ڵ���ʱ���� */
	void clearDelayFunc(DelayFunc** list);

	unsigned long long timeVal = 0;
	unsigned long long timeDelta = 0;
	unsigned long fps = 0;

	DelayFunc* publicTimingFunc = nullptr;

	/* ����ʱ��ֵ */
	inline unsigned long long updateTime();

	/* ��ȡ��ǰ��ʱ��ֵ */
	inline unsigned long long getTimeVal();

	/* ��ȡ��ǰʱ��ֵ����һ�θ��¼��ʱ��� */
	inline unsigned long long getTimeDelta();


	// ================================ MU_snake.h ================================

	class Element;

	// ================================ MU_snake.h ================================

	enum SnakeDirect {
		MU_SNAKE_DIRECT_NONE = -1,
		MU_SNAKE_DIRECT_UP = 0,
		MU_SNAKE_DIRECT_RIGHT,
		MU_SNAKE_DIRECT_DOWN,
		MU_SNAKE_DIRECT_LEFT
	};
	class Snake;


	// ================================ MU_grid.h ================================

	class Grid;
	// ��������Ʒ����
	enum GridObjectType {
		MU_GRID_OBJECT_TYPE_DARK = -1,  // ���ƿ���ǽ�Ķ���
		MU_GRID_OBJECT_TYPE_EMPTY = 0,  // �ո�
		MU_GRID_OBJECT_TYPE_SNAKE,  // ����
		MU_GRID_OBJECT_TYPE_BLOCK,  // �ϰ���
		MU_GRID_OBJECT_TYPE_FOOD,  // ʳ��
	};


	// ================================ MU_food.h ================================

	class Food;


	// ================================ MU_game.h ================================

	// ��Ϸ״̬ö��
	enum GameState {
		MU_GAME_STATE_RUNNING,
		MU_GAME_STATE_LOADING,
		MU_GAME_STATE_PAUSED,
		MU_GAME_STATE_OVER,
		MU_GAME_STATE_END
	};

	// ����֡�����Ƿ�
	enum SnakeFlameIndex {
		MU_SNAKE_FLAME_HEAD_UP,
		MU_SNAKE_FLAME_HEAD_RIGHT,
		MU_SNAKE_FLAME_HEAD_DOWN,
		MU_SNAKE_FLAME_HEAD_LEFT,

		MU_SNAKE_FLAME_TAIL_UP,
		MU_SNAKE_FLAME_TAIL_RIGHT,
		MU_SNAKE_FLAME_TAIL_DOWN,
		MU_SNAKE_FLAME_TAIL_LEFT,

		MU_SNAKE_FLAME_BODY_UPDOWN,
		MU_SNAKE_FLAME_BODY_RIGHTLEFT,
		MU_SNAKE_FLAME_BODY_UPRIGHT,
		MU_SNAKE_FLAME_BODY_RIGHTDOWN,
		MU_SNAKE_FLAME_BODY_DOWNLEFT,
		MU_SNAKE_FLAME_BODY_UPLEFT,

		// �����Ƕ�̬��ͼ

		MU_SNAKE_FLAME_HEAD_0toUP,
		MU_SNAKE_FLAME_HEAD_UPtoRIGHT,
		MU_SNAKE_FLAME_HEAD_UPtoDOWN,
		MU_SNAKE_FLAME_HEAD_UPtoLEFT,
		MU_SNAKE_FLAME_HEAD_0toRIGHT,
		MU_SNAKE_FLAME_HEAD_RIGHTtoUP,
		MU_SNAKE_FLAME_HEAD_RIGHTtoDOWN,
		MU_SNAKE_FLAME_HEAD_RIGHTtoLEFT,
		MU_SNAKE_FLAME_HEAD_0toDOWN,
		MU_SNAKE_FLAME_HEAD_DOWNtoUP,
		MU_SNAKE_FLAME_HEAD_DOWNtoRIGHT,
		MU_SNAKE_FLAME_HEAD_DOWNtoLEFT,
		MU_SNAKE_FLAME_HEAD_0toLEFT,
		MU_SNAKE_FLAME_HEAD_LEFTtoUP,
		MU_SNAKE_FLAME_HEAD_LEFTtoRIGHT,
		MU_SNAKE_FLAME_HEAD_LEFTtoDOWN,

		MU_SNAKE_FLAME_TAIL_UPshake,
		MU_SNAKE_FLAME_TAIL_UPto0,
		MU_SNAKE_FLAME_TAIL_RIGHTtoUP,
		MU_SNAKE_FLAME_TAIL_DOWNtoUP,
		MU_SNAKE_FLAME_TAIL_LEFTtoUP,
		MU_SNAKE_FLAME_TAIL_RIGHTshake,
		MU_SNAKE_FLAME_TAIL_RIGHTto0,
		MU_SNAKE_FLAME_TAIL_UPtoRIGHT,
		MU_SNAKE_FLAME_TAIL_DOWNtoRIGHT,
		MU_SNAKE_FLAME_TAIL_LEFTtoRIGHT,
		MU_SNAKE_FLAME_TAIL_DOWNshake,
		MU_SNAKE_FLAME_TAIL_DOWNto0,
		MU_SNAKE_FLAME_TAIL_UPtoDOWN,
		MU_SNAKE_FLAME_TAIL_RIGHTtoDOWN,
		MU_SNAKE_FLAME_TAIL_LEFTtoDOWN,
		MU_SNAKE_FLAME_TAIL_LEFTshake,
		MU_SNAKE_FLAME_TAIL_LEFTto0,
		MU_SNAKE_FLAME_TAIL_UPtoLEFT,
		MU_SNAKE_FLAME_TAIL_RIGHTtoLEFT,
		MU_SNAKE_FLAME_TAIL_DOWNtoLEFT,
	};

	class Game;

	SDL_Renderer* render = nullptr;
	Grid* gameMap[64][64] = { nullptr };  // ������Ϸ�õ��ĵظ��ǣ���һάΪX���꣬�ڶ�άΪY����

	Mix_Music* titleBGM = nullptr;

	Flame* snakeFlame[50] = { nullptr };  // ������Ҫ�õ���֡��
	Flame* foodFlame[1] = { nullptr };  // ����ʳ��Ҫ�õ�֡���ǣ����Խ׶�һ���͹�
	Flame* hpFlame[3][2] = { nullptr };  // ����Ѫ��Ҫ�õ���֡����
	Flame* notesignFlame[3] = { nullptr };
	Flame* charFlame[96] = { nullptr };  // ��������Ҫ�õ�֡����ASCII-32��ʼ

	Flame* titleBGFlame = nullptr;
	Flame* titleLBGFlame = nullptr;
	Flame* titleRBGFlame = nullptr;
	Flame* titleMusnakeFlame = nullptr;
	Flame* titleAuthorFlame = nullptr;
	Flame* gamePauseBGMask = nullptr;
	Flame* gamePauseTitleFlame = nullptr;
	Flame* gamewinBGFlame = nullptr;

	Flame* titleEnterButtonFlame = nullptr;
	Flame* menuBackButtonFlame = nullptr;
	Flame* menuUpButtonFlame = nullptr;
	Flame* menuDownButtonFlame = nullptr;
	Flame* menuPlayButtonFlame = nullptr;
	Flame* menuClassButtonFlame = nullptr;
	Flame* gamePauseBackButtonLFlame = nullptr;
	Flame* gamePauseUpButtonFlame = nullptr;
	Flame* gamePauseDownButtonFlame = nullptr;
	Flame* gamePauseResumeButtonFlame[2] = { nullptr };
	Flame* gamePauseRetryButtonFlame[2] = { nullptr };
	Flame* gamePauseBackButtonFlame[2] = { nullptr };
	Flame* gameOverBackButtonFlame = nullptr;
	Flame* gameOverOKButtonFlame = nullptr;
	Flame* gameOverRetryButtonFlame = nullptr;

	TTF_Font* titleMusnakeFont = nullptr;  // ���⣬��Ϸ��������
	TTF_Font* titleAuthorFont = nullptr;  // �����½ǵ�����������
	TTF_Font* menuClassNameFont = nullptr;  // �˵�������������
	TTF_Font* menuSongnameFont = nullptr;  // �˵�����������
	TTF_Font* menuSongtimeFont = nullptr;  // �˵�������ʱ������
	TTF_Font* gameScorelabelFont = nullptr;  // ��Ϸʱ������ʾ��������
	TTF_Font* gameScorenumFont = nullptr;  // ��Ϸʱ������������
	TTF_Font* gameCombolabelFont = nullptr;  // ��Ϸʱ������ʾ��������
	TTF_Font* gameCombonumFont = nullptr;  // ��Ϸʱ������������
	TTF_Font* gamePauseTitleFont = nullptr;  // ��Ϸ��ͣ��������
	TTF_Font* gamePauseSongnameFont = nullptr;  // ��Ϸ��ͣ��������
	TTF_Font* gameWinSongnameFont = nullptr;  // ��Ϸͨ��ҳ�ĸ���������
	TTF_Font* gameWinScorelabelFont = nullptr;  // ��Ϸͨ�ص÷���ʾ��������
	TTF_Font* gameWinScorenumFont = nullptr;  // ��Ϸͨ�ص÷���������
	TTF_Font* gameWinLengthlabelFont = nullptr;  // ��Ϸͨ���߳���ʾ����
	TTF_Font* gameWinLengthnumFont = nullptr;  // ��Ϸͨ���߳���������
	TTF_Font* gameLoseTitleFont = nullptr;  // ��Ϸʧ�ܱ�������
	TTF_Font* gameLoseSongnameFont = nullptr;  // ��Ϸʧ�ܸ�������

	Game* thisGame = nullptr;  // ��ǰ�����е���Ϸ�����㣬�ô���㣬�������㡭����
	int noteDelta = 60;  // ����ƫ��

	std::default_random_engine Rander(time(NULL));  // �����������

	void drawText(SDL_Renderer* render, char* text, int x, int y, int size);


	// ================================ MU_menu.h ================================
	// �ֽ׶λ�û�������ͷ

	typedef struct _Level {
		char id[4];  // ���3λID�ţ�����ȷ��·��
		char name[32];  // 10�����֣�����ǿǿ��
		char time[8];  // h:mm:ss�������б��⻹���������˰ɣ���ʹ����Ҳ��������Ϸ���~
		int timev;  // ��ʱ��ms
		Mix_Music* sample;  // Ԥ����
		Flame* cover;  // ����
		Flame* nameFlm;
		Flame* timeFlm;
		struct _Level* prev;
		struct _Level* next;
	}Level;

	typedef struct _LevelClass {
		char name[32];
		Level* levels;
		Flame* nameFlm;
		struct _LevelClass* prev;
		struct _LevelClass* next;
	}LevelClass;

	LevelClass* levelClasses = nullptr;


	// ================================ MU_file.h ================================
	void loadLevels();

}
