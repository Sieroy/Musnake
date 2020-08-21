#pragma once

#include <random>
#include <math.h>

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
	Snake* snakeTail = nullptr;
	DelayFunc* timingFunc = nullptr;  // ������Ϸ�õ���ʱ��������ע��ʵ����ͣЧ��ʱ����ʱ�䣩
	DelayFunc* toastQueue = nullptr;
	bool playing = false;

	void setRenderer(SDL_Renderer* render);
	void setDelayFunc(void (*func)(unsigned long), unsigned long arg, int delay);
	void setSnakeHead(Snake* snake);
	void setSnakeTail(Snake* snake);

	// ΪС�߽��ƶ���
	void unlockMoving();

	// ����BGM
	void playBGM(int loop);

	void init(Level* lp);

	// ��Ϸ���е�С������
	void run();

	// ��ͣʱ����
	void pause();

	// ˢ����ʱ������Note��ʱ��ֵ���ڿ��ֺͽ�����ͣʱ����
	void initTime(int delta);
	void refreshTime(int delta);


private:
	char levelPath[32] = "level\\";
	char rankVal;
	char rankStr[4];
	unsigned int combo = 0;  // ������
	unsigned short noteCount = 0;  // �ܵ�������
	unsigned short badCount = 0;  // 对于只会冲冲冲不会踩节奏的人的评级惩罚
	unsigned int score = 0;  // �÷�
	short hp = 5;  // �ߵ�Ѫ������ʼΪ5
	unsigned int hits = 0;  // ���е�������
	short fever = 0;  // FEVER״̬��2���÷�
	unsigned int length = 4;  // һ����Ϸ��65531��ƻ�����Ǿ����NB�ˣ�����0.1s���ƶ������⼴ʹ��ŷ������ҲҪ���ϸ���Сʱ

	int interval = 0;

	Level* levelinfo = nullptr;
	Note* note = nullptr;  // ����
	Food* food[3] = { nullptr };  // ʳ��
	Mix_Music* bgm = nullptr;  // BGM
	bool movingLock = false;  // Ϊʵ��Moves per Second���ƶ��ӵ��ƶ���
	unsigned long long pausingTime = 0;  // ��ͣʱ��ʱ��ֵ
	SDL_Rect drawRect;  // ��ǰ��Ļ���������Ӧ��������ͼ�ľ���
	Snake* snakeHead = nullptr;  // ��ͷ����β
	SDL_Point base = { 0, 0 };

	bool pointInWindow(SDL_Point* p);

	// ���ߺ�������������ƶ����ͷ���0�����򷵻�1���ں�����ʵ�ֿ�Ѫ֮��Ĳ���
	// �����ƶ������Ž������β��ķ��򡢳ɹ����ƶ���
	// �������ƶ����������ƶ���β���ƶ���ײǽ��ײ����
	int moveSnake(int dir);

	// ����ʳ��
	void refreshFood(int index);

	void drawGame();

	void drawUI();

	void drawFoodPointer(int index);

	void loadMap();

	void loadToast();

	void unloadToast();

	void updateBase();
};

musnake::Game::Game() {

}

musnake::Game::~Game() {

	// �ȱ����ţ���ΪĿǰ���ǲ��Խ׶Σ�һ�����еĻ�û��ж�صظ�֮��ı�Ҫ����������ص��ͺ�~
	// �Ȱѵظ���ͷ�д�˰�
	for (int i = 0;i < 64;i++) {
		for (int j = 0;j < 64;j++) {
			delete gameMap[i][j];
		}
	}
	// �ٰ��ߵ��ͷż��ϰɣ������ߵİ�ݹ�����Ӧ�ò���
	delete snakeHead;
	delete snakeTail;
	if (food[0]) delete food[0];
	if (food[1]) delete food[1];
	if (food[2]) delete food[2];

	clearDelayFunc(&timingFunc);
	unloadToast();
	clearNotes(&note);

	Mix_HaltMusic();
	Mix_FreeMusic(bgm);
}

