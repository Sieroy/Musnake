#include"SDL.h"
#include"SDL_main.h"

SDL_Window* window;
SDL_Renderer* render;
bool running = true;

int main(int argc, char* argv[]) {
	SDL_Event evt;

	SDL_Init(SDL_INIT_EVERYTHING);

	window = SDL_CreateWindow("Musnake", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 800, 600, SDL_WINDOW_RESIZABLE);
	render = SDL_CreateRenderer(window, -1, 0);

	while (running) {
		while (SDL_PollEvent(&evt)) {
			switch (evt.type) {
			case SDL_QUIT:
				running = false;
			}
		}
	}

	SDL_DestroyRenderer(render);
	SDL_DestroyWindow(window);
	SDL_Quit();

	return 0;
}