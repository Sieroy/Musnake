#pragma once

#include <SDL.h>

#include "MU_declaration.h"
#include "MU_flame.h"
#include "MU_time.h"

class musnake::Element {
public:
	void setPosition(int x, int y, int w, int h);
	void setPosition(SDL_Rect* rect);
	void setFlame(Flame* flame);

	void delayFlameTime(long long delta);

	void update();
	void draw();
	void draw(SDL_Point* base);

protected:
	SDL_Rect rect = { 0,0,0,0 };
	Flame* flame = nullptr;
	long long flameTime = -1;
};


class musnake::Food :public Element {
public:
	Food(int type);
	~Food();

	void setGrid(Grid* grid);
	Grid* getGrid();

	void update();

private:
	Grid* grid = nullptr;
	long long duration = -1;
};



class musnake::Snake : public Element {
public:
	Snake();
	Snake(int tailDir);
	~Snake();

	void setHeadDir(int dir);
	void setTailDir(int dir);
	void setNext(Snake* snake);
	void setPrev(Snake* snake);
	void setGrid(Grid* grid);

	Snake* getNext();
	Snake* getPrev();
	Grid* getGrid();
	int getHeadDir();
	int getTailDir();

	void turnBody();
	void turnBody(int headDir);
	void turnTail();
	void shakeTail();
	void endTail(bool delay = true);

private:
	int headDir = MU_SNAKE_DIRECT_NONE;
	int tailDir = MU_SNAKE_DIRECT_NONE;

	Snake* next = nullptr;
	Snake* prev = nullptr;
	Grid* grid = nullptr;
};


// ============================================================
// ============================================================


inline void musnake::Element::setFlame(Flame* flame) {
	Element::flame = flame;
	flameTime = flame->getDuration();
}

inline void musnake::Element::setPosition(int x, int y, int w, int h) {
	rect.x = x;
	rect.y = y;
	rect.w = w;
	rect.h = h;
}

inline void musnake::Element::setPosition(SDL_Rect* rect) {
	this->rect = *rect;
}

inline void musnake::Element::delayFlameTime(long long delta) {
	if (this)
		flameTime += delta;
}

inline void musnake::Element::update() {
	if (flameTime < 0) return;
	flameTime -= getTimeDelta();
	if (flameTime <= 0) {
		if (flame) flame = flame->getNext();
		if (flame) {
			do {
				flameTime += flame->getDuration();
			} while (flameTime <= 0 && flame->getDuration() > 0);
		}
	}
}

inline void musnake::Element::draw() {
	if (flame) flame->draw(&rect);
}

inline void musnake::Element::draw(SDL_Point* base) {
	if (flame) {
		SDL_Rect r = rect;
		if (base) {
			r.x += base->x;
			r.y += base->y;
		}
		if (r.x > MU_WINDOW_WIDTH || r.y > MU_WINDOW_HEIGHT || r.x + 40 < 0 || r.y + 40 < 0) return;
		flame->draw(&r);
	}
}


// ============================================================


class musnake::Grid :public Element {
public:
	Grid();
	Grid(int x, int y);
	~Grid();

	int x, y;
	int objType;

	void setSnake(Snake* snake);
	void setFood(Food* food);

	Snake* getSnake();
	Food* getFood();

	SDL_Rect* getRect();
	SDL_Point* getCenterPoint();

	void update();
	void draw(SDL_Point* base);

private:
	SDL_Point center;
	union {
		Snake* snake;
		Food* food;
	}obj;
};

musnake::Grid::Grid() {
	objType = MU_GRID_OBJECT_TYPE_EMPTY;
	obj.snake = nullptr;
	x = y = -1;
	setFlame(gridFlame);
}

musnake::Grid::Grid(int x, int y) {
	objType = MU_GRID_OBJECT_TYPE_EMPTY;
	obj.snake = nullptr;
	this->x = x;
	this->y = y;
	setPosition(x * 40, y * 40, 40, 40);
	center.x = x * 40 + 20;
	center.y = y * 40 + 20;
	setFlame(gridFlame);
}

