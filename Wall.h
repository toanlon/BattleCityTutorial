#ifndef WALL_H_
#define WALL_H_

#include"CommonFuntion.h"

class Wall {
public:
    int x,y;
    SDL_Rect rect;
    string type;
    bool active;

    Wall(int X, int Y, string type_);

    void render(SDL_Renderer* renderer);
};


#endif // WALL_H_

