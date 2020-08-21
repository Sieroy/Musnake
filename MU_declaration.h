#pragma once

#include <random>
#include <time.h>
#include "SDL.h"
#include "SDL_ttf.h"
#include "json/json.h"

#define MU_WINDOW_WIDTH 800
#define MU_WINDOW_HEIGHT 600

namespace musnake {
	// 锟斤拷锟斤拷状态枚锟斤拷
	enum MusnakeState {
		MU_STATE_OVER,
		MU_STATE_RUNNING,
		MU_STATE_GAMING,
	};
	// 锟斤拷锟斤拷前状态
	int musnakeState;

	SDL_Rect musnakeRect = { 0,0,MU_WINDOW_WIDTH,MU_WINDOW_HEIGHT };
	SDL_Point musnakeCenterPoint = { MU_WINDOW_WIDTH / 2, MU_WINDOW_HEIGHT / 2 };
	SDL_Keycode musnakeKey[4] = { 0 };

	const char* keyName[4] = { "up", "down", "left", "right" };


	// ================================ MU_flame.h =================================

	class Flame;


	// == MU_path.h

	// 锟斤拷锟斤拷锟斤拷锟斤拷路锟斤拷
	char PATH[256];

	// 锟斤拷始锟斤拷路锟斤拷
	void initPath(char* path);
	// 锟斤拷锟斤拷锟铰凤拷锟斤拷锟饺★拷锟斤拷锟铰凤拷锟�
	void catPath(char* dest, char* relative);

	Flame* loadFlameFromFile(char* relapath);

	Flame* loadFlameForText(TTF_Font* font, char* text, SDL_Color* color);

	Flame* loadFlameForUTF8(TTF_Font* font, char* text, SDL_Color* color);

	void drawNumber(SDL_Renderer* render, Flame** flames, int num, int x, int y);
	void drawNumber_Centered(SDL_Renderer* render, Flame** flames, int num, int x, int y);


	// ================================ MU_time.h =================================

	typedef struct _DelayFunc {
		void (*func)(unsigned long);
		unsigned long arg;  // 锟斤拷锟斤拷要锟矫的诧拷锟斤拷锟斤拷锟斤拷锟揭伙拷锟斤拷锟斤拷锟斤拷锟斤拷锟斤拷锟街革拷锟斤拷锟芥，锟角碉拷强转+锟酵凤拷
		long long time;
		struct _DelayFunc* next;
	}DelayFunc;

	/* 锟斤拷锟斤拷锟斤拷锟斤拷锟诫到指锟斤拷锟斤拷锟斤拷时锟斤拷锟斤拷锟斤拷锟斤拷 */
	void addDelayFunc(DelayFunc** list, void (*func)(unsigned long), unsigned long arg, long long delay);

	int removeDelayFuncByFuncArg(DelayFunc** list, void(*func)(unsigned long), unsigned long arg);
	/* 锟斤拷锟斤拷锟斤拷锟斤拷锟斤拷锟斤拷时锟斤拷锟斤拷锟绞憋拷锟斤拷锟� */
	void triggerDelayFunc(DelayFunc** list);
	/* 锟斤拷锟侥匡拷锟斤拷锟节碉拷锟斤拷时锟斤拷锟斤拷 */
	void clearDelayFunc(DelayFunc** list);

	unsigned long long timeVal = 0;
	unsigned long timeDelta = 0;
	unsigned long fps = 0;

	DelayFunc* publicTimingFunc = nullptr;

	/* 锟斤拷锟斤拷时锟斤拷值 */
	inline unsigned long long updateTime();

	/* 锟斤拷取锟斤拷前锟斤拷时锟斤拷值 */
	inline unsigned long long getTimeVal();

	/* 锟斤拷取锟斤拷前时锟斤拷值锟斤拷锟斤拷一锟轿革拷锟铰硷拷锟绞憋拷锟斤拷 */
	inline unsigned long getTimeDelta();


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
	// 锟斤拷锟斤拷锟斤拷锟斤拷品锟斤拷锟斤拷
	enum GridObjectType {
		MU_GRID_OBJECT_TYPE_DARK = -1,  // 锟斤拷锟狡匡拷锟斤拷墙锟侥讹拷锟斤拷
		MU_GRID_OBJECT_TYPE_EMPTY = 0,  // 锟秸革拷
		MU_GRID_OBJECT_TYPE_SNAKE,  // 锟斤拷锟斤拷
		MU_GRID_OBJECT_TYPE_BLOCK,  // 锟较帮拷锟斤拷
		MU_GRID_OBJECT_TYPE_FOOD,  // 食锟斤拷
	};


