#include "BaseObject.h"

BaseObject::BaseObject() {
    p_object_ = NULL;
    rect_.x = 0;
    rect_.y = 0;
    rect_.w = 0;
    rect_.h = 0;
}

BaseObject::~BaseObject() {
    Free();
}

bool BaseObject::loadImg(const char* path, SDL_Renderer* renderer) {
    SDL_Surface* load_surface = IMG_Load(path);
    if(load_surface == NULL) {
        cerr << "Unable to load " << path << IMG_GetError() << endl;
        return false;
    }
    p_object_ = SDL_CreateTextureFromSurface(renderer, load_surface);                    //(renderer, load_surface);
    if(p_object_ == NULL) {
        cerr << "Unable to create texture from " << path << SDL_GetError() << endl;
        return false;
    }
    rect_.w = load_surface->w;
    rect_.h = load_surface->h;
    SDL_FreeSurface(load_surface);
    return true;
}

void BaseObject::Render(SDL_Renderer* renderer)
{
    cout << rect_.x << " " << rect_.y << " " <<  rect_.w << " " << rect_.h <<  endl;
    SDL_RenderCopy(renderer, p_object_, NULL, &rect_);                          //ve doi tuong
}

void BaseObject::Free() {
    if(p_object_ != NULL) {
        SDL_DestroyTexture(p_object_);
        p_object_ = NULL;
        rect_.w = 0;
        rect_.h = 0;
    }
}
