#ifndef PLAYERTANK_H_
#define PLAYERTANK_H_

#include "CommonFuntion.h"
#include "EnemyTank.h"
#include "Wall.h"

class PlayerTank{
public:

    bool isInvincible = false;
    Uint32 isInvincibleStart = 0;
    const Uint32 invincible_Time = 3000;

    int x, y;
    string type_input;
    bool tank_move;
    int invincible_Delay = 0;
    int right = 0;
    int left = 0;
    int up = 0;
    int down = 0;
    SDL_Rect frame[2] = {{0, 0, 2*TILE_SIZE, 2*TILE_SIZE},
                                     {48, 0, 2*TILE_SIZE, 2*TILE_SIZE}};
    int dirX, dirY;                             //direction
    SDL_Rect rect;
    vector<Bullet> bullets;

    PlayerTank();
    PlayerTank(int startX, int startY);

    void setInvincible(Uint32 currentTime);
    void p_move(bool tank_move, string type_input,int dx, int dy, const vector<Wall>& walls, const vector<Wall> rocks, const vector<EnemyTank>& enemies);
    void p_shoot();
    void p_updateBullets();
    void p_render(SDL_Renderer* renderer);
};

#endif // PLAYERTANK_H_
