#define _CRT_SECURE_NO_WARNINGS

#include"SDL.h"
#include"SDL_main.h"
#include"SDL_image.h"
#include"SDL_mixer.h"

#include"MU_declaration.h"
#include"MU_path.h"
#include"MU_game.h"

using namespace musnake;

SDL_Window* window;
SDL_Renderer* render;

int main(int argc, char* argv[]) {
	SDL_Event evt;

	initPath(argv[0]);
	SDL_Init(SDL_INIT_EVERYTHING);
	IMG_Init(IMG_INIT_PNG);
	Mix_Init(MIX_INIT_MP3);

	window = SDL_CreateWindow("Musnake", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 800, 600, SDL_WINDOW_RESIZABLE);
	render = SDL_CreateRenderer(window, -1, 0);
	SDL_GetDisplayUsableBounds(0, &drawableScreenArea);
	Mix_OpenAudio(22050, MIX_DEFAULT_FORMAT, 2, 4096);
	/*
	while (musnakeState) {
		while (SDL_PollEvent(&evt)) {
			switch (evt.type) {
			case SDL_QUIT:
				musnakeState = false;
				break;
			}
		}
	}
	*/
	thisGame = new Game();
	thisGame->setRenderer(render);
	thisGame->init((char*)"test");
	thisGame->run();

	IMG_Quit();
	Mix_Quit();
	SDL_DestroyRenderer(render);
	SDL_DestroyWindow(window);
	SDL_Quit();

	return 0;
}