musnake::Grid::~Grid() {
	switch (objType) {
	case MU_GRID_OBJECT_TYPE_FOOD:
		delete obj.food;
		break;
	case MU_GRID_OBJECT_TYPE_SNAKE:
		delete obj.snake;
		break;
	}
}

inline void musnake::Grid::setSnake(Snake* snake) {
	if (snake) {
		objType = MU_GRID_OBJECT_TYPE_SNAKE;
		obj.snake = snake;
		snake->setGrid(this);
		snake->setPosition(&rect);
	}
	else {
		obj.snake = nullptr;
		objType = MU_GRID_OBJECT_TYPE_EMPTY;
	}
}

inline void musnake::Grid::setFood(Food* food) {
	if (food) {
		objType = MU_GRID_OBJECT_TYPE_FOOD;
		obj.food = food;
		food->setGrid(this);
		food->setPosition(&rect);
	}
	else {
		obj.food = nullptr;
		objType = MU_GRID_OBJECT_TYPE_EMPTY;
	}
}

inline musnake::Snake* musnake::Grid::getSnake() {
	if (objType == MU_GRID_OBJECT_TYPE_SNAKE)
		return obj.snake;
	else
		return nullptr;
}

inline musnake::Food* musnake::Grid::getFood() {
	if (objType == MU_GRID_OBJECT_TYPE_FOOD)
		return obj.food;
	else
		return nullptr;
}

inline SDL_Rect* musnake::Grid::getRect() {
	return &rect;
}

inline SDL_Point* musnake::Grid::getCenterPoint() {
	return &center;
}

inline void musnake::Grid::update() {
	Element::update();
	if (objType == GridObjectType::MU_GRID_OBJECT_TYPE_SNAKE) obj.snake->update();
}

inline void musnake::Grid::draw(SDL_Point* base) {
	if( objType == MU_GRID_OBJECT_TYPE_NONE ) return;
	if (objType == MU_GRID_OBJECT_TYPE_BLOCK) {
		SDL_Rect r = rect;
		r.x += base->x;
		r.y += base->y - 20;
		if (r.x > MU_WINDOW_WIDTH || r.y > MU_WINDOW_HEIGHT || r.x + 40 < 0 || r.y + 60 < 0) return;
		r.h = 60;
		gridBlockFlame->draw(&r);
		return;
	}
	if (objType == MU_GRID_OBJECT_TYPE_DARK) {
		SDL_Rect r = rect;
		r.x += base->x;
		r.y += base->y - 20;
		if (r.x > 800 || r.y > 600 || r.x + 40 < 0 || r.y + 60 < 0) return;
		r.h = 60;
		gridDarkFlame->draw(&r);
		return;
	}
	Element::draw(base);
	if (objType == MU_GRID_OBJECT_TYPE_SNAKE) obj.snake->draw(base);
	if (objType == MU_GRID_OBJECT_TYPE_FOOD) obj.food->draw(base);
}


inline void musnake::Snake::setGrid(Grid* grid) {
	this->grid = grid;
}

inline void musnake::Food::setGrid(Grid* grid) {
	this->grid = grid;
}

inline musnake::Grid* musnake::Food::getGrid() {
	return grid;
}

inline void musnake::Food::update() {
	Element::update();
}


// ============================================================



inline musnake::Food::Food(int type) {
	setFlame(foodFlame[type]);
}

inline musnake::Food::~Food() {

}


// ============================================================


inline musnake::Snake::Snake() {

}

inline musnake::Snake::Snake(int tailDir) {
	Snake::tailDir = tailDir;

	switch (tailDir) {
	case MU_SNAKE_DIRECT_UP:
		setFlame(snakeFlame[MU_SNAKE_FLAME_HEAD_0toUP]);
		break;
	case MU_SNAKE_DIRECT_RIGHT:
		setFlame(snakeFlame[MU_SNAKE_FLAME_HEAD_0toRIGHT]);
		break;
	case MU_SNAKE_DIRECT_DOWN:
		setFlame(snakeFlame[MU_SNAKE_FLAME_HEAD_0toDOWN]);
		break;
	case MU_SNAKE_DIRECT_LEFT:
		setFlame(snakeFlame[MU_SNAKE_FLAME_HEAD_0toLEFT]);
		break;
	}
}