void musnake::Game::init(Level* lp){
	state = MU_GAME_STATE_LOADING;

	levelinfo = lp;
	SDL_strlcat(levelPath, lp->id, 25);

	// ���ﶨλ�ļ���λ��
	char tmpPath[256];

	// �ؿ�·��
	char levelfile[64];
	// װ�عؿ�����
	SDL_strlcpy(levelfile, levelPath, 64);
	SDL_strlcat(levelfile, "\\bgm.mp3", 64);
	catPath(tmpPath, levelfile);
	bgm = Mix_LoadMUS(tmpPath);

	if (lp->timev) {
		// װ�عؿ��Ľ���
		SDL_strlcpy(levelfile, levelPath, 64);
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
				noteCount++;
			}
		}
		SDL_RWclose(f);
	}
	else {
		Note* nnp = new Note;
		nnp->time = 0;
		nnp->next = nullptr;
		interval = lp->interval;
		addNote(&note, nnp);
	}

	// ��ʼ���ؿ�
	for (int i = 0;i < 64;i++) {
		for (int j = 0;j < 64;j++) {
			Grid* map = gameMap[i][j] = new Grid(i, j);
			map->setFlame(gridFlame);
			if (i == 0 || i == 63 || j == 0 || j == 63)
				map->objType = MU_GRID_OBJECT_TYPE_DARK;
			else 
				map->objType = MU_GRID_OBJECT_TYPE_EMPTY;
		}
	}

	// �ػ�һ�����ǵ�С��

	// Snake* sp[4];
	// gameMap[7][3]->setSnake(sp[0] = new Snake);
	// sp[0]->setTailDir(MU_SNAKE_DIRECT_LEFT);
	// gameMap[6][3]->setSnake(sp[1] = new Snake);
	// sp[1]->setHeadDir(MU_SNAKE_DIRECT_RIGHT);
	// sp[1]->setTailDir(MU_SNAKE_DIRECT_LEFT);
	// gameMap[5][3]->setSnake(sp[2] = new Snake);
	// sp[2]->setHeadDir(MU_SNAKE_DIRECT_RIGHT);
	// sp[2]->setTailDir(MU_SNAKE_DIRECT_LEFT);
	// gameMap[4][3]->setSnake(sp[3] = new Snake);
	// sp[3]->setHeadDir(MU_SNAKE_DIRECT_RIGHT);

	// setSnakeHead(sp[0]);
	// sp[0]->setNext(sp[1]);
	// sp[1]->setNext(sp[2]);
	// sp[2]->setNext(sp[3]);
	// setSnakeTail(sp[3]);
	// sp[3]->setPrev(sp[2]);
	// sp[2]->setPrev(sp[1]);
	// sp[1]->setPrev(sp[0]);

	// sp[0]->setFlame(snakeFlame[MU_SNAKE_FLAME_HEAD_LEFT]);
	// sp[1]->setFlame(snakeFlame[MU_SNAKE_FLAME_BODY_RIGHTLEFT]);
	// sp[2]->setFlame(snakeFlame[MU_SNAKE_FLAME_BODY_RIGHTLEFT]);
	// sp[3]->setFlame(snakeFlame[MU_SNAKE_FLAME_TAIL_RIGHT]);
	loadMap();
	loadToast();
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

inline bool musnake::Game::pointInWindow(SDL_Point* p) {
	SDL_Point bp = { p->x + base.x,p->y + base.y };
	return SDL_PointInRect(&bp, &musnakeRect);
}

inline void musnake::Game::unlockMoving() {
	movingLock = false;
}

inline void musnake::Game::playBGM(int loop) {
	if(bgm) Mix_PlayMusic(bgm, loop);
}

void unlockMoving_D(unsigned long arg) {
	musnake::thisGame->unlockMoving();
}

void discardTail(unsigned long arg) {
	using namespace musnake;
	Snake* snake = (Snake*)arg;
	thisGame->setSnakeTail(snake->getPrev());
	snake->getPrev()->setNext(nullptr);
	delete snake;
}

