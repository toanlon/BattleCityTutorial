#include "Bullet.h"

 Bullet :: Bullet(int startX, int startY, int dirX, int dirY) {
        x = startX;
        y = startY;
        dx = dirX;
        dy = dirY;
        active = true;
        rect = {x, y, 10, 10};              // dan hinh vuong;
    }

void Bullet::move() {
    x += dx;
    y += dy;
    rect.x = x;
    rect.y = y;
    if(x < TILE_SIZE*2 || x>= SCREEN_WIDTH - TILE_SIZE*6 ||
       y < TILE_SIZE*1 || y > SCREEN_HEIGHT - TILE_SIZE*1)  {
                active = false;
       }
}

void Bullet::render(SDL_Renderer* renderer) {
    if(active) {
        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);       // dan mau trang
        SDL_RenderFillRect(renderer, &rect);
    }
}
