#pragma once

#include <random>

#include "SDL.h"
#include "SDL_image.h"
#include "SDL_mixer.h"

#include "MU_declaration.h"
#include "MU_note.h"
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

	// 生成食物
	void refreshFood();

	// 为小蛇解移动锁
	void unlockMoving();

	// 开播BGM
	void playBGM();
	
	// 游戏运行的小主函数
	void run();

	// 刷新延时函数和Note的时延值，在开局和结束暂停时调用
	void refreshTime(int delta);

	void init(char* levelname);

	void draw();

	void drawText(char* text, int x, int y, int size);

private:
	char level[32] = "level\\";
	int hp = 5;  // 蛇的血量，初始为5
	Note* note = nullptr;  // 节拍
	Food* food = nullptr;  // 食物
	Mix_Music* bgm = nullptr;  // BGM
	bool movingLock = false;  // 为实现Moves per Second限制而加的移动锁
	unsigned int combo = 0;  // 连击数
	unsigned int score = 0;  // 得分
	unsigned long long pausingTime = 0;  // 暂停时的时间值
	SDL_Rect drawRect;  // 当前屏幕绘制区域对应的完整地图的矩形
	Snake* snakeHead, * snakeTail;  // 蛇头和蛇尾
	DelayFunc* timingFunc = nullptr;  // 当局游戏用的延时函数表（注意实现暂停效果时翻新时间）
};

musnake::Game::Game() {

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

	clearDelayFunc(&timingFunc);

	Mix_HaltMusic();
	Mix_FreeMusic(bgm);
}