int musnake::Game::moveSnake(int dir) {
	Grid* gp = snakeHead->getGrid();
	Snake* sp;
	int returnVal = 0;
	int x = gp->x, y = gp->y;

	if (movingLock) {  // ���������10MPS�����ƣ����ж�Ϊ�ֶ�
		removeDelayFuncByFuncArg(&timingFunc, &unlockMoving_D, 0);
		if (!removeDelayFuncByFuncArg(&timingFunc, &discardTail, (unsigned long)snakeTail)) {
			Snake* sp = snakeTail->getPrev();
			delete snakeTail;
			snakeTail = sp;
		}
		setDelayFunc(&unlockMoving_D, 0, 78);  // 0.085����Խ⿪
	}
	else {
		movingLock = true;
		setDelayFunc(&unlockMoving_D, 0, 78);  // 0.085����Խ⿪
	}

	if (dir == snakeHead->getTailDir()) {  // ����
		combo = 0;
		dir = (dir + 2) % 4;
		returnVal = 1;
	}

	if (!playing || !note || note->time > (long long)getTimeVal()) {  // ��ǰû������
		combo = 0;
		returnVal = 1;
	}
	else if (dir == MU_SNAKE_DIRECT_NONE) {  // һ����������0.2��û�а�����Ϊmiss
		combo = 0;
		returnVal = 1;
		dir = (snakeHead->getTailDir() + 2) % 4;
		Note* np = note;
		if (interval > 0) {
			Note* nnp = new Note;
			nnp->time = np->time + interval;
			nnp->next = nullptr;
			addNote(&note, nnp);
		}
		note = np->next;
		delete np;
	}
	else {  // ������������
		Note* np = note;
		if (interval > 0) {
			Note* nnp = new Note;
			nnp->time = np->time + interval;
			nnp->next = nullptr;
			addNote(&note, nnp);
		}
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
		if (gp->getSnake() != snakeTail) {  // �����ָΪ��������ʼ�˺��ж���ȡ���˴��ƶ�
	case MU_GRID_OBJECT_TYPE_BLOCK:  // ���ǰ�����ϰ�����ͬ
	case MU_GRID_OBJECT_TYPE_DARK:
		hp -= interval > 0 ? 10 : 3;  // ���ˣ��ļ�һЩ���Ϳ�3Ѫ
			if (hp < 0) hp = 0;
			combo = 0;
			return 1;
		}  // �����ָΪ��β��������һ��case��ʼ��ͷ����β
		if (gp->objType == MU_GRID_OBJECT_TYPE_FOOD) {
	case MU_GRID_OBJECT_TYPE_FOOD:
			length += 1;
			if (hp < 10) hp += 1;
			else fever = 4 + length / 10;  // ֻҪ�߹������������ϣ���ôÿһ������FEVER��~
			Food* fp = gp->getFood();
			gp->setFood(nullptr);
			if (fp == food[0]) food[0] = nullptr;
			else if (fp == food[1]) food[1] = nullptr;
			else if (fp == food[2]) food[2] = nullptr;
			delete fp;
			score += fever > 0 ? 200 : 100;
			snakeTail->shakeTail();
		}
		else {
	case MU_GRID_OBJECT_TYPE_EMPTY:
			snakeTail->getPrev()->turnTail();
			snakeTail->endTail();
			snakeTail->getGrid()->setSnake(nullptr);
		}
		// �ٸ�ͷ
		sp = new Snake((dir + 2) % 4);  // ���ʣ��ʾ�����ѧ
		snakeHead->setPrev(sp);
		snakeHead->turnBody(dir);
		sp->setNext(snakeHead);
		snakeHead = sp;
		gp->setSnake(sp);

		if (returnVal == 0) combo++;
	}
	return returnVal;
}

inline void musnake::Game::updateBase() {
	static int dc = 0;
	int x, y;
	x = snakeHead->getGrid()->x;
	y = snakeHead->getGrid()->y;
	if (x * 40 + base.x > 600) {
		dc += getTimeDelta();
		while (dc >= 5) {
			base.x -= 2;
			dc -= 5;
		}
	}
	else if (x * 40 + base.x < 160) {
		dc += getTimeDelta();
		while (dc >= 5) {
			base.x += 2;
			dc -= 5;
		}
	}
	else if (y * 40 + base.y > 400) {
		dc += getTimeDelta();
		while (dc >= 5) {
			base.y -= 2;
			dc -= 5;
		}
	}
	else if (y * 40 + base.y < 160) {
		dc += getTimeDelta();
		while (dc >= 5) {
			base.y += 2;
			dc -= 5;
		}
	}
	else
		dc = 0;
}

inline void musnake::Game::refreshFood(int index) {
	while (true) {
		int x = Rander() % 62 + 1, y = Rander() % 62 + 1;  // ����roll��ʽ�϶�Ҫ�ĵģ���Ȼ�����ں���roll���յľͺܲ���
		if (gameMap[x][y]->objType == MU_GRID_OBJECT_TYPE_EMPTY) {
			food[index] = new Food(index);
			gameMap[x][y]->setFood(food[index]);
			break;
		}
	}
}

void musnake::Game::initTime(int delta) {
	long long dt = getTimeVal() + delta + noteDelta;
	DelayFunc* dfp = timingFunc;
	Note* np = note;

	while (dfp) {  // ����ʱӦ�ò�������Ϸ������ʱ������
		dfp->time += delta;
		dfp = dfp->next;
	}

	dfp = toastQueue;
	while (dfp) {
		dfp->time += delta;
		dfp = dfp->next;
	}

	while (np) {
		np->time += dt;
		np = np->next;
	}
}

inline void musnake::Game::refreshTime(int delta) {
	long long dt = (long long)(getTimeVal() - pausingTime) + delta;
	DelayFunc* dfp = timingFunc;
	Note* np = note;

	while (dfp) {  // ����ʱӦ�ò�������Ϸ������ʱ������
		dfp->time += dt;
		dfp = dfp->next;
	}

	dfp = toastQueue;
	while (dfp) {  // ����ʱӦ�ò�������Ϸ������ʱ������
		dfp->time += dt;
		dfp = dfp->next;
	}
	

	if (!pausingTime) dt += noteDelta;  // ����run��ʼ���Ǵ�ˢ�£�����ƫ��
	while (np) {
		np->time += dt;
		np = np->next;
	}

	if (pausingTime) {
		Snake* sp = snakeHead;
		dt = getTimeVal() - pausingTime;

		sp->delayFlameTime(dt);
		do {
			if (!sp) break;
			sp = sp->getNext();
			sp->delayFlameTime(dt);
		} while (sp != snakeTail);
	}
}

