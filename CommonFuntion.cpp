#include "CommonFuntion.h"

SDL_Window* window = NULL;
SDL_Renderer* renderer = NULL;
bool running = true;

SDL_Texture* LoadTexture(const char* path, SDL_Renderer* renderer) {
    SDL_Surface* surface = IMG_Load(path);
    if (!surface) return nullptr;

    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
    if(!texture) cerr << "Can't load texture " << path << endl;
    else cout << "secces load " << path << endl;
    SDL_FreeSurface(surface);
    return texture;
}
