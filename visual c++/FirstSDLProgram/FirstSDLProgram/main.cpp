#include <iostream>
#include <SDL/SDL.H>
#pragma comment(lib, "SDL.lib")
#pragma comment(lib, "SDLmain.lib")
int main(int argc, char* argv[]){
	SDL_Surface* hello = NULL;
	SDL_Surface* screen = NULL;
	SDL_Init(SDL_INIT_EVERYTHING);
	screen = SDL_SetVideoMode(640, 480, 32, SDL_SWSURFACE);
	SDL_Flip(screen);
	system("pause");
	SDL_Quit();
	
	return 0;
}