	// ================================ MU_food.h ================================

	class Food;


	// ================================ MU_game.h ================================

	// 锟斤拷戏状态枚锟斤拷
	enum GameState {
		MU_GAME_STATE_RUNNING,
		MU_GAME_STATE_LOADING,
		MU_GAME_STATE_PAUSED,
		MU_GAME_STATE_OVER,
		MU_GAME_STATE_END
	};

	enum GameKeyType {
		MU_KEY_UP,
		MU_KEY_DOWN,
		MU_KEY_LEFT,
		MU_KEY_RIGHT
	};

	// 锟斤拷锟斤拷帧锟斤拷锟斤拷锟角凤拷
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

		// 锟斤拷锟斤拷锟角讹拷态锟斤拷图

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
	Grid* gameMap[64][64] = { nullptr };  // 锟斤拷锟斤拷锟斤拷戏锟矫碉拷锟侥地革拷锟角ｏ拷锟斤拷一维为X锟斤拷锟疥，锟节讹拷维为Y锟斤拷锟斤拷

	Mix_Music* titleBGM = nullptr;
	Mix_Music* configBGM = nullptr;

	Flame* snakeFlame[50] = { nullptr };  // 锟斤拷锟斤拷锟斤拷要锟矫碉拷锟斤拷帧锟斤拷
	Flame* gridFlame = nullptr;
	Flame* gridDarkFlame = nullptr;
	Flame* gridBlockFlame = nullptr;
	Flame* gridWaterFlame = nullptr;
	Flame* foodFlame[3] = { nullptr };  // 锟斤拷锟斤拷食锟斤拷要锟矫碉拷帧锟斤拷锟角ｏ拷锟斤拷锟皆阶讹拷一锟斤拷锟酵癸拷
	Flame* foodPointerFlame[3][2] = { nullptr };
	Flame* hpFlame[3][2] = { nullptr };  // 锟斤拷锟斤拷血锟斤拷要锟矫碉拷锟斤拷帧锟斤拷锟斤拷
	Flame* notesignFlame[3] = { nullptr };
	Flame* charFlame[96] = { nullptr };  // 锟斤拷锟斤拷锟斤拷锟斤拷要锟矫碉拷帧锟斤拷锟斤拷ASCII-32锟斤拷始

	Flame* titleBGFlame = nullptr;
	Flame* titleLBGFlame = nullptr;
	Flame* titleRBGFlame = nullptr;
	Flame* titleMusnakeFlame = nullptr;
	Flame* titleAuthorFlame = nullptr;
	Flame* configFGMask = nullptr;
	Flame* gamePauseBGMask = nullptr;
	Flame* gamePauseTitleFlame = nullptr;
	Flame* gamewinBGFlame = nullptr;
	Flame* gamewinNewBestFlame = nullptr;
	Flame* configSetKeyFlame = nullptr;
	Flame* configSetDeltaFlame = nullptr;
	Flame* configSetPointerFlame = nullptr;
	Flame* configKeyFlame[4] = { nullptr };

	Flame* titleEnterButtonFlame = nullptr;
	Flame* menuBackButtonFlame = nullptr;
	Flame* menuUpButtonFlame = nullptr;
	Flame* menuDownButtonFlame = nullptr;
	Flame* menuPlayButtonFlame = nullptr;
	Flame* menuConfigButtonFlame = nullptr;
	Flame* menuClassButtonFlame = nullptr;
	Flame* configBackButtonFlame = nullptr;
	Flame* configOKButtonFlame = nullptr;
	Flame* gamePauseBackButtonLFlame = nullptr;
	Flame* gamePauseUpButtonFlame = nullptr;
	Flame* gamePauseDownButtonFlame = nullptr;
	Flame* gamePauseResumeButtonFlame[2] = { nullptr };
	Flame* gamePauseRetryButtonFlame[2] = { nullptr };
	Flame* gamePauseBackButtonFlame[2] = { nullptr };
	Flame* gameOverBackButtonFlame = nullptr;
	Flame* gameOverOKButtonFlame = nullptr;
	Flame* gameOverRetryButtonFlame = nullptr;

	Flame* text_Best_Flame = nullptr;
	Flame* text_KeyConf_Flame = nullptr;
	Flame* text_DeltaConf_Flame = nullptr;
	Flame* text_DeltaValue_Flame = nullptr;
	Flame* text_Score_Flame = nullptr;
	Flame* text_TotalScore_Flame = nullptr;
	Flame* text_TotalLength_Flame = nullptr;
	Flame* text_Hits_Flame = nullptr;

