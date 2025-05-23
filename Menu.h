#ifndef MENU_H_
#define MENU_H_

#include "CommonFuntion.h"
#include "BaseObject.h"

class Menu {
public:
    enum MenuResult {
        ONE_PLAYER = 0,
        TWO_PLAYER = 1,
        RANKING = 2,
        SETTING = 3,
        EXIT = 4,
        NOTHING = 5
    };

    Menu(SDL_Renderer* renderer);
    ~Menu();

    MenuResult chooseMenu();

    BaseObject background;
    vector<BaseObject> buttons;         // Danh sách nút
    vector<SDL_Rect> buttonRects;       // Vùng click của nút

    void loadMenu(SDL_Renderer* renderer);
    void renderPauseMenu(SDL_Renderer* renderer);
    MenuResult handleClick(int x, int y);
};

#endif // MENU_H_
