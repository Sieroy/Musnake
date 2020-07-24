#pragma once

#include "SDL.h"
#include "SDL_image.h"

#include "MU_declaration.h"
#include "MU_path.h"
#include "MU_grid.h"
#include "MU_snake.h"
#include "MU_flame.h"


class musnake::Game {
public:
	Game();
	~Game();

	int state;
	SDL_Renderer* gameRender;

	void setRenderer(SDL_Renderer* render);
	void setDelayFunc(void (*func)(unsigned long), unsigned long arg, int delay);
	void setSnakeHead(Snake* snake);
	void setSnakeTail(Snake* snake);

	// 移蛇函数，如果正常移动，就返回0，否则返回1并在函数内实现扣血之类的操作
	// 正常移动：踩着节拍向非尾向的方向、成功地移动；
	// 非正常移动：错节拍移动、尾向移动、撞墙、撞蛇身
	int moveSnake(int dir);

	// 为小蛇解移动锁
	void unlockMoving();
	
	// 游戏运行的小主函数
	void run();

private:
	int hp = 5;  // 蛇的血量，初始为5
	long long int note = -1;  // 节拍判定。到达某个音符的判定点后，note会被赋值为当前时间值，判定成功后变为-1
	bool movingLock = false;  // 为实现Moves per Second限制而加的移动锁
	int combo = 0;  // 连击数
	SDL_Rect drawRect;  // 当前屏幕绘制区域对应的完整地图的矩形
	Snake* snakeHead, * snakeTail;  // 蛇头和蛇尾
	DelayFunc* timingFunc = nullptr;  // 当局游戏用的延时函数表（注意实现暂停效果时翻新时间）
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

	// 然后对不同的帧组进行关联，艹了
	snakeFlame[MU_SNAKE_FLAME_HEAD_0toUP]->setNext(snakeFlame[MU_SNAKE_FLAME_HEAD_UP]);
	snakeFlame[MU_SNAKE_FLAME_HEAD_0toRIGHT]->setNext(snakeFlame[MU_SNAKE_FLAME_HEAD_RIGHT]);
	snakeFlame[MU_SNAKE_FLAME_HEAD_0toDOWN]->setNext(snakeFlame[MU_SNAKE_FLAME_HEAD_DOWN]);
	snakeFlame[MU_SNAKE_FLAME_HEAD_0toLEFT]->setNext(snakeFlame[MU_SNAKE_FLAME_HEAD_LEFT]);
	snakeFlame[MU_SNAKE_FLAME_HEAD_UPtoRIGHT]->setNext(snakeFlame[MU_SNAKE_FLAME_BODY_UPRIGHT]);
	snakeFlame[MU_SNAKE_FLAME_HEAD_UPtoDOWN]->setNext(snakeFlame[MU_SNAKE_FLAME_BODY_UPDOWN]);
	snakeFlame[MU_SNAKE_FLAME_HEAD_UPtoLEFT]->setNext(snakeFlame[MU_SNAKE_FLAME_BODY_UPLEFT]);
	snakeFlame[MU_SNAKE_FLAME_HEAD_RIGHTtoUP]->setNext(snakeFlame[MU_SNAKE_FLAME_BODY_UPRIGHT]);
	snakeFlame[MU_SNAKE_FLAME_HEAD_RIGHTtoDOWN]->setNext(snakeFlame[MU_SNAKE_FLAME_BODY_RIGHTDOWN]);
	snakeFlame[MU_SNAKE_FLAME_HEAD_RIGHTtoLEFT]->setNext(snakeFlame[MU_SNAKE_FLAME_BODY_RIGHTLEFT]);
	snakeFlame[MU_SNAKE_FLAME_HEAD_DOWNtoUP]->setNext(snakeFlame[MU_SNAKE_FLAME_BODY_UPDOWN]);
	snakeFlame[MU_SNAKE_FLAME_HEAD_DOWNtoRIGHT]->setNext(snakeFlame[MU_SNAKE_FLAME_BODY_RIGHTDOWN]);
	snakeFlame[MU_SNAKE_FLAME_HEAD_DOWNtoLEFT]->setNext(snakeFlame[MU_SNAKE_FLAME_BODY_DOWNLEFT]);
	snakeFlame[MU_SNAKE_FLAME_HEAD_LEFTtoUP]->setNext(snakeFlame[MU_SNAKE_FLAME_BODY_UPLEFT]);
	snakeFlame[MU_SNAKE_FLAME_HEAD_LEFTtoRIGHT]->setNext(snakeFlame[MU_SNAKE_FLAME_BODY_RIGHTLEFT]);
	snakeFlame[MU_SNAKE_FLAME_HEAD_LEFTtoDOWN]->setNext(snakeFlame[MU_SNAKE_FLAME_BODY_DOWNLEFT]);

