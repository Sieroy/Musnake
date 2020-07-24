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

	// ���ߺ�������������ƶ����ͷ���0�����򷵻�1���ں�����ʵ�ֿ�Ѫ֮��Ĳ���
	// �����ƶ������Ž������β��ķ��򡢳ɹ����ƶ���
	// �������ƶ���������ƶ���β���ƶ���ײǽ��ײ����
	int moveSnake(int dir);

	// ΪС�߽��ƶ���
	void unlockMoving();
	
	// ��Ϸ���е�С������
	void run();

private:
	int hp = 5;  // �ߵ�Ѫ������ʼΪ5
	long long int note = -1;  // �����ж�������ĳ���������ж����note�ᱻ��ֵΪ��ǰʱ��ֵ���ж��ɹ����Ϊ-1
	bool movingLock = false;  // Ϊʵ��Moves per Second���ƶ��ӵ��ƶ���
	int combo = 0;  // ������
	SDL_Rect drawRect;  // ��ǰ��Ļ���������Ӧ��������ͼ�ľ���
	Snake* snakeHead, * snakeTail;  // ��ͷ����β
	DelayFunc* timingFunc = nullptr;  // ������Ϸ�õ���ʱ������ע��ʵ����ͣЧ��ʱ����ʱ�䣩
};

musnake::Game::Game() {
	// ��˵����Ӧ���Ǵ������ļ������������ʼ���ظ�ģ��ֽ׶ξ���д����
	SDL_Surface* picSurf, tmpSurf;
	SDL_Texture* tmpTex;

	state = MU_GAME_STATE_LOADING;

	// ���ﶨλͼƬ�ļ���λ��
	char snakeBodyPicturePath[256], snakeHeadPicturePath[256], snakeTailPicturePath[256];
	catPath(snakeBodyPicturePath, "image\\snake_0_body.png");
	catPath(snakeHeadPicturePath, "image\\snake_0_head.png");
	catPath(snakeTailPicturePath, "image\\snake_0_tail.png");

	// ��ʼװ����ͷͼ��������ͷ��Ӧ��ö�ٴ�14�ţ�MU_SNAKE_FLAME_HEAD_0toUP����ʼ��29�Ž���
	picSurf = IMG_Load(snakeHeadPicturePath);
	for (int i = 0;i < 16;i++) {  // 16����ͷ�˶����
		Flame* flames[8];
		for (int j = 0;j < 8;j++) {  // ÿ���˶������8֡
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

	// ��ʼװ����βͼ��������β��Ӧ��ö�ٴ�30�ţ�MU_SNAKE_FLAME_TAIL_UPshake����ʼ��49�Ž���
	picSurf = IMG_Load(snakeTailPicturePath);
	for (int i = 0;i < 20;i++) {  // 20��������β�����
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

	// ��ʼװ��һ������ͼ��һ�������Ӧ��ö�ٴ�0�ţ�MU_SNAKE_FLAME_HEAD_UP����ʼ��13�Ž���
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

	// Ȼ��Բ�ͬ��֡����й�����ܳ��
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
	// ܳ�ˣ�

	/* ����Spritesͼ��װ�أ����ȿ϶������Ż������ġ�����ͷ��β����֡��ĩ֡����һ���;�̬��ͷ��βͼ�ظ���
	 * ������Щ��ͬ��ͼ��û��Ҫ�ٴ�װ���ˣ���������Ÿ���һ�¾ͺ��ˣ������������Ż������ڴ����⡣
	 * ���⣬Ŀǰ��ֻ������һ��Ƥ����������򵥵�����Ƥ����֮���ٻ����Ƥ���Ļ����Ϳ��ܳ���ԭ��Ӧ����̬��ͼ��ö�̬֮������
	 * �������ã��Ͼ����ڵ�֡��ṹ��Ӧ���������������̬ͼ��Sprites���̲�һ��ô�졭��
	 * �������ڿ���Ҫ��Ҫ���������JSON֧�֣�������̬�������ˣ��������������Կ�����������֮��ġ�
	 * �ȿ����Űɣ�����˳��+ʱ���ԣ�Ļ�����Ҳ���ǲ����ԡ�
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
	// �ȱ����ţ���ΪĿǰ���ǲ��Խ׶Σ�һ�����еĻ�û��ж�صظ�֮��ı�Ҫ���������ص��ͺ�~
	// �Ȱѵظ���ͷ�д�˰�
	for (int i = 0;i < 20;i++) {
		for (int j = 0;j < 15;j++) {
			delete gameMap[i][j];
		}
	}
	// �ٰ��ߵ��ͷż��ϰɣ������ߵİ�ݹ�����Ӧ�ò���
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

	if (movingLock) {  // ���������MPS�����ƣ����ж�Ϊ�ֶ�
		return 0;
	}
	else {
		movingLock = true;
		setDelayFunc(&unlockMoving_D, 0, 200);  // 0.2����Խ⿪
	}

	if (dir == snakeHead->getTailDir()) {  // ����
		combo = 0;
		dir = (dir + 2) % 4;
		returnVal = 1;
	}

	if (note == -1) {  // ��ǰû������
		combo = 0;
		returnVal = 1;
	}
	else if (getTimeVal() - note > 400) {  // һ����������0.4��û�а�����Ϊmiss������������ϡ��һЩ����������
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
		if (gp->getSnake() != snakeTail) {  // �����ָΪ������ʼ�˺��ж���ȡ���˴��ƶ�
	case MU_GRID_OBJECT_TYPE_BLOCK:  // ���ǰ�����ϰ�����ͬ
			hp -= 4;
			combo = 0;
			return 1;
		}  // �����ָΪ��β��������һ��case��ʼ��ͷ����β
	case MU_GRID_OBJECT_TYPE_EMPTY:
	case MU_GRID_OBJECT_TYPE_FOOD:
		// �ȴ���β�͵�����
		if (gp->objType == MU_GRID_OBJECT_TYPE_FOOD) {
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

void musnake::Game::run() {
	// ��������µ����̰�
	state = MU_GAME_STATE_RUNNING;

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
				break;  // ���ڿ���Ҫ��Ҫgotoֱ�ӳ�����return����Ȼ�������ʣ�µ��¼���emmm...
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
		snakeTail->draw(gameRender);  // Ϊ�˱�֤��β��ʹ������ʧ�׶�Ҳ����ȷ���ƣ��������һ����

		SDL_RenderPresent(gameRender);
	}
}