inline void musnake::Snake::setHeadDir(int dir) {
	headDir = dir;
}

inline void musnake::Snake::setTailDir(int dir) {
	tailDir = dir;
}

inline void musnake::Snake::setNext(Snake* snake) {
	next = snake;
}

inline void musnake::Snake::setPrev(Snake* snake) {
	prev = snake;
}

inline musnake::Snake* musnake::Snake::getNext() {
	return next;
}

inline musnake::Snake* musnake::Snake::getPrev() {
	return prev;
}

inline musnake::Grid* musnake::Snake::getGrid() {
	return grid;
}

inline int musnake::Snake::getHeadDir() {
	return headDir;
}

inline int musnake::Snake::getTailDir() {
	return tailDir;
}

inline void musnake::Snake::turnBody() {
	switch (tailDir) {
	case MU_SNAKE_DIRECT_UP:
		switch (headDir) {
		case MU_SNAKE_DIRECT_RIGHT:
			setFlame(snakeFlame[MU_SNAKE_FLAME_HEAD_UPtoRIGHT]);
			break;
		case MU_SNAKE_DIRECT_DOWN:
			setFlame(snakeFlame[MU_SNAKE_FLAME_HEAD_UPtoDOWN]);
			break;
		case MU_SNAKE_DIRECT_LEFT:
			setFlame(snakeFlame[MU_SNAKE_FLAME_HEAD_UPtoLEFT]);
			break;
		}
		break;

	case MU_SNAKE_DIRECT_RIGHT:
		switch (headDir) {
		case MU_SNAKE_DIRECT_UP:
			setFlame(snakeFlame[MU_SNAKE_FLAME_HEAD_RIGHTtoUP]);
			break;
		case MU_SNAKE_DIRECT_DOWN:
			setFlame(snakeFlame[MU_SNAKE_FLAME_HEAD_RIGHTtoDOWN]);
			break;
		case MU_SNAKE_DIRECT_LEFT:
			setFlame(snakeFlame[MU_SNAKE_FLAME_HEAD_RIGHTtoLEFT]);
			break;
		}
		break;

	case MU_SNAKE_DIRECT_DOWN:
		switch (headDir) {
		case MU_SNAKE_DIRECT_UP:
			setFlame(snakeFlame[MU_SNAKE_FLAME_HEAD_DOWNtoUP]);
			break;
		case MU_SNAKE_DIRECT_RIGHT:
			setFlame(snakeFlame[MU_SNAKE_FLAME_HEAD_DOWNtoRIGHT]);
			break;
		case MU_SNAKE_DIRECT_LEFT:
			setFlame(snakeFlame[MU_SNAKE_FLAME_HEAD_DOWNtoLEFT]);
			break;
		}
		break;

	case MU_SNAKE_DIRECT_LEFT:
		switch (headDir) {
		case MU_SNAKE_DIRECT_UP:
			setFlame(snakeFlame[MU_SNAKE_FLAME_HEAD_LEFTtoUP]);
			break;
		case MU_SNAKE_DIRECT_RIGHT:
			setFlame(snakeFlame[MU_SNAKE_FLAME_HEAD_LEFTtoRIGHT]);
			break;
		case MU_SNAKE_DIRECT_DOWN:
			setFlame(snakeFlame[MU_SNAKE_FLAME_HEAD_LEFTtoDOWN]);
			break;
		}
		break;
	}
}

inline void musnake::Snake::turnBody(int headDir) {
	Snake::headDir = headDir;
	turnBody();
}

