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

	int rv, sv, lv;

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

	void drawWin(int t);
	void drawLose(int t);

	void loadMap();

	void loadToast();

	void unloadToast();

	void updateBase();

	void ending();
};

musnake::Game::Game() {

}

musnake::Game::~Game() {

	for (int i = 0;i < 64;i++) {
		for (int j = 0;j < 64;j++) {
			delete gameMap[i][j];
		}
	}

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

	char tmpPath[256];
	char levelfile[64];

	SDL_strlcpy(levelfile, levelPath, 64);
	SDL_strlcat(levelfile, "\\bgm.mp3", 64);
	catPath(tmpPath, levelfile);
	bgm = Mix_LoadMUS(tmpPath);

	if (lp->timev) {
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

	if (movingLock) {
		removeDelayFuncByFuncArg(&timingFunc, &unlockMoving_D, 0);
		if (!removeDelayFuncByFuncArg(&timingFunc, &discardTail, (unsigned long)snakeTail)) {
			Snake* sp = snakeTail->getPrev();
			delete snakeTail;
			snakeTail = sp;
		}
		setDelayFunc(&unlockMoving_D, 0, 78);
	}
	else {
		movingLock = true;
		setDelayFunc(&unlockMoving_D, 0, 78);
	}

	if (dir == snakeHead->getTailDir()) {
		combo = 0;
		dir = (dir + 2) % 4;
		returnVal = 1;
	}

	if (!playing || !note || note->time > (long long)getTimeVal()) {
		combo = 0;
		returnVal = 1;
	}
	else if (dir == MU_SNAKE_DIRECT_NONE) {
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
	else {
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
		if (gp->getSnake() != snakeTail) {
	case MU_GRID_OBJECT_TYPE_BLOCK:
	case MU_GRID_OBJECT_TYPE_DARK:
		hp -= interval > 0 ? 10 : 3;
			if (hp < 0) hp = 0;
			combo = 0;
			return 1;
		}
		if (gp->objType == MU_GRID_OBJECT_TYPE_FOOD) {
	case MU_GRID_OBJECT_TYPE_FOOD:
			length += 1;
			if (hp < 10) hp += 1;
			else fever = 4 + length / 10;
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

		sp = new Snake((dir + 2) % 4);
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
		int x = Rander() % 62 + 1, y = Rander() % 62 + 1;
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

	while (dfp) {
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

	while (dfp) {
		dfp->time += dt;
		dfp = dfp->next;
	}

	dfp = toastQueue;
	while (dfp) {
		dfp->time += dt;
		dfp = dfp->next;
	}
	

	if (!pausingTime) dt += noteDelta;
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
	SDL_Surface* tmpSurf;

	state = MU_GAME_STATE_RUNNING;
	if (interval <= 0) {
		setDelayFunc(&passLevel, 0, levelinfo->timev + 2000);
		setDelayFunc(&playBGM_D, 1, 500);
	}
	else {
		setDelayFunc(&playBGM_D, -1, 500);
	}

	while (state == MU_GAME_STATE_RUNNING) {
		updateTime();
		SDL_RenderClear(gameRender);

		while (SDL_PollEvent(&evt)) {
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
				case SDL_WINDOWEVENT_FOCUS_LOST:
				case SDL_WINDOWEVENT_HIDDEN:
					state = MU_GAME_STATE_PAUSED;
					pause();
					break;
				}
				break;
			case SDL_QUIT:
				state = MU_GAME_STATE_END;
				musnakeState = MU_STATE_OVER;
				break;
			}
		}

		if (playing && note && ((long long)getTimeVal() - note->time > 300)) moveSnake(MU_SNAKE_DIRECT_NONE);

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

	ending();

	if (state == MU_GAME_STATE_END) return;
}
	
void musnake::Game::ending(){
	int timing = 0;
	char rankStr[4];
	SDL_Event evt;

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
		lv = length > userData["record"][levelinfo->id]["length"].asUInt() ? length : 0;

		updateUserScore(levelinfo->id, rv, sv, lv);
		updateLevelBestFlame(levelinfo);
		flushUserData();
	}
	else if (interval > 0) {
		lv = length > userData["record"][levelinfo->id]["length"].asUInt() ? length : 0;

		updateUserScore(levelinfo->id, -1, -1, lv);
		updateLevelBestFlame(levelinfo);
		flushUserData();
	}

	while (state == MU_GAME_STATE_OVER) {
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
				if( evt.button.button == SDL_BUTTON_LEFT && timing > 1400 ){
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

		if (hp > 0 || interval > 0) {
			drawWin(timing);
		}
		else {
			drawLose(timing);
		}
		
		drawNumber(numberFPSFlame, fps, 750, 580);

		SDL_RenderPresent(gameRender);
		timing += getTimeDelta();
	}
	SDL_RenderClear(gameRender);
}

void musnake::Game::drawWin(int t) {
	if (t < 1000) {
		drawGame();
		gamewinBGFlame->draw(0, (int)(600 - t));
	}
	else {
		int l = t > 1400 ? 400 : (int)(t - 1000);
		SDL_Rect prect = { 970 - 2 * l, 0, 610, 600 };
		gamewinBGFlame->draw(0, -400 - (t / 10 - 100) % 130);
		SDL_RenderFillRect(gameRender, &prect);
		gameOverBackButtonFlame->draw(l - 400, 0);
		gameOverRetryButtonFlame->draw(l - 400, 450);
		gameOverOKButtonFlame->draw(l - 400, 520);

		if (interval <= 0)
			drawText(gameRender, rankStr, 1000 - 2 * l, 100, 80);

		if (interval > 0) {
			levelinfo->nameFlm->draw(1000 - 2 * l, 140);
			text_TotalLength_Flame->draw(1000 - 2 * l, 200);
			drawNumber(numberTotalFlame, length, 1060 - 2 * l, 240);
			if (lv > 0) gamewinNewBestFlame->draw(1060 - 2 * l, 280);
		}
		else {
			levelinfo->nameFlm->draw(1300 - 2 * l, 140);
			text_TotalScore_Flame->draw(1000 - 2 * l, 300);
			text_TotalLength_Flame->draw(1000 - 2 * l, 450);
			drawNumber(numberTotalFlame, score, 1060 - 2 * l, 355);
			drawNumber(numberTotalFlame, length, 1060 - 2 * l, 505);
			if (rv >= 0) gamewinNewBestFlame->draw(1300 - 2 * l, 200);
			if (sv >= 0) gamewinNewBestFlame->draw(1140 - 2 * l, 300);
			if (lv > 0) gamewinNewBestFlame->draw(1140 - 2 * l, 450);
		}
	}
}

void musnake::Game::drawLose(int t) {
	drawGame();
	if (t < 1000) {
		gameloseBGFlame->setAlpha(t / 2 <= 255 ? t / 2 : 255);
		gameloseFGFlame->setAlpha(t / 2 <= 255 ? t / 2 : 255);
		gameloseBGFlame->draw(&musnakeRect);
		gameloseFGFlame->draw_centered(400, 280);
	}
	else {
		int l = t > 1400 ? 400 : (int)(t - 1000);
		SDL_Rect prect = { 970 - 2 * l, 0, 610, 600 };
		gameloseBGFlame->draw(&musnakeRect);
		SDL_RenderFillRect(gameRender, &prect);
		gameOverBackButtonFlame->draw(l - 400, 0);
		gameOverRetryButtonFlame->draw(l - 400, 450);
		gameOverOKButtonFlame->draw(l - 400, 520);
		gameloseFGFlame->draw_centered(400 + l / 5, 280 - l / 10);
		levelinfo->nameFlm->draw_centered(1275 - 2 * l, 480);
	}
}

void musnake::Game::pause() {
	static int dt = 0;
	int choosing = 0;
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

		long long nt = (getTimeVal() / 500) & 1 ? 4 - (getTimeVal() % 1000 / 200) : getTimeVal() % 1000 / 100;
		titleBGFlame->draw(0, -dt / 10);
		gamePauseBGMask->draw(0, 0);
		gamePauseTitleFlame->draw_centered(400, 180);
		gamePauseResumeButtonFlame[choosing == 0]->draw(280, 250);
		gamePauseRetryButtonFlame[choosing == 1]->draw(280, 330);
		gamePauseBackButtonFlame[choosing == 2]->draw(280, 410);
		drawNumber(numberFPSFlame, fps, 750, 580);

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
			gridWaterFlame->draw(i * 40 + vp, j * 40 + vp / 2);
		}
	}

	for (int i = 0;i < 64;i++) {
		for (int j = 0;j < 64;j++) {
			gameMap[i][j]->update();
			gameMap[i][j]->draw(&base);
		}
	}
	snakeTail->update();
	snakeTail->draw(&base);

	drawUI();

	drawNumber(numberFPSFlame, fps, 750, 580);
}

inline void musnake::Game::drawUI() {
	// 食物指示标
	if (!pointInWindow(food[0]->getGrid()->getCenterPoint())) drawFoodPointer(0);
	if (!pointInWindow(food[1]->getGrid()->getCenterPoint())) drawFoodPointer(1);
	if (!pointInWindow(food[2]->getGrid()->getCenterPoint())) drawFoodPointer(2);

	if (interval > 0) {
		text_Score_Flame->draw(10, 4);
		drawNumber(numberScoreFlame, length, 10, 32);
		return;
	}

	SDL_Rect hpRect[5] = {
		{800 - 210, 10, 40, 40},
		{800 - 170, 10, 40, 40},
		{800 - 130, 10, 40, 40},
		{800 - 90, 10, 40, 40},
		{800 - 50, 10, 40, 40},
	};
	int fg = movingLock ? 1 : 0;
	for (int i = 0;i < 5;i++) {
		if (i < hp / 2) {
			hpFlame[0][fg]->draw(&hpRect[i]);
		}
		else if (i * 2 + 1 == hp) {
			hpFlame[1][fg]->draw(&hpRect[i]);
		}
		else {
			hpFlame[2][fg]->draw(&hpRect[i]);
		}
	}

	if (hp > 0) {
		Note* np = note;
		int fn = fever;
		notesignFlame[2]->draw(80, 520);
		if (playing) {
			while (np) {
				int dt;
				if ((dt = (int)(np->time - getTimeVal())) > 1500) break;
				dt = (dt + 500) * 2 / 5;
				SDL_Rect r = { dt, 520, 5, 60 };
				if (fn > 0) {
					notesignFlame[1]->draw(&r);
					fn--;
				}
				else {
					notesignFlame[0]->draw(&r);
				}
				np = np->next;
			}
		}
	}

	text_Score_Flame->draw(10, 4);
	drawNumber(numberScoreFlame, score, 10, 32);

	if (combo >= 2) {
		drawNumber_Centered(numberHitsFlame, combo, 140, 470);
		text_Hits_Flame->draw_centered(140, 510);
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
					foodPointerFlame[index][0]->draw_centered(760, (y = int(300 + 360 / k)));
					foodPointerFlame[index][1]->draw_centered(760, y, 180 - 180 * atan(k) / M_PI);
				}
				else {
					int x;
					foodPointerFlame[index][0]->draw_centered((x = int(400 + 260 * k)), 560);
					foodPointerFlame[index][1]->draw_centered(x, 560, 180 - 180 * atan(k) / M_PI);
				}
			}
			else {
				if ((k = 1. * dx / dy) > 360. / 180) {
					int y;
					foodPointerFlame[index][0]->draw_centered(760, (y = int(300 + 360 / k)));
					foodPointerFlame[index][1]->draw_centered(760, y, 180 - 180 * atan(k) / M_PI);
				}
				else {
					int x;
					foodPointerFlame[index][0]->draw_centered((x = int(400 + 180 * k)), 480);
					foodPointerFlame[index][1]->draw_centered(x, 480, 180 - 180 * atan(k) / M_PI);
				}
			}
		}
		else if (dy < 0) {
			double k;
			if ((k = 1. * dx / -dy) > 360. / 260) {
				int y;
				foodPointerFlame[index][0]->draw_centered(760, (y = int(300 - 360 / k)));
				foodPointerFlame[index][1]->draw_centered(760, y, 180 * atan(k) / M_PI);
			}
			else {
				int x;
				foodPointerFlame[index][0]->draw_centered((x = int(400 + 260 * k)), 40);
				foodPointerFlame[index][1]->draw_centered(x, 40, 180 * atan(k) / M_PI);
			}
		}
		else {  // dy == 0
			foodPointerFlame[index][0]->draw_centered(760, 300);
			foodPointerFlame[index][1]->draw_centered(760, 300, 90);
		}
	}
	else if (dx < 0) {
		if (dy > 0) {
			double k;
			if (interval > 0) {
				if ((k = 1. * -dx / dy) > 360. / 260) {
					int y;
					foodPointerFlame[index][0]->draw_centered(40, (y = int(300 + 360 / k)));
					foodPointerFlame[index][1]->draw_centered(40, y, 180 + 180 * atan(k) / M_PI);
				}
				else {
					int x;
					foodPointerFlame[index][0]->draw_centered((x = int(400 - 260 * k)), 560);
					foodPointerFlame[index][1]->draw_centered(x, 560, 180 + 180 * atan(k) / M_PI);
				}
			}
			else {
				if ((k = 1. * -dx / dy) > 360. / 180) {
					int y;
					foodPointerFlame[index][0]->draw_centered(40, (y = int(300 + 360 / k)));
					foodPointerFlame[index][1]->draw_centered(40, y, 180 + 180 * atan(k) / M_PI);
				}
				else {
					int x;
					foodPointerFlame[index][0]->draw_centered((x = int(400 - 180 * k)), 480);
					foodPointerFlame[index][1]->draw_centered(x, 480, 180 + 180 * atan(k) / M_PI);
				}
			}
		}
		else if (dy < 0) {
			double k;
			if ((k = 1. * -dx / -dy) > 360. / 260) {
				int y;
				foodPointerFlame[index][0]->draw_centered(40, (y = int(300 - 360 / k)));
				foodPointerFlame[index][1]->draw_centered(40, y, -180 * atan(k) / M_PI);
			}
			else {
				int x;
				foodPointerFlame[index][0]->draw_centered((x = int(400 - 260 * k)), 40);
				foodPointerFlame[index][1]->draw_centered(x, 40, -180 * atan(k) / M_PI);
			}
		}
		else {  // dy == 0
			foodPointerFlame[index][0]->draw_centered(40, 300);
			foodPointerFlame[index][1]->draw_centered(40, 300, -90);
				
		}
	}
	else {  // dx == 0
		if (dy > 0) {
			if (interval > 0) {
				foodPointerFlame[index][0]->draw_centered(400, 560);
				foodPointerFlame[index][1]->draw_centered(400, 560, 180);
			}
			else {
				foodPointerFlame[index][0]->draw_centered(400, 480);
				foodPointerFlame[index][1]->draw_centered(400, 480, 180);
			}
		}
		if (dy < 0) {
			foodPointerFlame[index][0]->draw_centered(400, 40);
			foodPointerFlame[index][1]->draw_centered(400, 40);
		}
	}
}

void musnake::drawText(SDL_Renderer* render, char* text, int x, int y, int size) {
	char* cp = text;
	while (*cp) {
		SDL_Rect r = { x, y, size, size * 2 };
		charFlame[*cp - 32]->draw(&r);
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
		musnake::thisGame->setDelayFunc(&discardTail, (unsigned long)this, 75);
	}
	else {
		delete this;
	}
}
