    #ifndef COMMON_FUNTION_H_
    #define COMMON_FUNTION_H_

    #include <SDL.h>
    #include <SDL_image.h>
    #include <iostream>
    #include <vector>
    #include <algorithm>
    #include <cstdlib>
    #include <ctime>
    #include <map>
    #include <cmath>
    #include <fstream>
    using namespace std;

    extern SDL_Window* window;
    extern SDL_Renderer* renderer;
    extern bool running;

    const int FRAME_PER_SECOND = 60;
    const int TILE_SIZE = 12;
    const int MAP_WIDTH = 60;
    const int MAP_HEIGHT = 56;
    const int  SCREEN_WIDTH = MAP_WIDTH*TILE_SIZE;
    const int  SCREEN_HEIGHT = MAP_HEIGHT*TILE_SIZE;

    SDL_Texture* LoadTexture(const char* path, SDL_Renderer* renderer);

     #endif // COMMON_FUNTION_H_