	Flame* numberTotalFlame[10] = { nullptr };
	Flame* numberScoreFlame[10] = { nullptr };
	Flame* numberHitsFlame[10] = { nullptr };
	Flame* numberFPSFlame[10] = { nullptr };
	Flame* numberConfigDeltaFlame[11] = { nullptr };

	Flame* rankFlame[7] = { nullptr };  // SSS  SS  S  A  B  C  D

	TTF_Font* titleMusnakeFont = nullptr;  // 标题，游戏名的字体
	TTF_Font* titleAuthorFont = nullptr;  // 标题下角的作者名字体
	TTF_Font* menuClassNameFont = nullptr;  // 菜单处分区名字体
	TTF_Font* menuSongnameFont = nullptr;  // 菜单处歌名字体
	TTF_Font* menuSongbyFont = nullptr;  // 菜单处的作者字体
	TTF_Font* menuSongtimeFont = nullptr;  // 菜单处歌曲时长字体
	TTF_Font* menuSongBestTextFont = nullptr;  // 菜单处最高纪录文字
	TTF_Font* menuSongBestDataFont = nullptr;  // 菜单处最高纪录数据
	TTF_Font* configSettingFont = nullptr;  // 设置页各分项标签字体
	TTF_Font* configKeyFont = nullptr;  // 设置页按键设置 按键字体
	TTF_Font* configDeltaLabelFont = nullptr;  // 设置页面偏移值提示文字字体
	TTF_Font* gameScorelabelFont = nullptr;  // 游戏时分数提示文字字体
	TTF_Font* gameCombolabelFont = nullptr;  // 游戏时连击提示文字字体
	TTF_Font* gameToastFont = nullptr;  // 游戏中Toast文字字体
	TTF_Font* gamePauseTitleFont = nullptr;  // 游戏暂停标题字体
	TTF_Font* gameWinScorelabelFont = nullptr;  // 游戏通关得分提示文字字体
	TTF_Font* gameWinLengthlabelFont = nullptr;  // 游戏通关蛇长提示字体
	TTF_Font* gameWinNewBestFont = nullptr;  // 游戏通关后新纪录字体
	TTF_Font* gameLoseTitleFont = nullptr;  // 游戏失败标题字体

	TTF_Font* numberTotalFont = nullptr;
	TTF_Font* numberScoreFont = nullptr;
	TTF_Font* numberHitsFont = nullptr;
	TTF_Font* numberFPSFont = nullptr;
	TTF_Font* numberConfigDeltaFont = nullptr;

	Game* thisGame = nullptr;  // 当前进行中的游戏对象（鱼，好大的鱼，虎纹鲨鱼……）
	int noteDelta = 0;  // 节奏偏移

	std::default_random_engine Rander((unsigned int)time(NULL));  // 随机数生成器

	void drawText(SDL_Renderer* render, char* text, int x, int y, int size);


	// ================================ MU_menu.h ================================
	// 锟街阶段伙拷没锟斤拷锟斤拷锟斤拷锟酵�

	typedef struct _Level {
		char id[4];  // 最多3位ID号，用来确定路径
		int timev;  // 总时间ms
		int interval;
		Mix_Music* sample;  // 预览歌
		Flame* cover;  // 封面
		Flame* nameFlm;
		Flame* byFlm;
		Flame* timeFlm;
		Flame* bestFlm;
		struct _Level* prev;
		struct _Level* next;
	}Level;

	typedef struct _LevelClass {
		Level* levels;
		Flame* nameFlm;
		struct _LevelClass* prev;
		struct _LevelClass* next;
	}LevelClass;

	LevelClass* levelClasses = nullptr;
	Level* bonusTutorialLevel = nullptr;
	Level* bonusInfoLevel = nullptr;


	// ================================ MU_file.h ================================

	typedef struct _Toast {
		musnake::Flame* flame;
		int pre;
		int duration;
		int x;
		int y;
	}Toast;

	SDL_Keycode MUK_UP = SDLK_UP;
	SDL_Keycode MUK_RIGHT = SDLK_RIGHT;
	SDL_Keycode MUK_DOWN = SDLK_DOWN;
	SDL_Keycode MUK_LEFT = SDLK_LEFT;

	inline void parseRankStr(char* str, int rankv);
	inline void updateLevelBestFlame(Level* lp);

	void loadUserData();
	void updateUserScore(char* levelId, int rank, int score, int length);
	void updateUserKeySetting(int keyType, SDL_Keycode keyCode);
	void flushUserData();
	void loadLevels();
	Json::Value userData;


}
