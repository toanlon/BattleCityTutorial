#ifndef ENEMYTANK_H_
#define ENEMYTANK_H_

#include"CommonFuntion.h"
#include "Bullet.h"
#include "Wall.h"

class EnemyTank {
public:
    int x, y;
     SDL_Rect frame[2] = {{0, 0, 2*TILE_SIZE, 2*TILE_SIZE},
                                     {48, 0, 2*TILE_SIZE, 2*TILE_SIZE}};
    string type_move;
    bool tank_move;
    int right = 0;
    int left = 0;
    int up = 0;
    int down = 0;
    int dirX, dirY;
    int moveDelay, shootDelay;
    SDL_Rect rect;
    bool active;
    vector<Bullet> bullets;

    EnemyTank();

    EnemyTank(int startX, int startY, int dirX_ = 0, int dirY_ = 12);

    void false_active(SDL_Renderer* renderer);
    void e_move(const vector<Wall>& bricks, const vector<Wall>& rocks, int playerX, int playerY);

    void e_shoot();

    void e_updateBullets();

    void e_render(SDL_Renderer* renderer);
};

#endif // ENEMYTANK_H_