void musnake::Game::init(char* levelname){
	// 按说这里应该是从配置文件里读数据来初始化地格的，现阶段就先写死吧
	SDL_Surface* picSurf, * tmpSurf;
	SDL_Texture* tmpTex;

	SDL_strlcat(level, levelname, 25);

	state = MU_GAME_STATE_LOADING;

	// 这里定位文件的位置
	char tmpPath[256];

	// 装载Note图
	catPath(tmpPath, (char*)"image\\notesign.png");
	picSurf = IMG_Load(tmpPath);
	tmpTex = SDL_CreateTextureFromSurface(gameRender, picSurf);
	SDL_FreeSurface(picSurf);
	notesignFlame[0] = new Flame(tmpTex, -1);
	notesignFlame[0]->setNext(nullptr);

	// 装载血条图
	catPath(tmpPath, (char*)"image\\hp.png");
	picSurf = IMG_Load(tmpPath);
	for (int i = 0;i < 6;i++) {
		SDL_Rect srect = { i * 20, 0, 20, 20 };

#if SDL_BYTEORDER == SDL_BIG_ENDIAN
		tmpSurf = SDL_CreateRGBSurface(0, 20, 20, 32, 0xff000000, 0x00ff0000, 0x0000ff00, 0x000000ff);
#else
		tmpSurf = SDL_CreateRGBSurface(0, 20, 20, 32, 0x000000ff, 0x0000ff00, 0x00ff0000, 0xff000000);
#endif
		SDL_BlitSurface(picSurf, &srect, tmpSurf, NULL);
		tmpTex = SDL_CreateTextureFromSurface(gameRender, tmpSurf);
		SDL_FreeSurface(tmpSurf);
		hpFlame[i / 2][i & 1] = new Flame(tmpTex, -1);
		hpFlame[i / 2][i & 1]->setNext(nullptr);
	}

	// 装载食物图
	catPath(tmpPath, (char*)"image\\food_0.png");
	picSurf = IMG_Load(tmpPath);
	tmpTex = SDL_CreateTextureFromSurface(gameRender, picSurf);
	SDL_FreeSurface(picSurf);
	foodFlame[0] = new Flame(tmpTex, -1);
	foodFlame[0]->setNext(nullptr);

	// 开始装载字符图
	catPath(tmpPath, (char*)"image\\char.png");
	picSurf = IMG_Load(tmpPath);
	for (int i = 0;i < 6;i++) {  // 6行文字排列
		for (int j = 0;j < 16;j++) {  // 每行16个字符
			SDL_Rect srect = { j * 80, i * 160, 80, 160 };

#if SDL_BYTEORDER == SDL_BIG_ENDIAN
			tmpSurf = SDL_CreateRGBSurface(0, 80, 160, 32, 0xff000000, 0x00ff0000, 0x0000ff00, 0x000000ff);
#else
			tmpSurf = SDL_CreateRGBSurface(0, 80, 160, 32, 0x000000ff, 0x0000ff00, 0x00ff0000, 0xff000000);
#endif

			SDL_BlitSurface(picSurf, &srect, tmpSurf, NULL);
			tmpTex = SDL_CreateTextureFromSurface(gameRender, tmpSurf);
			SDL_FreeSurface(tmpSurf);
			charFlame[16 * i + j] = new Flame(tmpTex, -1);
		}
	}
	SDL_FreeSurface(picSurf);

	// 开始装载蛇头图，有向蛇头对应的枚举从14号（MU_SNAKE_FLAME_HEAD_0toUP）开始，29号结束
	catPath(tmpPath, (char*)"image\\snake_0_head.png");
	picSurf = IMG_Load(tmpPath);
	for (int i = 0;i < 16;i++) {  // 16种蛇头运动情况
		Flame* flames[8];
		for (int j = 0;j < 8;j++) {  // 每种运动情况有8帧
			SDL_Rect srect = { j * 20, i * 20, 20, 20 };

#if SDL_BYTEORDER == SDL_BIG_ENDIAN
			tmpSurf = SDL_CreateRGBSurface(0, 20, 20, 32, 0xff000000, 0x00ff0000, 0x0000ff00, 0x000000ff);
#else
			tmpSurf = SDL_CreateRGBSurface(0, 20, 20, 32, 0x000000ff, 0x0000ff00, 0x00ff0000, 0xff000000);
#endif

			SDL_BlitSurface(picSurf, &srect, tmpSurf, NULL);
			tmpTex = SDL_CreateTextureFromSurface(gameRender, tmpSurf);
			SDL_FreeSurface(tmpSurf);
			flames[j] = new Flame(tmpTex, 10);
			flames[j]->setGroupId(MU_SNAKE_FLAME_HEAD_0toUP + i);
			if (j)flames[j - 1]->setNext(flames[j]);
		}
		snakeFlame[MU_SNAKE_FLAME_HEAD_0toUP + i] = flames[0];
	}
	SDL_FreeSurface(picSurf);

	// 开始装载蛇尾图，有向蛇尾对应的枚举从30号（MU_SNAKE_FLAME_TAIL_UPshake）开始，49号结束
	catPath(tmpPath, (char*)"image\\snake_0_tail.png");
	picSurf = IMG_Load(tmpPath);
	for (int i = 0;i < 20;i++) {  // 20种有向蛇尾的情况
		Flame* flames[8];
		for (int j = 0;j < 8;j++) {
			SDL_Rect srect = { j * 20, i * 20, 20, 20 };

#if SDL_BYTEORDER == SDL_BIG_ENDIAN
			tmpSurf = SDL_CreateRGBSurface(0, 20, 20, 32, 0xff000000, 0x00ff0000, 0x0000ff00, 0x000000ff);
#else
			tmpSurf = SDL_CreateRGBSurface(0, 20, 20, 32, 0x000000ff, 0x0000ff00, 0x00ff0000, 0xff000000);
#endif

			SDL_BlitSurface(picSurf, &srect, tmpSurf, NULL);
			tmpTex = SDL_CreateTextureFromSurface(gameRender, tmpSurf);
			SDL_FreeSurface(tmpSurf);
			flames[j] = new Flame(tmpTex, 10);
			flames[j]->setGroupId(MU_SNAKE_FLAME_TAIL_UPshake + i);
			if (j)flames[j - 1]->setNext(flames[j]);
		}
		snakeFlame[MU_SNAKE_FLAME_TAIL_UPshake + i] = flames[0];
	}
	SDL_FreeSurface(picSurf);

	// 开始装载一般蛇体图，一般蛇体对应的枚举从0号（MU_SNAKE_FLAME_HEAD_UP）开始，13号结束
	catPath(tmpPath, (char*)"image\\snake_0_body.png");
	picSurf = IMG_Load(tmpPath);
	for (int j = MU_SNAKE_FLAME_HEAD_UP;j <= MU_SNAKE_FLAME_TAIL_LEFT;j++) {
		SDL_Rect srect = { j * 20, 0, 20, 20 };

#if SDL_BYTEORDER == SDL_BIG_ENDIAN
		tmpSurf = SDL_CreateRGBSurface(0, 20, 20, 32, 0xff000000, 0x00ff0000, 0x0000ff00, 0x000000ff);
#else
		tmpSurf = SDL_CreateRGBSurface(0, 20, 20, 32, 0x000000ff, 0x0000ff00, 0x00ff0000, 0xff000000);
#endif

		SDL_BlitSurface(picSurf, &srect, tmpSurf, NULL);
		tmpTex = SDL_CreateTextureFromSurface(gameRender, tmpSurf);
		SDL_FreeSurface(tmpSurf);
		snakeFlame[j] = new Flame(tmpTex, -1);
		snakeFlame[j]->setGroupId(j);
	}
	for (int j = MU_SNAKE_FLAME_BODY_UPDOWN;j <= MU_SNAKE_FLAME_BODY_UPLEFT;j++) {
		SDL_Rect srect = { (j - MU_SNAKE_FLAME_BODY_UPDOWN) * 20, 20, 20, 20 };

#if SDL_BYTEORDER == SDL_BIG_ENDIAN
		tmpSurf = SDL_CreateRGBSurface(0, 20, 20, 32, 0xff000000, 0x00ff0000, 0x0000ff00, 0x000000ff);
#else
		tmpSurf = SDL_CreateRGBSurface(0, 20, 20, 32, 0x000000ff, 0x0000ff00, 0x00ff0000, 0xff000000);
#endif

		SDL_BlitSurface(picSurf, &srect, tmpSurf, NULL);
		tmpTex = SDL_CreateTextureFromSurface(gameRender, tmpSurf);
		SDL_FreeSurface(tmpSurf);
		snakeFlame[j] = new Flame(tmpTex, -1);
		snakeFlame[j]->setGroupId(j);
	}

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

	// 关卡路径
	char levelfile[64];
	// 装载关卡音乐
	SDL_strlcpy(levelfile, level, 64);
	SDL_strlcat(levelfile, "\\bgm.mp3", 64);
	catPath(tmpPath, levelfile);
	bgm = Mix_LoadMUS(tmpPath);

	// 装载关卡的节拍
	SDL_strlcpy(levelfile, level, 64);
	SDL_strlcat(levelfile, "\\notes.mu", 64);
	catPath(tmpPath, levelfile);
	SDL_RWops* f = SDL_RWFromFile(tmpPath, "r");
	char nts[16], * ntp = nts;
	char c = 1;
	Note* endn = nullptr;
	while (SDL_RWread(f, &c, 1, 1)) {
		if (c != ';') {
			*ntp = c;
			ntp++;
		}
		else {
			long long nt = 0;
			unsigned long p = 1;
			do {
				ntp--;
				nt += (*ntp - '0') * p;
				p *= 10;
			} while (ntp != nts);

			if (!endn) {
				endn = note = newNote(nt);
			}
			else {
				endn->next = newNote(nt);
				endn = endn->next;
			}
		}
	}

	for (int i = 0;i < 20;i++) {
		for (int j = 0;j < 15;j++) {
			Grid* map = gameMap[i][j] = new Grid(i, j);
			map->setPosition(i * 40, j * 40, 40, 40);
			if (i == 0 || i == 19 || j == 0 || j == 14)
				map->objType = MU_GRID_OBJECT_TYPE_BLOCK;
			else 
				map->objType = MU_GRID_OBJECT_TYPE_EMPTY;
		}
	}

	// 特化一下我们的小蛇

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

	sp[0]->setFlame(snakeFlame[MU_SNAKE_FLAME_HEAD_LEFT]);
	sp[1]->setFlame(snakeFlame[MU_SNAKE_FLAME_BODY_RIGHTLEFT]);
	sp[2]->setFlame(snakeFlame[MU_SNAKE_FLAME_BODY_RIGHTLEFT]);
	sp[3]->setFlame(snakeFlame[MU_SNAKE_FLAME_TAIL_RIGHT]);
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

inline void musnake::Game::playBGM() {
	if(bgm) Mix_PlayMusic(bgm, 1);
}

void unlockMoving_D(unsigned long arg) {
	musnake::thisGame->unlockMoving();
}

int musnake::Game::moveSnake(int dir) {
	Grid* gp = snakeHead->getGrid();
	Snake* sp;
	int returnVal = 0;
	int x = gp->x, y = gp->y;

	if (movingLock) {  // 如果超出了10MPS的限制，则判定为手抖
		return 1;
	}
	else {
		movingLock = true;
		setDelayFunc(&unlockMoving_D, 0, 100);  // 0.1秒后自解开
	}

	if (dir == snakeHead->getTailDir()) {  // 返向
		combo = 0;
		dir = (dir + 2) % 4;
		returnVal = 1;
	}

	if (note->time > (long long)getTimeVal()) {  // 当前没有音符
		combo = 0;
		returnVal = 1;
	}
	else if (dir == MU_SNAKE_DIRECT_NONE) {  // 一个音符超过0.2秒没有按就视为miss
		combo = 0;
		returnVal = 1;
		dir = (snakeHead->getTailDir() + 2) % 4;
		Note* np = note;
		note = np->next;
		delete np;
	}
	else {  // 正常击中音符
		Note* np = note;
		note = np->next;
		delete np;
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
			hp -= 3;  // 算了，改简单一些，就扣3血
			if (hp < 0) hp = 0;
			combo = 0;
			return 1;
		}  // 如果所指为蛇尾，接着下一个case开始生头并缩尾
	case MU_GRID_OBJECT_TYPE_EMPTY:
	case MU_GRID_OBJECT_TYPE_FOOD:
		// 先处理尾巴的问题
		if (gp->objType == MU_GRID_OBJECT_TYPE_FOOD) {
			if (hp < 10) hp += 1;
			delete gp->getFood();
			gp->setFood(nullptr);
			food = nullptr;
			score += 100;
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

void musnake::Game::refreshFood() {
	while (true) {
		int x = Rander() % 20, y = Rander() % 15;  // 这种roll方式肯定要改的，不然到后期很难roll到空的就很不妙
		if (gameMap[x][y]->objType == MU_GRID_OBJECT_TYPE_EMPTY) {
			food = new Food;
			food->setFlame(foodFlame[0]);
			gameMap[x][y]->setFood(food);
			break;
		}
	}
}

void musnake::Game::refreshTime(int delta) {
	long long dt = getTimeVal() - pausingTime + delta;
	DelayFunc* dfp = timingFunc;
	Note* np = note;

	while (dfp) {  // 开局时应该不会有游戏当局延时函数吧
		dfp->time += dt;
		dfp = dfp->next;
	}

	if (!pausingTime) dt += noteDelta;  // 对于run开始的那次刷新，加上偏移
	while (np) {
		np->time += dt;
		np = np->next;
	}
}

void playBGM_D(unsigned long arg) {
	musnake::thisGame->playBGM();
}

void musnake::Game::run() {
	SDL_Rect r = { 0, 0, 20, 20 };
	// 先想想大致的流程吧
	state = MU_GAME_STATE_RUNNING;
	updateTime();
	refreshTime(2000);
	movingLock = true;
	setDelayFunc(&unlockMoving_D, 0, 1500);
	setDelayFunc(&playBGM_D, 0, 2000);

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
					if (!moveSnake(MU_SNAKE_DIRECT_UP)) score += 10 + combo / 10;
					break;
				case SDLK_RIGHT:
				case SDLK_d:
					if (!moveSnake(MU_SNAKE_DIRECT_RIGHT)) score += 10 + combo / 10;
					break;
				case SDLK_DOWN:
				case SDLK_s:
					if (!moveSnake(MU_SNAKE_DIRECT_DOWN)) score += 10 + combo / 10;
					break;
				case SDLK_LEFT:
				case SDLK_a:
					if (!moveSnake(MU_SNAKE_DIRECT_LEFT)) score += 10 + combo / 10;
					break;
				}
				break;
			case SDL_QUIT:
				state = MU_GAME_STATE_OVER;
				musnakeState = MU_STATE_OVER;
				break;  // 正在考虑要不要goto直接出来或return，不然还会遍历剩下的事件。emmm...
			}
		}

		if ((long long)getTimeVal() - note->time > 300) moveSnake(MU_SNAKE_DIRECT_NONE);
		// 事实证明，该转long long的地方保持unsigned，会出现贼鸡儿奇葩的BUG

		triggerDelayFunc(&timingFunc);

		if (!food)
			refreshFood();

		draw();

		SDL_RenderPresent(gameRender);
	}
}