void playBGM_D(unsigned long arg) {
	using namespace musnake;
	thisGame->playBGM((long) arg);
	thisGame->playing = true;
	updateTime();
	thisGame->initTime(0);
}

void passLevel(unsigned long arg) {
	musnake::thisGame->state = musnake::MU_GAME_STATE_OVER;
}

void musnake::Game::run() {
	SDL_Event evt;
	long long timing = 0;
	// ��������µ����̰�
	state = MU_GAME_STATE_RUNNING;
	if (interval <= 0) {
		setDelayFunc(&passLevel, 0, levelinfo->timev + 2000);  // ����������߻�û�����͹���
		setDelayFunc(&playBGM_D, 1, 500);
	}
	else {
		setDelayFunc(&playBGM_D, -1, 500);
	}

	while (state == MU_GAME_STATE_RUNNING) {
		updateTime();
		SDL_RenderClear(gameRender);  // ��Ӳ�壡

		while (SDL_PollEvent(&evt)) {
			// ����Ҫ�������������¼���
			// �Դ����뵽��һ�־���Ľ������
			// ��ϧ����λ��̫�٣�д���� [doge]
			switch (evt.type) {
			case SDL_KEYDOWN:
				switch (evt.key.keysym.sym) {
				case SDLK_UP:__keyup:
					if (!moveSnake(MU_SNAKE_DIRECT_UP)) {
						hits++;
						if (fever > 0) {
							score += (10 + combo / 10) * 2;
							fever--;
						}
						else
							score += (10 + combo / 10);
					}
					else {
						badCount++;
						fever = 0;
					}
					break;
				case SDLK_RIGHT:__keyright:
					if (!moveSnake(MU_SNAKE_DIRECT_RIGHT)) {
						hits++;
						if (fever > 0) {
							score += (10 + combo / 10) * 2;
							fever--;
						}
						else
							score += (10 + combo / 10);
					}
					else {
						badCount++;
						fever = 0;
					}
					break;
				case SDLK_DOWN:__keydown:
					if (!moveSnake(MU_SNAKE_DIRECT_DOWN)) {
						hits++;
						if (fever > 0) {
							score += (10 + combo / 10) * 2;
							fever--;
						}
						else
							score += (10 + combo / 10);
					}
					else {
						badCount++;
						fever = 0;
					}
					break;
				case SDLK_LEFT:__keyleft:
					if (!moveSnake(MU_SNAKE_DIRECT_LEFT)) {
						hits++;
						if (fever > 0) {
							score += (10 + combo / 10) * 2;
							fever--;
						}
						else
							score += (10 + combo / 10);
					}
					else {
						badCount++;
						fever = 0;
					}
					break;
				case SDLK_ESCAPE:
					state = MU_GAME_STATE_PAUSED;
					pause();
					break;
				default:
					if (evt.key.keysym.sym == musnakeKey[MU_KEY_UP]) goto __keyup;
					if (evt.key.keysym.sym == musnakeKey[MU_KEY_DOWN]) goto __keydown;
					if (evt.key.keysym.sym == musnakeKey[MU_KEY_LEFT]) goto __keyleft;
					if (evt.key.keysym.sym == musnakeKey[MU_KEY_RIGHT]) goto __keyright;
				}
				break;
			case SDL_WINDOWEVENT:
				switch (evt.window.event) {
				case SDL_WINDOWEVENT_MOVED:
				case SDL_WINDOWEVENT_FOCUS_LOST:  // 2345�������~Ŷ���군~
				case SDL_WINDOWEVENT_HIDDEN:
					state = MU_GAME_STATE_PAUSED;
					pause();
					break;
				}
				break;
			case SDL_QUIT:
				state = MU_GAME_STATE_END;
				musnakeState = MU_STATE_OVER;
				break;  // ���ڿ���Ҫ��Ҫgotoֱ�ӳ�����return����Ȼ�������ʣ�µ��¼���emmm...
			}
		}

		if (playing && note && ((long long)getTimeVal() - note->time > 300)) moveSnake(MU_SNAKE_DIRECT_NONE);
		// ��ʵ֤������תlong long�ĵط�����unsigned������������������BUG

		triggerDelayFunc(&timingFunc);

		updateBase();

		if (hp <= 0) {
			state = MU_GAME_STATE_OVER;
			Mix_FadeOutMusic(1000);
		}

		if (!food[0])
			refreshFood(0);
		if (!food[1])
			refreshFood(1);
		if (!food[2])
			refreshFood(2);

		drawGame();

		triggerDelayFunc(&toastQueue);

		SDL_RenderPresent(gameRender);
	}

	if (state == MU_GAME_STATE_END) return;

	timing = 0;
	
	int rv, sv, lv;
	char rankStr[4];
	if (hp > 0) {  // 如果胜利，那么存个档
		rankVal = hits * 100 / (noteCount + badCount);
		switch (rankVal / 10) {
		case 10:
			SDL_strlcpy(rankStr, "SSS", 4);
			break;
		case 9:
			if (rankVal >= 95)
				SDL_strlcpy(rankStr, "SS", 3);
			else
				SDL_strlcpy(rankStr, "S", 3);
			break;
		case 8:
			SDL_strlcpy(rankStr, "A", 3);
			break;
		case 7:
			SDL_strlcpy(rankStr, "B", 3);
			break;
		case 6:
			SDL_strlcpy(rankStr, "C", 3);
			break;
		default:
			SDL_strlcpy(rankStr, "D", 3);
		}
		rv = rankVal > userData["record"][levelinfo->id]["rank"].asInt() ? rankVal : -1;
		sv = score > userData["record"][levelinfo->id]["score"].asUInt() ? score : -1;
		lv = length > userData["record"][levelinfo->id]["length"].asInt() ? length : 0;

		updateUserScore(levelinfo->id, rv, sv, lv);
		updateLevelBestFlame(levelinfo);
		flushUserData();
	}
	else if (interval > 0) {
		lv = length > userData["record"][levelinfo->id]["length"].asInt() ? length : 0;

		updateUserScore(levelinfo->id, -1, -1, lv);
		updateLevelBestFlame(levelinfo);
		flushUserData();
	}

	while (state == MU_GAME_STATE_OVER) {
		char ss[32];
		updateTime();
		SDL_RenderClear(gameRender);

		while (SDL_PollEvent(&evt)) {
			switch (evt.type) {
			case SDL_QUIT:
				state = MU_GAME_STATE_END;
				musnakeState = MU_STATE_OVER;
				break;
			case SDL_KEYDOWN:
				switch (evt.key.keysym.sym) {
				case SDLK_RETURN:
				case SDLK_RETURN2:
				case SDLK_KP_ENTER:
				case SDLK_ESCAPE:
					state = MU_GAME_STATE_END;
					musnakeState = MU_STATE_RUNNING;
					break;
				case SDLK_r:
					state = MU_GAME_STATE_END;
					musnakeState = MU_STATE_GAMING;
					break;
				}
				break;
			case SDL_MOUSEBUTTONDOWN:
				if( evt.button.button == SDL_BUTTON_LEFT && hp > 0 && timing > 1400 ){
					SDL_Point point{evt.button.x, evt.button.y};
					SDL_Rect BackButton, RetryButton, OKButton;
					BackButton = { 0,0,150,60 };
					RetryButton = { 0,450,150,60 };
					OKButton = { 0,520,150,60 };
					if( SDL_PointInRect( &point, &BackButton ) ) {
						state = MU_GAME_STATE_END;
						musnakeState = MU_STATE_RUNNING;
					}
					else if( SDL_PointInRect( &point, &RetryButton ) ) {
						state = MU_GAME_STATE_END;
						musnakeState = MU_STATE_GAMING;
					}
					else if( SDL_PointInRect( &point, &OKButton ) ) {
						state = MU_GAME_STATE_END;
						musnakeState = MU_STATE_RUNNING;
					}
				}
				break;
				
			}
		}

		if (hp > 0 || interval > 0) {  // ˳������
			if (timing < 1000) {
				drawGame();
				gamewinBGFlame->draw(gameRender, 0, (int)(600 - timing));
			}
			else {
				int l = timing > 1400 ? 400 : (int)(timing - 1000);
				SDL_Rect prect = { 970 - 2 * l, 0, 610, 600 };
				gamewinBGFlame->draw(gameRender, 0, -400 - (timing / 10 - 100) % 130);
				SDL_RenderFillRect(gameRender, &prect);
				gameOverBackButtonFlame->draw(gameRender, l - 400, 0);
				gameOverRetryButtonFlame->draw(gameRender, l - 400, 450);
				gameOverOKButtonFlame->draw(gameRender, l - 400, 520);

				if (interval <= 0)
					drawText(gameRender, rankStr, 1000 - 2 * l, 100, 80);

				// ���Ƶ÷�
				if (interval > 0) {
					levelinfo->nameFlm->draw(gameRender, 1000 - 2 * l, 140);
					text_TotalLength_Flame->draw(gameRender, 1000 - 2 * l, 200);
					drawNumber(gameRender, numberTotalFlame, length, 1060 - 2 * l, 240);
					if (lv > 0) gamewinNewBestFlame->draw(gameRender, 1060 - 2 * l, 280);
				}
				else {
					levelinfo->nameFlm->draw(gameRender, 1300 - 2 * l, 140);
					text_TotalScore_Flame->draw(gameRender, 1000 - 2 * l, 300);
					text_TotalLength_Flame->draw(gameRender, 1000 - 2 * l, 450);
					drawNumber(gameRender, numberTotalFlame, score, 1060 - 2 * l, 355);
					drawNumber(gameRender, numberTotalFlame, length, 1060 - 2 * l, 505);
					if (rv >= 0) gamewinNewBestFlame->draw(gameRender, 1300 - 2 * l, 200);
					if (sv >= 0) gamewinNewBestFlame->draw(gameRender, 1140 - 2 * l, 300);
					if (lv > 0) gamewinNewBestFlame->draw(gameRender, 1140 - 2 * l, 450);
				}
			}
			timing += getTimeDelta();
		}
		else {  // ����
			drawGame();
			SDL_RenderFillRect(gameRender, NULL);
			drawText(gameRender, (char*)"you", 340, 150, 40);
			drawText(gameRender, (char*)"are", 340, 250, 40);
			drawText(gameRender, (char*)"dead", 280, 350, 60);
		}
		// ����FPS
		drawNumber(gameRender, numberFPSFlame, fps, 750, 580);

		SDL_RenderPresent(gameRender);
	}

	SDL_RenderClear(gameRender);
}