	snakeFlame[MU_SNAKE_FLAME_TAIL_UPshake]->setNext(snakeFlame[MU_SNAKE_FLAME_TAIL_UP]);
	snakeFlame[MU_SNAKE_FLAME_TAIL_RIGHTtoUP]->setNext(snakeFlame[MU_SNAKE_FLAME_TAIL_UP]);
	snakeFlame[MU_SNAKE_FLAME_TAIL_DOWNtoUP]->setNext(snakeFlame[MU_SNAKE_FLAME_TAIL_UP]);
	snakeFlame[MU_SNAKE_FLAME_TAIL_LEFTtoUP]->setNext(snakeFlame[MU_SNAKE_FLAME_TAIL_UP]);
	snakeFlame[MU_SNAKE_FLAME_TAIL_RIGHTshake]->setNext(snakeFlame[MU_SNAKE_FLAME_TAIL_RIGHT]);
	snakeFlame[MU_SNAKE_FLAME_TAIL_UPtoRIGHT]->setNext(snakeFlame[MU_SNAKE_FLAME_TAIL_RIGHT]);
	snakeFlame[MU_SNAKE_FLAME_TAIL_DOWNtoRIGHT]->setNext(snakeFlame[MU_SNAKE_FLAME_TAIL_RIGHT]);
	snakeFlame[MU_SNAKE_FLAME_TAIL_LEFTtoRIGHT]->setNext(snakeFlame[MU_SNAKE_FLAME_TAIL_RIGHT]);
	snakeFlame[MU_SNAKE_FLAME_TAIL_DOWNshake]->setNext(snakeFlame[MU_SNAKE_FLAME_TAIL_DOWN]);
	snakeFlame[MU_SNAKE_FLAME_TAIL_UPtoDOWN]->setNext(snakeFlame[MU_SNAKE_FLAME_TAIL_DOWN]);
	snakeFlame[MU_SNAKE_FLAME_TAIL_RIGHTtoDOWN]->setNext(snakeFlame[MU_SNAKE_FLAME_TAIL_DOWN]);
	snakeFlame[MU_SNAKE_FLAME_TAIL_LEFTtoDOWN]->setNext(snakeFlame[MU_SNAKE_FLAME_TAIL_DOWN]);
	snakeFlame[MU_SNAKE_FLAME_TAIL_LEFTshake]->setNext(snakeFlame[MU_SNAKE_FLAME_TAIL_LEFT]);
	snakeFlame[MU_SNAKE_FLAME_TAIL_UPtoLEFT]->setNext(snakeFlame[MU_SNAKE_FLAME_TAIL_LEFT]);
	snakeFlame[MU_SNAKE_FLAME_TAIL_RIGHTtoLEFT]->setNext(snakeFlame[MU_SNAKE_FLAME_TAIL_LEFT]);
	snakeFlame[MU_SNAKE_FLAME_TAIL_DOWNtoLEFT]->setNext(snakeFlame[MU_SNAKE_FLAME_TAIL_LEFT]);
	// 艹了！

