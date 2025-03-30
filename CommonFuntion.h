    #ifndef COMMON_FUNTION_H_
    #define COMMON_FUNTION_H_

    #include <SDL.h>
    #include <SDL_image.h>
    #include <SDL_mixer.h>
    #include <SDL_ttf.h>
    #include <iostream>
    #include <vector>
    #include <algorithm>
    #include <cstdlib>
    #include <ctime>
    #include <map>
    #include <cmath>
    #include <fstream>
    #include <sstream>
    using namespace std;

    extern SDL_Window* window;
    extern SDL_Renderer* renderer;
    extern TTF_Font* scoreFont;
    extern bool running;
    extern map<string, SDL_Texture*> TextureManager;

    const int FRAME_PER_SECOND = 60;
    const int TILE_SIZE = 24;
    const int MAP_WIDTH = 34;
    const int MAP_HEIGHT = 28;
    const int  SCREEN_WIDTH = MAP_WIDTH*TILE_SIZE;
    const int  SCREEN_HEIGHT = MAP_HEIGHT*TILE_SIZE;

    SDL_Texture* LoadTexture(const char* path, SDL_Renderer* renderer);
    SDL_Texture* LoadTTF(TTF_Font* scoreFont,const char* text,SDL_Color textColor);

     #endif // COMMON_FUNTION_H_