void musnake::Game::pause() {
	static int dt = 0;
	int choosing = 0;
	char fpss[10];  // ��Ҫ���ܳ�9λ����������ԾͿ��������
	SDL_Event evt;

	Mix_PauseMusic();
	pausingTime = getTimeVal();

	while (state == MU_GAME_STATE_PAUSED) {
		SDL_RenderClear(gameRender);
		updateTime();

		while (SDL_PollEvent(&evt)) {
			switch (evt.type) {
			case SDL_QUIT:
				state = MU_GAME_STATE_END;
				musnakeState = MU_STATE_OVER;
				break;
			case SDL_KEYDOWN:
				switch (evt.key.keysym.sym) {
				case SDLK_w:
				case SDLK_UP:
					if (choosing > 0) choosing--;
					break;
				case SDLK_s:
				case SDLK_DOWN:
					if (choosing < 2) choosing++;
					break;
				case SDLK_RETURN:
				case SDLK_RETURN2:
					if (choosing == 0) {  // RESUME
				case SDLK_ESCAPE:
						state = MU_GAME_STATE_RUNNING;
						drawGame();
						SDL_Delay(300);
						break;
					}
					else if (choosing == 1) {  // RETRY
						state = MU_GAME_STATE_END;
					}
					else if (choosing == 2) {  // BACK
						state = MU_GAME_STATE_END;
						musnakeState = MU_STATE_RUNNING;
					}
				}
			case SDL_MOUSEBUTTONDOWN:
				if( evt.button.button == SDL_BUTTON_LEFT){
					SDL_Point point{evt.button.x, evt.button.y};
					SDL_Rect ResumeButton, RetryButton, BackButton;
					ResumeButton = {280,250,240,60};
					RetryButton = {280,330,240,60};
					BackButton = {280,410,240,60};
					if( SDL_PointInRect( &point, &ResumeButton ) ){
						state = MU_GAME_STATE_RUNNING;
						drawGame();
						SDL_Delay(300);
					}
					else if( SDL_PointInRect( &point, &RetryButton ) ){
						state = MU_GAME_STATE_END;
					}
					else if( SDL_PointInRect( &point, &BackButton ) ){
						state = MU_GAME_STATE_END;
						musnakeState = MU_STATE_RUNNING;
					}
				}
				break;
			}
		}
		dt += getTimeDelta();
		dt %= 3290;

		// ����FPS
		int2str(fpss, fps);
		drawText(gameRender, fpss, 740 - 10 * SDL_strlen(fpss), 570, 10);
		drawText(gameRender, (char*)"FPS", 750, 570, 10);

		// ������ͣ�˵�
		long long nt = (getTimeVal() / 500) & 1 ? 4 - (getTimeVal() % 1000 / 200) : getTimeVal() % 1000 / 100;
		titleBGFlame->draw(gameRender, 0, -dt / 10);
		gamePauseBGMask->draw(gameRender, 0, 0);
		gamePauseTitleFlame->draw_centered(gameRender, 400, 180);
		gamePauseResumeButtonFlame[choosing == 0]->draw(gameRender, 280, 250);
		gamePauseRetryButtonFlame[choosing == 1]->draw(gameRender, 280, 330);
		gamePauseBackButtonFlame[choosing == 2]->draw(gameRender, 280, 410);
		SDL_RenderPresent(gameRender);
	}
	SDL_RenderClear(gameRender);
	Mix_ResumeMusic();
	if (state == MU_GAME_STATE_RUNNING) {
		updateTime();
		refreshTime(0);
	}
}

