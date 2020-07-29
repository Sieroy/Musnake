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

	// ���ߺ�������������ƶ����ͷ���0�����򷵻�1���ں�����ʵ�ֿ�Ѫ֮��Ĳ���
	// �����ƶ������Ž������β��ķ��򡢳ɹ����ƶ���
	// �������ƶ���������ƶ���β���ƶ���ײǽ��ײ����
	int moveSnake(int dir);

	// ����ʳ��
	void refreshFood();

	// ΪС�߽��ƶ���
	void unlockMoving();

	// ����BGM
	void playBGM();
	
	// ��Ϸ���е�С������
	void run();

	// ˢ����ʱ������Note��ʱ��ֵ���ڿ��ֺͽ�����ͣʱ����
	void refreshTime(int delta);

	void init(char* levelname);

	void draw();

private:
	char level[32] = "level\\";
	int hp = 5;  // �ߵ�Ѫ������ʼΪ5
	int fever = 0;  // FEVER״̬��2���÷�
	Note* note = nullptr;  // ����
	Food* food = nullptr;  // ʳ��
	Mix_Music* bgm = nullptr;  // BGM
	bool movingLock = false;  // Ϊʵ��Moves per Second���ƶ��ӵ��ƶ���
	unsigned int combo = 0;  // ������
	unsigned int score = 0;  // �÷�
	unsigned long long pausingTime = 0;  // ��ͣʱ��ʱ��ֵ
	SDL_Rect drawRect;  // ��ǰ��Ļ���������Ӧ��������ͼ�ľ���
	Snake* snakeHead, * snakeTail;  // ��ͷ����β
	DelayFunc* timingFunc = nullptr;  // ������Ϸ�õ���ʱ������ע��ʵ����ͣЧ��ʱ����ʱ�䣩
};

musnake::Game::Game() {

}

musnake::Game::~Game() {
	// �ȱ����ţ���ΪĿǰ���ǲ��Խ׶Σ�һ�����еĻ�û��ж�صظ�֮��ı�Ҫ���������ص��ͺ�~
	// �Ȱѵظ���ͷ�д�˰�
	for (int i = 0;i < 20;i++) {
		for (int j = 0;j < 15;j++) {
			delete gameMap[i][j];
		}
	}
	// �ٰ��ߵ��ͷż��ϰɣ������ߵİ�ݹ�����Ӧ�ò���
	delete snakeHead;

	clearDelayFunc(&timingFunc);
	clearNotes(&note);

	Mix_HaltMusic();
	Mix_FreeMusic(bgm);
}

