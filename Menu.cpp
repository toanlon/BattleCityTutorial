#include "Menu.h"
#include <utility>
static pair<string, SDL_Texture*> TextureManagerMenu[10];

Menu::Menu(SDL_Renderer* renderer) {
    bool load = false;
    for(int i = 0; i< 10; i++) {
        if(TextureManagerMenu[i].second != NULL) load = true;
    }
    if(!load) {
        TextureManagerMenu[5] = {"background", LoadTexture("image/menubackgr.jpg", renderer) };
        TextureManagerMenu[6] = {"stringbattlecity", LoadTexture("image/stringbattlecity.png", renderer) };
        TextureManagerMenu[0] = {"mode1p", LoadTexture("image/tt1player.png", renderer)};
        TextureManagerMenu[1] = {"mode2p", LoadTexture("image/tt2player.png", renderer)};
        TextureManagerMenu[2] = {"ranking", LoadTexture("image/ranking.png", renderer)};
        TextureManagerMenu[3] = {"setting", LoadTexture("image/settings.png", renderer)};
        TextureManagerMenu[4] ={"exit", LoadTexture("image/exit.png", renderer)};
        TextureManagerMenu[7] = {"pauseBGR", LoadTexture("image/menuPauseBackgr.jpg", renderer)};
        TextureManagerMenu[8] = {"pausePlay", LoadTexture("image/menuPausePlay.png", renderer)};
        TextureManagerMenu[9] = {"pauseMenu", LoadTexture("image/menuPauseBackToMenu.png", renderer)};
    }
    loadMenu(renderer);
}

Menu::~Menu() {
    background.Free();
    for(auto& button : buttons) {
        button.Free();
    }
}

void Menu::loadMenu(SDL_Renderer* renderer) {

    const int BUTTON_WIDTH = 200;
    const int BUTTON_HEIGHT = 50;
    const int START_Y = 320;
    const int SPACING = 20;

    SDL_RenderClear(renderer);
    SDL_Rect bgr_menu = {0, 0, SCREEN_WIDTH, SCREEN_HEIGHT};
    SDL_Rect stringbattlecity = {180, 60, 450, 250};
    SDL_RenderCopy(renderer, TextureManagerMenu[5].second, NULL, &bgr_menu);
    SDL_RenderCopy(renderer, TextureManagerMenu[6].second, NULL, &stringbattlecity);
    for(int i = 0; i < 5; i++) {
        SDL_Rect button_rect = {(SCREEN_WIDTH - BUTTON_WIDTH)/2,
            START_Y + i*(BUTTON_HEIGHT + SPACING),
            BUTTON_WIDTH,
            BUTTON_HEIGHT};
        SDL_RenderCopy(renderer, TextureManagerMenu[i].second, NULL, &button_rect);
        buttonRects.push_back(button_rect);
    }

    SDL_RenderPresent(renderer);

}
Menu::MenuResult Menu::chooseMenu()
{
    SDL_Event event;
    SDL_Delay(10);
    bool isRun = true;
    while(isRun) {
        while(SDL_PollEvent(&event)) {
            switch(event.type) {
            case SDL_QUIT:
                return EXIT;
            case SDL_MOUSEBUTTONDOWN:
                if(event.button.button == SDL_BUTTON_LEFT) {
                    MenuResult result = handleClick(event.button.x, event.button.y);
                    if(result != NOTHING) return result;
                }
                break;
            }
        }
        SDL_Delay(10);
    }

    return EXIT;
}

void Menu::renderPauseMenu(SDL_Renderer* renderer) {
    // Giả sử bạn có texture cho background pause và các nút:
    SDL_Rect fullScreen = {0, 0, SCREEN_WIDTH, SCREEN_HEIGHT};
    SDL_RenderClear(renderer);
    // Vẽ background pause (có thể là một hình ảnh với màu đen và hiệu ứng)
    SDL_RenderCopy(renderer, TextureManagerMenu[7].second, NULL, &fullScreen);

    // Vẽ nút Resume ở góc dưới hoặc vị trí phù hợp
    SDL_Rect resumeRect = {SCREEN_WIDTH/2 - 100, SCREEN_HEIGHT/2 - 60, 200, 50};
    SDL_RenderCopy(renderer, TextureManagerMenu[8].second, NULL, &resumeRect);

    // Vẽ nút Menu (để quay về menu chính)
    SDL_Rect menuRect = {SCREEN_WIDTH/2 - 100, SCREEN_HEIGHT/2 + 10, 200, 50};
    SDL_RenderCopy(renderer, TextureManagerMenu[9].second, NULL, &menuRect);

    SDL_RenderPresent(renderer);
}

Menu::MenuResult Menu::handleClick(int x, int y) {
    for(int i = 0; i<buttonRects.size(); i++ ) {
        if(x >= buttonRects[i].x && x<= buttonRects[i].x +buttonRects[i].w &&
           y>= buttonRects[i].y && y <= buttonRects[i].y + buttonRects[i].h) {
            switch(i) {
                case 0 : return ONE_PLAYER;
                case 1 : return TWO_PLAYER;
                case 2 : return RANKING;
                case 3 : return SETTING;
                case 4 : return EXIT;
            }
        }
    }
    return NOTHING;
}
