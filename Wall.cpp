#include "Wall.h"

Wall::Wall(int X, int Y, string type_)
{
    type = type_;
    x = X;
    y = Y;
    active = true;
    rect = {x, y, TILE_SIZE, TILE_SIZE};
}

void Wall::render( SDL_Renderer* renderer){
    if(active) {
        if(type == "brick") SDL_RenderCopy(renderer, TextureManager["brick"], NULL, &rect);
        if(type == "rock") SDL_RenderCopy(renderer, TextureManager["rock"], NULL, &rect);
    }
}