inline void musnake::Game::drawGame() {
	static int v = 0;
	v += getTimeDelta();
	int vp = v / 50;
	vp %= 80;

	for (int i = -2;i < 20;i++) {
		for (int j = -1;j < 15;j++) {
			gridWaterFlame->draw(gameRender, i * 40 + vp, j * 40 + vp / 2);
		}
	}

	for (int i = 0;i < 64;i++) {
		for (int j = 0;j < 64;j++) {
			gameMap[i][j]->update();
			gameMap[i][j]->draw(gameRender, &base);
		}
	}
	snakeTail->update();
	snakeTail->draw(gameRender, &base);  // Ϊ�˱�֤��β��ʹ������ʧ�׶�Ҳ����ȷ���ƣ��������һ����

	drawUI();

	// ����FPS
	drawNumber(gameRender, numberFPSFlame, fps, 750, 580);
}

inline void musnake::Game::drawUI() {
	// 食物指示标
	if (!pointInWindow(food[0]->getGrid()->getCenterPoint())) drawFoodPointer(0);
	if (!pointInWindow(food[1]->getGrid()->getCenterPoint())) drawFoodPointer(1);
	if (!pointInWindow(food[2]->getGrid()->getCenterPoint())) drawFoodPointer(2);

	if (interval > 0) {
		text_Score_Flame->draw(gameRender, 10, 4);
		drawNumber(gameRender, numberScoreFlame, length, 10, 32);
		return;
	}

	// �������Ѫ��
	SDL_Rect hpRect[5] = {
		{800 - 210, 10, 40, 40},
		{800 - 170, 10, 40, 40},
		{800 - 130, 10, 40, 40},
		{800 - 90, 10, 40, 40},
		{800 - 50, 10, 40, 40},
	};  // hp��ʾ
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

	// �������Note
	Note* np = note;
	int fn = fever;
	notesignFlame[2]->draw(gameRender, 80, 520);
	if (playing) {
		while (np) {
			int dt;
			if ((dt = (int)(np->time - getTimeVal())) > 1500) break;
			dt = (dt + 500) * 2 / 5;
			SDL_Rect r = { dt, 520, 5, 60 };
			if (fn > 0) {
				notesignFlame[1]->draw(gameRender, &r);
				fn--;
			}
			else {
				notesignFlame[0]->draw(gameRender, &r);
			}
			np = np->next;
		}
	}

	text_Score_Flame->draw(gameRender, 10, 4);
	drawNumber(gameRender, numberScoreFlame, score, 10, 32);

	// ����������
	if (combo >= 2) {
		drawNumber_Centered(gameRender, numberHitsFlame, combo, 140, 470);
		text_Hits_Flame->draw_centered(gameRender, 140, 510);
	}
}