void musnake::Game::init(char* levelname){
	// ��˵����Ӧ���Ǵ������ļ������������ʼ���ظ�ģ��ֽ׶ξ���д����
	SDL_Surface* picSurf, * tmpSurf;
	SDL_Texture* tmpTex;

	state = MU_GAME_STATE_LOADING;

	SDL_strlcat(level, levelname, 25);

	// ���ﶨλ�ļ���λ��
	char tmpPath[256];

	// �ؿ�·��
	char levelfile[64];
	// װ�عؿ�����
	SDL_strlcpy(levelfile, level, 64);
	SDL_strlcat(levelfile, "\\bgm.mp3", 64);
	catPath(tmpPath, levelfile);
	bgm = Mix_LoadMUS(tmpPath);

	// װ�عؿ��Ľ���
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
	SDL_RWclose(f);

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

	// �ػ�һ�����ǵ�С��

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

	if (movingLock) {  // ���������10MPS�����ƣ����ж�Ϊ�ֶ�
		return 1;
	}
	else {
		movingLock = true;
		setDelayFunc(&unlockMoving_D, 0, 100);  // 0.1����Խ⿪
	}

	if (dir == snakeHead->getTailDir()) {  // ����
		combo = 0;
		dir = (dir + 2) % 4;
		returnVal = 1;
	}

	if (note->time > (long long)getTimeVal()) {  // ��ǰû������
		combo = 0;
		returnVal = 1;
	}
	else if (dir == MU_SNAKE_DIRECT_NONE) {  // һ����������0.2��û�а�����Ϊmiss
		combo = 0;
		returnVal = 1;
		dir = (snakeHead->getTailDir() + 2) % 4;
		Note* np = note;
		note = np->next;
		delete np;
	}
	else {  // ������������
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
		if (gp->getSnake() != snakeTail) {  // �����ָΪ������ʼ�˺��ж���ȡ���˴��ƶ�
	case MU_GRID_OBJECT_TYPE_BLOCK:  // ���ǰ�����ϰ�����ͬ
			hp -= 3;  // ���ˣ��ļ�һЩ���Ϳ�3Ѫ
			if (hp < 0) hp = 0;
			combo = 0;
			return 1;
		}  // �����ָΪ��β��������һ��case��ʼ��ͷ����β
	case MU_GRID_OBJECT_TYPE_EMPTY:
	case MU_GRID_OBJECT_TYPE_FOOD:
		// �ȴ���β�͵�����
		if (gp->objType == MU_GRID_OBJECT_TYPE_FOOD) {
			if (hp < 10) hp += 1;
			else fever = 5;
			delete gp->getFood();
			gp->setFood(nullptr);
			food = nullptr;
			score += fever > 0 ? 200 : 100;
			snakeTail->shakeTail();
		}
		else {
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

void musnake::Game::refreshFood() {
	while (true) {
		int x = Rander() % 20, y = Rander() % 15;  // ����roll��ʽ�϶�Ҫ�ĵģ���Ȼ�����ں���roll���յľͺܲ���
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

	while (dfp) {  // ����ʱӦ�ò�������Ϸ������ʱ������
		dfp->time += dt;
		dfp = dfp->next;
	}

	if (!pausingTime) dt += noteDelta;  // ����run��ʼ���Ǵ�ˢ�£�����ƫ��
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
	// ��������µ����̰�
	state = MU_GAME_STATE_RUNNING;
	updateTime();
	refreshTime(2000);
	movingLock = true;
	setDelayFunc(&unlockMoving_D, 0, 1500);
	setDelayFunc(&playBGM_D, 0, 2000);

	while (state != MU_GAME_STATE_OVER) {
		SDL_Event evt;
		updateTime();
		SDL_RenderClear(gameRender);  // ��Ӳ�壡

		while (SDL_PollEvent(&evt)) {
			// ����Ҫ������������¼���
			// �Դ����뵽��һ�־���Ľ������
			// ��ϧ����λ��̫�٣�д���� [doge]
			switch (evt.type) {
			case SDL_KEYDOWN:
				switch (evt.key.keysym.sym) {
				case SDLK_UP:
				case SDLK_w:  // �������λ�ȱ����Űɣ��Ժ󿪷��������ü�λʱ��˵���ʵ�ַ���
					if (!moveSnake(MU_SNAKE_DIRECT_UP)) {
						if (fever > 0) {
							score += (10 + combo / 10) * 2;
							fever--;
						}
						else
							score += (10 + combo / 10);
					}
					else
						fever = 0;
					break;
				case SDLK_RIGHT:
				case SDLK_d:
					if (!moveSnake(MU_SNAKE_DIRECT_RIGHT)) {
						if (fever > 0) {
							score += (10 + combo / 10) * 2;
							fever--;
						}
						else
							score += (10 + combo / 10);
					}
					else
						fever = 0;
					break;
				case SDLK_DOWN:
				case SDLK_s:
					if (!moveSnake(MU_SNAKE_DIRECT_DOWN)) {
						if (fever > 0) {
							score += (10 + combo / 10) * 2;
							fever--;
						}
						else
							score += (10 + combo / 10);
					}
					else
						fever = 0;
					break;
				case SDLK_LEFT:
				case SDLK_a:
					if (!moveSnake(MU_SNAKE_DIRECT_LEFT)) {
						if (fever > 0) {
							score += (10 + combo / 10) * 2;
							fever--;
						}
						else
							score += (10 + combo / 10);
					}
					else
						fever = 0;
					break;
				}
				break;
			case SDL_QUIT:
				state = MU_GAME_STATE_OVER;
				musnakeState = MU_STATE_OVER;
				break;  // ���ڿ���Ҫ��Ҫgotoֱ�ӳ�����return����Ȼ�������ʣ�µ��¼���emmm...
			}
		}

		if ((long long)getTimeVal() - note->time > 300) moveSnake(MU_SNAKE_DIRECT_NONE);
		// ��ʵ֤������תlong long�ĵط�����unsigned������������������BUG

		triggerDelayFunc(&timingFunc);

		if (!food)
			refreshFood();

		draw();

		SDL_RenderPresent(gameRender);
	}
}

void musnake::Game::draw() {
	// �Ȼ��Ƶ�ͼ�ϵ�����
	if (food) food->draw(gameRender);

	for (int i = 0;i < 20;i++) {
		for (int j = 0;j < 15;j++) {
			gameMap[i][j]->update();
			gameMap[i][j]->draw(gameRender);
		}
	}
	snakeTail->update();
	snakeTail->draw(gameRender);  // Ϊ�˱�֤��β��ʹ������ʧ�׶�Ҳ����ȷ���ƣ��������һ����

	// �ٻ���GUI����
	// �ᶨ�����ع�ʱҪд����ľ���
	
	// �������Ѫ��
	SDL_Rect hpRect[5] = {
		{800 - 210, 10, 40, 40},
		{800 - 170, 10, 40, 40},
		{800 - 130, 10, 40, 40},
		{800 -  90, 10, 40, 40},
		{800 -  50, 10, 40, 40},
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
	while (np) {
		int dt;
		if ((dt = np->time - getTimeVal()) > 1500) break;
		dt = (dt + 500) * 800 / 2000;
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
	
	// ���Ƶ÷�
	char ss[16] = { 0 };  // �ҾͲ��������ܴ�15λ���ķ��ˣ�
	drawText(gameRender, (char*)"score:", 10, 10, 10);
	int2str(ss, score);
	drawText(gameRender, ss, 10, 23, 20);

	// ����������
	if (combo >= 2) {
		int2str(ss, combo);
		SDL_strlcat(ss, " hits!", 16);
		drawText(gameRender, ss, 10, 440, 20);
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

	musnake::thisGame->setDelayFunc(&discardTail, (unsigned long)this, 90);  // չʾ��Ч���͸Ͻ�GG
}
