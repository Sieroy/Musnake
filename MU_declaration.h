#pragma once

#include <random>
#include "SDL.h"

namespace musnake {
	typedef struct _LevelPanel {
		char name[24];
		char time[8];  // h:mm:ss，不会有比这还长的曲子了吧？即使有我也不会往游戏里加~
		Mix_Music* sample;  // 预览歌
		Flame* flame;  // 封面
		struct _LevelPanel* prev;
		struct _LevelPanel* next;
	}LevelPanel;
	LevelPanel* levels = nullptr;

	// 程序状态枚举
	enum MusnakeState {
		MU_STATE_OVER,
		MU_STATE_RUNNING,
		MU_STATE_GAMING,
	};
	// 程序当前状态
	int musnakeState;

	SDL_Rect drawableScreenArea;

	// ================================ MU_flame.h =================================

	class Flame;


	// == MU_path.h

	// 程序所在路径
	char PATH[256];

	// 初始化路径
	void initPath(char* path);
	// 由相对路径获取绝对路径
	void catPath(char* dest, char* relative);


	// ================================ MU_time.h =================================

	typedef struct _DelayFunc {
		void (*func)(unsigned long);
		unsigned long arg;  // 函数要用的参数。如果一个不够，就用指针代替，记得强转+释放
		unsigned long long time;
		struct _DelayFunc* next;
	}DelayFunc;

	/* 将函数插入到指定的延时函数表中 */
	void addDelayFunc(DelayFunc** list, void (*func)(unsigned long), unsigned long arg, long long delay);
	/* 轮流触发到达时间的延时函数 */
	void triggerDelayFunc(DelayFunc** list);
	/* 清空目标表内的延时函数 */
	void clearDelayFunc(DelayFunc** list);

	unsigned long long timeVal = 0;
	unsigned long long timeDelta = 0;
	unsigned long fps = 0;

	/* 更新时间值 */
	inline unsigned long long updateTime();

	/* 获取当前的时间值 */
	inline unsigned long long getTimeVal();

	/* 获取当前时间值与上一次更新间的时间差 */
	inline unsigned long long getTimeDelta();


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
	// 格子内物品种类
	enum GridObjectType {
		MU_GRID_OBJECT_TYPE_DARK = -1,  // 类似空气墙的东西
		MU_GRID_OBJECT_TYPE_EMPTY = 0,  // 空格
		MU_GRID_OBJECT_TYPE_SNAKE,  // 蛇体
		MU_GRID_OBJECT_TYPE_BLOCK,  // 障碍物
		MU_GRID_OBJECT_TYPE_FOOD,  // 食物
	};


	// ================================ MU_food.h ================================

	class Food;


	// ================================ MU_game.h ================================

	// 游戏状态枚举
	enum GameState {
		MU_GAME_STATE_RUNNING,
		MU_GAME_STATE_LOADING,
		MU_GAME_STATE_PAUSED,
		MU_GAME_STATE_OVER
	};

	// 蛇体帧组助记符
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

		// 下面是动态的图

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

	Grid* gameMap[20][15] = { nullptr };  // 单局游戏用到的地格们，第一维为X坐标，第二维为Y坐标
	Flame* snakeFlame[50] = { nullptr };  // 绘制蛇要用到的帧们
	Flame* foodFlame[1] = { nullptr };  // 绘制食物要用的帧组们，测试阶段一个就够
	Flame* hpFlame[3][2] = { nullptr };  // 绘制血条要用到的帧组们
	Flame* notesignFlame[2] = { nullptr };
	Flame* charFlame[96] = { nullptr };  // 绘制文字要用的帧，从ASCII-32开始
	Game* thisGame = nullptr;  // 当前进行中的游戏对象（鱼，好大的鱼，虎纹鲨鱼……）
	int noteDelta = 60;  // 节奏偏移

	std::default_random_engine Rander;  // 随机数生成器
}
