#pragma once

#include <random>
#include <time.h>

#include <SDL.h>
#include <SDL_ttf.h>
#include "json/json.h"

#define MU_WINDOW_WIDTH 800
#define MU_WINDOW_HEIGHT 600

namespace musnake {
	enum MusnakeState {
		MU_STATE_OVER,
		MU_STATE_RUNNING,
		MU_STATE_GAMING,
	};

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

	enum GridObjectType {
		MU_GRID_OBJECT_TYPE_DARK = -1,
		MU_GRID_OBJECT_TYPE_EMPTY = 0,
		MU_GRID_OBJECT_TYPE_SNAKE,
		MU_GRID_OBJECT_TYPE_BLOCK,
		MU_GRID_OBJECT_TYPE_FOOD,
		MU_GRID_OBJECT_TYPE_NONE,	
	};

	enum SnakeDirect {
		MU_SNAKE_DIRECT_NONE = -1,
		MU_SNAKE_DIRECT_UP = 0,
		MU_SNAKE_DIRECT_RIGHT,
		MU_SNAKE_DIRECT_DOWN,
		MU_SNAKE_DIRECT_LEFT
	};

	enum LoadingIndex {
		MU_LOAD_PUBLIC,
		MU_LOAD_TITLE,
		MU_LOAD_MENU,
		MU_LOAD_CONFIG,
		MU_LOAD_GAME
	};

	enum MusicChannel {
		MU_MUSIC_CHANNEL_BGM,
		MU_MUSIC_CHANNEL_SE
	};

	class Flame;
	class Element;
	class Snake;
	class Grid;
	class Food;
	class Game;

	typedef struct _DelayFunc {
		void (*func)(unsigned long);
		unsigned long arg;
		long long time;
		struct _DelayFunc* next;
	}DelayFunc;

	typedef struct _Level {
		char id[4];  // 最多3位ID号，用来确定路径
		int index;
		int timev;  // 总时间ms
		int interval;
		Mix_Chunk* sample;  // 预览歌
		Flame* cover;  // 封面
		Flame* nameFlm;
		Flame* byFlm;
		Flame* timeFlm;
		Flame* bestFlm;
		struct _LevelClass* myClass;
		struct _Level* prev;
		struct _Level* next;
	}Level;

	typedef struct _LevelClass {
		int index;
		Level* levels;
		Flame* nameFlm;
		struct _LevelClass* prev;
		struct _LevelClass* next;
	}LevelClass;

	typedef struct _Toast {
		musnake::Flame* flame;
		int pre;
		int duration;
		int x;
		int y;
	}Toast;

	const char* keyName[4] = { "up", "down", "left", "right" };

	SDL_Rect musnakeRect = { 0,0,MU_WINDOW_WIDTH,MU_WINDOW_HEIGHT };
	SDL_Point musnakeCenterPoint = { MU_WINDOW_WIDTH / 2, MU_WINDOW_HEIGHT / 2 };
	SDL_Renderer* musnakeRender = nullptr;
	SDL_Window* musnakeWindow = nullptr;

	std::default_random_engine Rander((unsigned int)time(NULL));  // 随机数生成器

	Mix_Chunk* titleBGM = nullptr;
	Mix_Chunk* configBGM = nullptr;
	Mix_Chunk* gamewinBGM = nullptr;
	Mix_Chunk* gameloseBGM = nullptr;
	Mix_Chunk* gameHurtSe = nullptr;
	Mix_Chunk* gameEatSe = nullptr;