	/* 关于Sprites图的装载，首先肯定是有优化方法的。比如头、尾的首帧或末帧会有一个和静态的头、尾图重复。
	 * 于是这些相同的图就没必要再次装载了，程序控制着复制一下就好了，这样至少能优化不少内存问题。
	 * 此外，目前还只画好了一个皮肤，还是最简单的那种皮肤，之后再画别的皮肤的话，就可能出现原本应当静态的图变得动态之类的情况
	 * 这样还好，毕竟现在的帧组结构能应付的来，但如果动态图的Sprites长短不一怎么办……
	 * 现在我在考虑要不要给程序加上JSON支持，这样动态问题解决了，而且甚至还可以开放设区创作之类的。
	 * 先考虑着吧，测试顺利+时间充裕的话，倒也不是不可以。
	 */

	for (int i = 0;i < 20;i++) {
		for (int j = 0;j < 15;j++) {
			Grid* map = gameMap[i][j] = new Grid(i, j);
			map->setPosition(i * 40, j * 40, 40, 40);
			map->objType = MU_GRID_OBJECT_TYPE_EMPTY;
		}
	}

	Snake* sp[4];
	gameMap[7][3]->setSnake(sp[0] = new Snake);
	sp[0]->setTailDir(MU_SNAKE_DIRECT_LEFT);
	gameMap[6][3]->setSnake(sp[1] = new Snake);
	sp[1]->setHeadDir(MU_SNAKE_DIRECT_RIGHT);
	sp[1]->setTailDir(MU_SNAKE_DIRECT_LEFT);
	gameMap[5][3]->setSnake(sp[2] = new Snake);
	sp[2]->setHeadDir(MU_SNAKE_DIRECT_RIGHT);
	sp[2]->setTailDir(MU_SNAKE_DIRECT_LEFT);
	gameMap[4][3]->setSnake(sp[3] = new Snake);
	sp[3]->setHeadDir(MU_SNAKE_DIRECT_RIGHT);

	setSnakeHead(sp[0]);
	sp[0]->setNext(sp[1]);
	sp[1]->setNext(sp[2]);
	sp[2]->setNext(sp[3]);
	setSnakeTail(sp[3]);
	sp[3]->setPrev(sp[2]);
	sp[2]->setPrev(sp[1]);
	sp[1]->setPrev(sp[0]);
}

musnake::Game::~Game() {
	// 先保留着，因为目前还是测试阶段，一次运行的话没有卸载地格之类的必要（软件问题关掉就好~
	// 先把地格的释放写了吧
	for (int i = 0;i < 20;i++) {
		for (int j = 0;j < 15;j++) {
			delete gameMap[i][j];
		}
	}
	// 再把蛇的释放加上吧，利用蛇的半递归析构应该不难
	delete snakeHead;
}

inline void musnake::Game::setRenderer(SDL_Renderer* render) {
	gameRender = render;
}

inline void musnake::Game::setDelayFunc(void(*func)(unsigned long), unsigned long arg, int delay) {
	addDelayFunc(&timingFunc, func, arg, delay);
}

inline void musnake::Game::setSnakeHead(Snake* snake) {
	snakeHead = snake;
}

inline void musnake::Game::setSnakeTail(Snake* snake) {
	snakeTail = snake;
}

inline void musnake::Game::unlockMoving() {
	movingLock = false;
}

void unlockMoving_D(unsigned long arg) {
	musnake::thisGame->unlockMoving();
}

