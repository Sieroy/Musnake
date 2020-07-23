#pragma once

#include "SDL.h"
#include "SDL_image.h"

#include "MU_path.h"
#include "MU_grid.h"
#include "MU_snake.h"
#include "MU_flame.h"

namespace musnake{
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
		MU_SNAKE_FLAME_HEAD_LEFTtoLEFT,

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

	Grid* gameMap[64][64] = { nullptr };  // 单局游戏用到的地格们，第一维为X坐标，第二维为Y坐标
	Flame* snakeFlame[50] = { nullptr };  // 绘制蛇要用到的帧们
}

class musnake::Game {
public:
	Game();
	~Game();

	int state;
	SDL_Renderer* gameRender;

	void setRenderer(SDL_Renderer* render);

private:
	SDL_Rect drawRect;  // 当前屏幕绘制区域对应的完整地图的矩形
};

musnake::Game::Game() {
	// 按说这里应该是从配置文件里读数据来初始化地格的，现阶段就先写死吧
	SDL_Surface* picSurf, tmpSurf;
	SDL_Texture* tmpTex;

	state = MU_GAME_STATE_LOADING;

	// 这里定位图片文件的位置
	char snakeBodyPicturePath[256], snakeHeadPicturePath[256], snakeTailPicturePath[256];
	catPath(snakeBodyPicturePath, "image\\snake_0_body.png");
	catPath(snakeHeadPicturePath, "image\\snake_0_head.png");
	catPath(snakeTailPicturePath, "image\\snake_0_tail.png");

	// 开始装载蛇头图，有向蛇头对应的枚举从14号（MU_SNAKE_FLAME_HEAD_0toUP）开始，29号结束
	picSurf = IMG_Load(snakeHeadPicturePath);
	for (int i = 0;i < 16;i++) {  // 16种蛇头运动情况
		Flame* flames[8];
		for (int j = 0;j < 8;j++) {  // 每种运动情况有8帧
			SDL_Rect srect = { j * 20, i * 20, 20, 20 };
			SDL_BlitSurface(picSurf, &srect, &tmpSurf, NULL);
			tmpTex = SDL_CreateTextureFromSurface(gameRender, &tmpSurf);
			flames[j] = new Flame(tmpTex, 25);
			flames[j]->setGroupId(MU_SNAKE_FLAME_HEAD_0toUP + i);
			if (j)flames[j - 1]->setNext(flames[j]);
		}
		snakeFlame[MU_SNAKE_FLAME_HEAD_0toUP + i] = flames[0];
	}
	SDL_FreeSurface(picSurf);

	// 开始装载蛇尾图，有向蛇尾对应的枚举从30号（MU_SNAKE_FLAME_TAIL_UPshake）开始，49号结束
	picSurf = IMG_Load(snakeTailPicturePath);
	for (int i = 0;i < 20;i++) {  // 20种有向蛇尾的情况
		Flame* flames[8];
		for (int j = 0;j < 8;j++) {
			SDL_Rect srect = { j * 20, i * 20, 20, 20 };
			SDL_BlitSurface(picSurf, &srect, &tmpSurf, NULL);
			tmpTex = SDL_CreateTextureFromSurface(gameRender, &tmpSurf);
			flames[j] = new Flame(tmpTex, 25);
			flames[j]->setGroupId(MU_SNAKE_FLAME_TAIL_UPshake + i);
			if (j)flames[j - 1]->setNext(flames[j]);
		}
		snakeFlame[MU_SNAKE_FLAME_TAIL_UPshake + i] = flames[0];
	}
	SDL_FreeSurface(picSurf);

	// 开始装载一般蛇体图，一般蛇体对应的枚举从0号（MU_SNAKE_FLAME_HEAD_UP）开始，13号结束
	picSurf = IMG_Load(snakeBodyPicturePath);
	for (int j = MU_SNAKE_FLAME_HEAD_UP;j <= MU_SNAKE_FLAME_TAIL_LEFT;j++) {
		SDL_Rect srect = { j * 20, 0, 20, 20 };
		SDL_BlitSurface(picSurf, &srect, &tmpSurf, NULL);
		tmpTex = SDL_CreateTextureFromSurface(gameRender, &tmpSurf);
		snakeFlame[j] = new Flame(tmpTex, -1);
		snakeFlame[j]->setGroupId(j);
	}
	for (int j = MU_SNAKE_FLAME_BODY_UPDOWN;j <= MU_SNAKE_FLAME_BODY_UPLEFT;j++) {
		SDL_Rect srect = { (j - MU_SNAKE_FLAME_BODY_UPDOWN) * 20, 20, 20, 20 };
		SDL_BlitSurface(picSurf, &srect, &tmpSurf, NULL);
		tmpTex = SDL_CreateTextureFromSurface(gameRender, &tmpSurf);
		snakeFlame[j] = new Flame(tmpTex, -1);
		snakeFlame[j]->setGroupId(j);
	}
	SDL_FreeSurface(picSurf);

	/* 关于Sprites图的装载，首先肯定是有优化方法的。比如头、尾的首帧或末帧会有一个和静态的头、尾图重复。
	 * 于是这些相同的图就没必要再次装载了，程序控制着复制一下就好了，这样至少能优化不少内存问题。
	 * 此外，目前还只画好了一个皮肤，还是最简单的那种皮肤，之后再画别的皮肤的话，就可能出现原本应当静态的图变得动态之类的情况
	 * 这样还好，毕竟现在的帧组结构能应付的来，但如果动态图的Sprites长短不一怎么办……
	 * 现在我在考虑要不要给程序加上JSON支持，这样动态问题解决了，而且甚至还可以开放设区创作之类的。
	 * 先考虑着吧，测试顺利+时间充裕的话，倒也不是不可以。
	 */

	for (int i = 0;i < 64;i++) {
		for (int j = 0;j < 64;j++) {
			Grid* map = gameMap[i][j] = new Grid;
			map->setPosition(i * 40, j * 40, 40, 40);
			map->objType = MU_GRID_OBJECT_TYPE_EMPTY;
		}
	}
}

inline void musnake::Game::setRenderer(SDL_Renderer* render) {
	gameRender = render;
}