	Flame* snakeFlame[50] = { nullptr };
	Flame* gridFlame = nullptr;
	Flame* gridDarkFlame = nullptr;
	Flame* gridBlockFlame = nullptr;
	Flame* gridWaterFlame = nullptr;
	Flame* foodFlame[3] = { nullptr };
	Flame* foodPointerFlame[3][2] = { nullptr };
	Flame* hpFlame[3][2] = { nullptr };
	Flame* notesignFlame[3] = { nullptr };
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
	Flame* gameloseBGFlame = nullptr;
	Flame* gameloseFGFlame = nullptr;
	Flame* configSetKeyFlame = nullptr;
	Flame* configSetDeltaFlame = nullptr;
	Flame* configSetPointerFlame = nullptr;
	Flame* titleEnterButtonFlame = nullptr;
	Flame* menuBackButtonFlame = nullptr;
	Flame* menuUpButtonFlame = nullptr;
	Flame* menuDownButtonFlame = nullptr;
	Flame* menuPlayButtonFlame = nullptr;
	Flame* menuConfigButtonFlame = nullptr;
	Flame* menuClassButtonFlame = nullptr;
	Flame* configBackButtonFlame = nullptr;
	Flame* configOKButtonFlame = nullptr;
	Flame* configTutorialButtonFlame = nullptr;
	Flame* configDeleteButtonFlame = nullptr;
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
	Flame* text_DeleteData_Flame = nullptr;
	Flame* text_Tutorial_Flame = nullptr;
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
	TTF_Font* menuSongnameSmallFont = nullptr;
	TTF_Font* menuSongbyFont = nullptr;  // 菜单处的作者字体
	TTF_Font* menuSongbySmallFont = nullptr;
	TTF_Font* menuSongtimeFont = nullptr;  // 菜单处歌曲时长字体
	TTF_Font* menuSongBestTextFont = nullptr;  // 菜单处最高纪录文字
	TTF_Font* menuSongBestDataFont = nullptr;  // 菜单处最高纪录数据
	TTF_Font* configSettingFont = nullptr;  // 设置页各分项标签字体
	TTF_Font* configKeyFont = nullptr;  // 设置页按键设置 按键字体
	TTF_Font* configLabelFont = nullptr;  // 设置页面偏移值提示文字字体
	TTF_Font* gameScorelabelFont = nullptr;  // 游戏时分数提示文字字体
	TTF_Font* gameCombolabelFont = nullptr;  // 游戏时连击提示文字字体
	TTF_Font* gameToastFont = nullptr;  // 游戏中Toast文字字体
	TTF_Font* gamePauseTitleFont = nullptr;  // 游戏暂停标题字体
	TTF_Font* gameWinScorelabelFont = nullptr;  // 游戏通关得分提示文字字体
	TTF_Font* gameWinLengthlabelFont = nullptr;  // 游戏通关蛇长提示字体
	TTF_Font* gameWinNewBestFont = nullptr;  // 游戏通关后新纪录字体
	TTF_Font* numberTotalFont = nullptr;
	TTF_Font* numberScoreFont = nullptr;
	TTF_Font* numberHitsFont = nullptr;
	TTF_Font* numberFPSFont = nullptr;
	TTF_Font* numberConfigDeltaFont = nullptr;

	LevelClass* levelClasses = nullptr;
	Level* bonusTutorialLevel = nullptr;
	Level* bonusInfoLevel = nullptr;

	Json::Value userData;
	Json::Value levelData;
	int noteDelta = 0;  // 节奏偏移
	SDL_Keycode musnakeKey[4] = { 0 };
	Flame* configKeyFlame[4] = { nullptr };

	unsigned long fps = 0;

	int musnakeState;
	bool loadingList[5] = { false };
	Game* thisGame = nullptr;  // 当前进行中的游戏对象（鱼，好大的鱼，虎纹鲨鱼……）



	void initPath(char* path);
	void catPath(char* dest, char* relative);

	void loadFonts();
	void loadLevels();

	void loadForLevel(Level* lp);
	void loadForClass(LevelClass* clp);
	void unloadForLevel(Level* lp);
	void unloadForClass(LevelClass* clp);

	void loadForPublic();
	void loadForTitle();
	void loadForMenu();
	void loadForConfig();
	void loadForGame();

	void unloadForPublic();
	void unloadForTitle();
	void unloadForMenu();
	void unloadForConfig();
	void unloadForGame();

	Flame* loadFlameFromFile(char* relapath);
	Flame* loadFlameForText(TTF_Font* font, char* text, SDL_Color* color);
	Flame* loadFlameForUTF8(TTF_Font* font, char* text, SDL_Color* color);

	void drawNumber(Flame** flames, int num, int x, int y);
	void drawNumber_Centered(Flame** flames, int num, int x, int y);
	
	inline unsigned long long updateTime();
	inline unsigned long long getTimeVal();
	inline unsigned long getTimeDelta();

	void addDelayFunc(DelayFunc** list, void (*func)(unsigned long), unsigned long arg, long long delay);
	int removeDelayFuncByFuncArg(DelayFunc** list, void(*func)(unsigned long), unsigned long arg);
	void triggerDelayFunc(DelayFunc** list);
	void clearDelayFunc(DelayFunc** list);

	void loadUserData();
	void updateUserScore(char* levelId, int rank, int score, int length);
	void updateUserKeySetting(int keyType, SDL_Keycode keyCode);
	void emptyUserData();
	void flushUserData();

	inline void parseRankStr(char* str, int rankv);
	inline void updateLevelBestFlame(Level* lp);
}