int musnake::Game::moveSnake(int dir) {
	Grid* gp = snakeHead->getGrid();
	Snake* sp;
	int returnVal = 0;
	int x = gp->x, y = gp->y;

	if (movingLock) {  // 如果超出了MPS的限制，则判定为手抖
		return 0;
	}
	else {
		movingLock = true;
		setDelayFunc(&unlockMoving_D, 0, 200);  // 0.2秒后自解开
	}

	if (dir == snakeHead->getTailDir()) {  // 返向
		combo = 0;
		dir = (dir + 2) % 4;
		returnVal = 1;
	}

	if (note == -1) {  // 当前没有音符
		combo = 0;
		returnVal = 1;
	}
	else if (getTimeVal() - note > 400) {  // 一个音符超过0.4秒没有按就视为miss。所以音符会稀疏一些，避免连打
		combo = 0;
		returnVal = 1;
		note = -1;
	}

	switch (dir) {
	case MU_SNAKE_DIRECT_UP:
		y--;
		break;
	case MU_SNAKE_DIRECT_RIGHT:
		x++;
		break;
	case MU_SNAKE_DIRECT_DOWN:
		y++;
		break;
	case MU_SNAKE_DIRECT_LEFT:
		x--;
		break;
	}
	gp = gameMap[x][y];

	switch (gp->objType) {
	case MU_GRID_OBJECT_TYPE_SNAKE:
		if (gp->getSnake() != snakeTail) {  // 如果所指为蛇身，则开始伤害判定并取消此次移动
	case MU_GRID_OBJECT_TYPE_BLOCK:  // 如果前方是障碍，则同
			hp -= 4;
			combo = 0;
			return 1;
		}  // 如果所指为蛇尾，接着下一个case开始生头并缩尾
	case MU_GRID_OBJECT_TYPE_EMPTY:
	case MU_GRID_OBJECT_TYPE_FOOD:
		// 先处理尾巴的问题
		if (gp->objType == MU_GRID_OBJECT_TYPE_FOOD) {
			snakeTail->shakeTail();
		}
		else {
			snakeTail->getPrev()->turnTail();
			snakeTail->endTail();
			snakeTail->getGrid()->setSnake(nullptr);
		}
		// 再搞头
		sp = new Snake((dir + 2) % 4);  // 别问，问就是数学
		snakeHead->setPrev(sp);
		snakeHead->turnBody(dir);
		sp->setNext(snakeHead);
		snakeHead = sp;
		gp->setSnake(sp);

		if (returnVal == 0) combo++;
	}
	return returnVal;
}

void musnake::Game::run() {
	// 先想想大致的流程吧
	state = MU_GAME_STATE_RUNNING;

	while (state != MU_GAME_STATE_OVER) {
		SDL_Event evt;
		updateTime();
		SDL_RenderClear(gameRender);  // 就硬清！

		while (SDL_PollEvent(&evt)) {
			// 这里要用来处理各种事件了
			// 对此我想到了一种绝妙的解决方法
			// 可惜这里位置太少，写不下 [doge]
			switch (evt.type) {
			case SDL_KEYDOWN:
				switch (evt.key.keysym.sym) {
				case SDLK_UP:
				case SDLK_w:  // 这个死键位先保留着吧，以后开放自行设置键位时再说别的实现方法
					moveSnake(MU_SNAKE_DIRECT_UP);
					break;
				case SDLK_RIGHT:
				case SDLK_d:
					moveSnake(MU_SNAKE_DIRECT_RIGHT);
					break;
				case SDLK_DOWN:
				case SDLK_s:
					moveSnake(MU_SNAKE_DIRECT_DOWN);
					break;
				case SDLK_LEFT:
				case SDLK_a:
					moveSnake(MU_SNAKE_DIRECT_LEFT);
					break;
				}
				break;
			case SDL_QUIT:
				state = MU_GAME_STATE_OVER;
				musnakeState = MU_STATE_OVER;
				break;  // 正在考虑要不要goto直接出来或return，不然还会遍历剩下的事件。emmm...
			}
		}

		triggerDelayFunc(&timingFunc);

		for (int i = 0;i < 20;i++) {
			for (int j = 0;j < 15;j++) {
				gameMap[i][j]->update();
				gameMap[i][j]->draw(gameRender);
			}
		}
		snakeTail->update();
		snakeTail->draw(gameRender);  // 为了保证蛇尾即使处于消失阶段也能正确绘制，在这里调一下它

		SDL_RenderPresent(gameRender);
	}
}