inline void musnake::Snake::turnTail() {
	switch (tailDir) {
	case MU_SNAKE_DIRECT_UP:
		switch (headDir) {
		case MU_SNAKE_DIRECT_RIGHT:
			setFlame(snakeFlame[MU_SNAKE_FLAME_TAIL_UPtoRIGHT]);
			break;
		case MU_SNAKE_DIRECT_DOWN:
			setFlame(snakeFlame[MU_SNAKE_FLAME_TAIL_UPtoDOWN]);
			break;
		case MU_SNAKE_DIRECT_LEFT:
			setFlame(snakeFlame[MU_SNAKE_FLAME_TAIL_UPtoLEFT]);
			break;
		}
		break;

	case MU_SNAKE_DIRECT_RIGHT:
		switch (headDir) {
		case MU_SNAKE_DIRECT_UP:
			setFlame(snakeFlame[MU_SNAKE_FLAME_TAIL_RIGHTtoUP]);
			break;
		case MU_SNAKE_DIRECT_DOWN:
			setFlame(snakeFlame[MU_SNAKE_FLAME_TAIL_RIGHTtoDOWN]);
			break;
		case MU_SNAKE_DIRECT_LEFT:
			setFlame(snakeFlame[MU_SNAKE_FLAME_TAIL_RIGHTtoLEFT]);
			break;
		}
		break;

	case MU_SNAKE_DIRECT_DOWN:
		switch (headDir) {
		case MU_SNAKE_DIRECT_UP:
			setFlame(snakeFlame[MU_SNAKE_FLAME_TAIL_DOWNtoUP]);
			break;
		case MU_SNAKE_DIRECT_RIGHT:
			setFlame(snakeFlame[MU_SNAKE_FLAME_TAIL_DOWNtoRIGHT]);
			break;
		case MU_SNAKE_DIRECT_LEFT:
			setFlame(snakeFlame[MU_SNAKE_FLAME_TAIL_DOWNtoLEFT]);
			break;
		}
		break;

	case MU_SNAKE_DIRECT_LEFT:
		switch (headDir) {
		case MU_SNAKE_DIRECT_UP:
			setFlame(snakeFlame[MU_SNAKE_FLAME_TAIL_LEFTtoUP]);
			break;
		case MU_SNAKE_DIRECT_RIGHT:
			setFlame(snakeFlame[MU_SNAKE_FLAME_TAIL_LEFTtoRIGHT]);
			break;
		case MU_SNAKE_DIRECT_DOWN:
			setFlame(snakeFlame[MU_SNAKE_FLAME_TAIL_LEFTtoDOWN]);
			break;
		}
		break;
	}

	setTailDir(MU_SNAKE_DIRECT_NONE);
	setNext(nullptr);
}

inline void musnake::Snake::shakeTail() {
	switch (headDir) {
	case MU_SNAKE_DIRECT_UP:
		setFlame(snakeFlame[MU_SNAKE_FLAME_TAIL_UPshake]);
		break;
	case MU_SNAKE_DIRECT_RIGHT:
		setFlame(snakeFlame[MU_SNAKE_FLAME_TAIL_RIGHTshake]);
		break;
	case MU_SNAKE_DIRECT_DOWN:
		setFlame(snakeFlame[MU_SNAKE_FLAME_TAIL_DOWNshake]);
		break;
	case MU_SNAKE_DIRECT_LEFT:
		setFlame(snakeFlame[MU_SNAKE_FLAME_TAIL_LEFTshake]);
		break;
	}
}


// ============================================================


typedef struct _Note {
	long long time;
	struct _Note* next;
}Note;

Note* newNote(long long time) {
	Note* note = new Note;
	note->time = time;
	note->next = nullptr;
	return note;
}

void addNote(Note** list, Note* note) {
	Note* np = *list;
	if (!np) {
		*list = note;
	}
	else {
		while (np->next) np = np->next;
		np->next = note;
	}
}

void clearNotes(Note** list) {
	Note* np;
	while (np = *list) {
		*list = np->next;
		delete np;
	}
}
