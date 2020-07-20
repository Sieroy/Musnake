#include"SDL.h"

SDL_Window* window;
SDL_Renderer* render;

int main(int argc, const char* argv[]) {
	SDL_AudioSpec* audio;
	Uint32 audio_length;
	Uint8* audio_buffer;

	SDL_Init(SDL_INIT_EVERYTHING);

	window = SDL_CreateWindow("Musnake", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 800, 600, SDL_WINDOW_RESIZABLE);
	render = SDL_CreateRenderer(window, -1, 0);

	SDL_LoadWAV("C:\\Users\\Sieroy\\Desktop\\Best Of My Love.wav", audio, &audio_buffer, &audio_length);

	SDL_DestroyRenderer(render);
	SDL_DestroyWindow(window);
	SDL_Quit();
}