void musnake::Game::draw() {
	// 先绘制地图上的内容
	if (food) food->draw(gameRender);

	for (int i = 0;i < 20;i++) {
		for (int j = 0;j < 15;j++) {
			gameMap[i][j]->update();
			gameMap[i][j]->draw(gameRender);
		}
	}
	snakeTail->update();
	snakeTail->draw(gameRender);  // 为了保证蛇尾即使处于消失阶段也能正确绘制，在这里调一下它

	// 再绘制GUI内容
	// 坚定了我重构时要写基类的决心
	
	// 下面绘制血条
	SDL_Rect hpRect[5] = {
		{800 - 210, 10, 40, 40},
		{800 - 170, 10, 40, 40},
		{800 - 130, 10, 40, 40},
		{800 -  90, 10, 40, 40},
		{800 -  50, 10, 40, 40},
	};  // hp显示
	int fg = movingLock ? 1 : 0;
	for (int i = 0;i < 5;i++) {
		if (i < hp / 2) {
			hpFlame[0][fg]->draw(gameRender, &hpRect[i]);
		}
		else if (i * 2 + 1 == hp) {
			hpFlame[1][fg]->draw(gameRender, &hpRect[i]);
		}
		else {
			hpFlame[2][fg]->draw(gameRender, &hpRect[i]);
		}
	}

	// 下面绘制Note
	Note* np = note;
	while (np) {
		int dt;
		if ((dt = np->time - getTimeVal()) > 1500) break;
		dt = (dt + 500) * 800 / 2000;
		SDL_Rect r = { dt, 520, 5, 60 };
		notesignFlame[0]->draw(gameRender, &r);
		np = np->next;
	}
	
	// 绘制得分
	char ss[16] = { 0 };  // 我就不信有人能打超15位数的分了！
	drawText((char*)"score:", 10, 10, 10);
	int2str(ss, score);
	drawText(ss, 10, 23, 20);

	// 绘制连击数
	if (combo >= 2) {
		int2str(ss, combo);
		SDL_strlcat(ss, " hits!", 16);
		drawText(ss, 10, 440, 20);
	}
}