void musnake::Game::drawFoodPointer(int index) {
	SDL_Point pp = *(food[index]->getGrid()->getCenterPoint());
	int dx = pp.x + base.x - musnakeCenterPoint.x;
	int dy = pp.y + base.y - musnakeCenterPoint.y;
	if (dx > 0) {
		if (dy > 0) {
			double k;
			if (interval > 0) {
				if ((k = 1. * dx / dy) > 360. / 260) {
					int y;
					foodPointerFlame[index][0]->draw_centered(gameRender, 760, (y = int(300 + 360 / k)));
					foodPointerFlame[index][1]->draw_centered(gameRender, 760, y, 180 - 180 * atan(k) / M_PI);
				}
				else {
					int x;
					foodPointerFlame[index][0]->draw_centered(gameRender, (x = int(400 + 260 * k)), 560);
					foodPointerFlame[index][1]->draw_centered(gameRender, x, 560, 180 - 180 * atan(k) / M_PI);
				}
			}
			else {
				if ((k = 1. * dx / dy) > 360. / 180) {
					int y;
					foodPointerFlame[index][0]->draw_centered(gameRender, 760, (y = int(300 + 360 / k)));
					foodPointerFlame[index][1]->draw_centered(gameRender, 760, y, 180 - 180 * atan(k) / M_PI);
				}
				else {
					int x;
					foodPointerFlame[index][0]->draw_centered(gameRender, (x = int(400 + 180 * k)), 480);
					foodPointerFlame[index][1]->draw_centered(gameRender, x, 480, 180 - 180 * atan(k) / M_PI);
				}
			}
		}
		else if (dy < 0) {
			double k;
			if ((k = 1. * dx / -dy) > 360. / 260) {
				int y;
				foodPointerFlame[index][0]->draw_centered(gameRender, 760, (y = int(300 - 360 / k)));
				foodPointerFlame[index][1]->draw_centered(gameRender, 760, y, 180 * atan(k) / M_PI);
			}
			else {
				int x;
				foodPointerFlame[index][0]->draw_centered(gameRender, (x = int(400 + 260 * k)), 40);
				foodPointerFlame[index][1]->draw_centered(gameRender, x, 40, 180 * atan(k) / M_PI);
			}
		}
		else {  // dy == 0
			foodPointerFlame[index][0]->draw_centered(gameRender, 760, 300);
			foodPointerFlame[index][1]->draw_centered(gameRender, 760, 300, 90);
		}
	}
	else if (dx < 0) {
		if (dy > 0) {
			double k;
			if (interval > 0) {
				if ((k = 1. * -dx / dy) > 360. / 260) {
					int y;
					foodPointerFlame[index][0]->draw_centered(gameRender, 40, (y = int(300 + 360 / k)));
					foodPointerFlame[index][1]->draw_centered(gameRender, 40, y, 180 + 180 * atan(k) / M_PI);
				}
				else {
					int x;
					foodPointerFlame[index][0]->draw_centered(gameRender, (x = int(400 - 260 * k)), 560);
					foodPointerFlame[index][1]->draw_centered(gameRender, x, 560, 180 + 180 * atan(k) / M_PI);
				}
			}
			else {
				if ((k = 1. * -dx / dy) > 360. / 180) {
					int y;
					foodPointerFlame[index][0]->draw_centered(gameRender, 40, (y = int(300 + 360 / k)));
					foodPointerFlame[index][1]->draw_centered(gameRender, 40, y, 180 + 180 * atan(k) / M_PI);
				}
				else {
					int x;
					foodPointerFlame[index][0]->draw_centered(gameRender, (x = int(400 - 180 * k)), 480);
					foodPointerFlame[index][1]->draw_centered(gameRender, x, 480, 180 + 180 * atan(k) / M_PI);
				}
			}
		}
		else if (dy < 0) {
			double k;
			if ((k = 1. * -dx / -dy) > 360. / 260) {
				int y;
				foodPointerFlame[index][0]->draw_centered(gameRender, 40, (y = int(300 - 360 / k)));
				foodPointerFlame[index][1]->draw_centered(gameRender, 40, y, -180 * atan(k) / M_PI);
			}
			else {
				int x;
				foodPointerFlame[index][0]->draw_centered(gameRender, (x = int(400 - 260 * k)), 40);
				foodPointerFlame[index][1]->draw_centered(gameRender, x, 40, -180 * atan(k) / M_PI);
			}
		}
		else {  // dy == 0
			foodPointerFlame[index][0]->draw_centered(gameRender, 40, 300);
			foodPointerFlame[index][1]->draw_centered(gameRender, 40, 300, -90);
				
		}
	}
	else {  // dx == 0
		if (dy > 0) {
			if (interval > 0) {
				foodPointerFlame[index][0]->draw_centered(gameRender, 400, 560);
				foodPointerFlame[index][1]->draw_centered(gameRender, 400, 560, 180);
			}
			else {
				foodPointerFlame[index][0]->draw_centered(gameRender, 400, 480);
				foodPointerFlame[index][1]->draw_centered(gameRender, 400, 480, 180);
			}
		}
		if (dy < 0) {
			foodPointerFlame[index][0]->draw_centered(gameRender, 400, 40);
			foodPointerFlame[index][1]->draw_centered(gameRender, 400, 40);
		}
	}
}

void musnake::drawText(SDL_Renderer* render, char* text, int x, int y, int size) {
	char* cp = text;
	while (*cp) {
		SDL_Rect r = { x, y, size, size * 2 };
		charFlame[*cp - 32]->draw(render, &r);
		cp++;
		x += size;
	}
}

inline musnake::Snake::~Snake() {
	if (next) {
		delete next;
	}
	else if (this == thisGame->snakeTail) thisGame->setSnakeTail(nullptr);
	/* 因为一般而言，只有蛇尾才会在游戏过程中自动被释放，所以如果不是蛇尾，也就是有next，
	 * 那么只会是游戏结束时的大释放，这时候应该递归地把这些蛇体全释放掉 */
}

inline void musnake::Snake::endTail(bool delay) {
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
	if (delay) {
		musnake::thisGame->setDelayFunc(&discardTail, (unsigned long)this, 75);  // չʾ��Ч���͸Ͻ�GG
	}
	else {
		delete this;
	}
}
