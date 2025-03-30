#include "CommonFuntion.h"

SDL_Window* window = NULL;
SDL_Renderer* renderer = NULL;
TTF_Font* scoreFont = NULL;
map<string, SDL_Texture*> TextureManager;
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

SDL_Texture* LoadTTF(TTF_Font* scoreFont,const char* text,SDL_Color textColor) {
    SDL_Surface* textSurface = TTF_RenderText_Blended(scoreFont, text, textColor);
    if(textSurface == NULL) {
        cerr << "Unable to render text surface! TTF_Error: " << TTF_GetError() << endl;
    } else {
        // Tạo texture từ surface
        SDL_Texture* textTexture = SDL_CreateTextureFromSurface(renderer, textSurface);
        if(textTexture == NULL) {
            cerr << "Unable to create texture from rendered text! SDL_Error: " << SDL_GetError() << endl;
        }
        SDL_FreeSurface(textSurface);
        return textTexture;
    }
    cout << "Error";
    return NULL;
}