void musnake::Game::drawText(char* text, int x, int y, int size) {
	char* cp = text;
	while (*cp) {
		SDL_Rect r = { x, y, size, size * 2 };
		charFlame[*cp - 32]->draw(gameRender, &r);
		cp++;
		x += size;
	}
}

void discardTail(unsigned long arg) {
	using namespace musnake;
	Snake* snake = (Snake*)arg;
	thisGame->setSnakeTail(snake->getPrev());
	snake->getPrev()->setNext(nullptr);
	delete snake;
}

inline void musnake::Snake::endTail() {
	switch (headDir) {
	case MU_SNAKE_DIRECT_UP:
		setFlame(snakeFlame[MU_SNAKE_FLAME_TAIL_UPto0]);
		break;
	case MU_SNAKE_DIRECT_RIGHT:
		setFlame(snakeFlame[MU_SNAKE_FLAME_TAIL_RIGHTto0]);
		break;
	case MU_SNAKE_DIRECT_DOWN:
		setFlame(snakeFlame[MU_SNAKE_FLAME_TAIL_DOWNto0]);
		break;
	case MU_SNAKE_DIRECT_LEFT:
		setFlame(snakeFlame[MU_SNAKE_FLAME_TAIL_LEFTto0]);
		break;
	}
	setHeadDir(MU_SNAKE_DIRECT_NONE);

	musnake::thisGame->setDelayFunc(&discardTail, (unsigned long)this, 90);  // 展示完效果就赶紧GG
}
