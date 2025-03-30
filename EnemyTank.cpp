#include "EnemyTank.h"

EnemyTank::EnemyTank() {
        type_move = "down";
        tank_move = false;
}

EnemyTank :: EnemyTank(int startX, int startY, int dirX_ , int dirY_ ) {
        type_move = "down";
        tank_move = false;
        moveDelay = 8;
        shootDelay = 5;
        x = startX;
        y = startY;
        rect = {x+4, y+4, 2*TILE_SIZE-4, 2*TILE_SIZE-4};
        dirX = dirX_;
        dirY = dirY_;
        active = true;
}

void EnemyTank :: false_active(SDL_Renderer* renderer) {
        for(int i = 0; i<2; i++) {
            SDL_RenderCopy(renderer, TextureManager["enemytankfalse"], &frame[i], &rect);
            SDL_RenderPresent(renderer);
            SDL_Delay(60);
        }
}

void EnemyTank :: e_move(const vector<Wall>& bricks, const vector<Wall>& rocks, int playerX, int playerY) {
        if(--moveDelay > 0) return;
        moveDelay = 12;

        if(rand() % 100 < 30) {
            int dx = playerX - x;
            int dy = playerY - y;
            if(abs(dx) > abs(dy)) {
                dirX = (dx > 0) ? 16: -16;
                dirY = 0;
            }
            else {
                dirX = 0;
                dirY = (dy > 0) ? 16 : -16;
            }
        } else if(rand()%100 >= 30 && rand() %100 <60) {
            switch (rand() % 4) {
                case 0: dirX = 0; dirY = -16; break;
                case 1: dirX = 0; dirY = 16; break;
                case 2: dirX = -16; dirY = 0; break;
                case 3: dirX = 16; dirY = 0; break;
            }
        }

        int newX = x + dirX;
        int newY = y + dirY;
        SDL_Rect newRect = {newX, newY, 2*TILE_SIZE-8, 2*TILE_SIZE-8};

        bool check = false;
        for(const auto& brick : bricks) {
            if(brick.active && SDL_HasIntersection(&newRect, &brick.rect)) {
                check = true;
                break;
            }
        }

        if(!check) {
            for(const auto& rock: rocks) {
                if(rock.active && SDL_HasIntersection(&newRect, &rock.rect)) {
                check = true;
                break;
                }
            }
        }

        if(!check) {
            SDL_Rect p_Rect = {playerX, playerY, 2*TILE_SIZE, 2*TILE_SIZE};
            if(SDL_HasIntersection(&newRect, &p_Rect)) {
                check = true;
            }
        }

        if(newX < TILE_SIZE*2 || newX > SCREEN_WIDTH - TILE_SIZE*8 ||
                newY < TILE_SIZE || newY > SCREEN_HEIGHT - TILE_SIZE*3)  {
                    check = true;
        }


        if (!check) {
            if(newX < x) type_move = "left";
            else if(newX > x)  type_move = "right";

            if(newY > y) type_move = "down";
            else if(newY < y) type_move = "up";
            x = newX;
            y = newY;
            rect.x = x;
            rect.y = y;
            tank_move = true;
        }
        else {
            if(dirX > 0) type_move = "right";
            else if(dirX < 0) type_move = "left";
            if(dirY > 0) type_move = "down";
            else if(dirY < 0) type_move = "up";
            tank_move = false;
        }
}

void EnemyTank :: e_shoot() {
        if(-- shootDelay > 0) return;
        shootDelay = 5;
        bullets.push_back(Bullet(x+TILE_SIZE -10, y+TILE_SIZE - 5, this->dirX/2, this->dirY/2));
    }

void EnemyTank :: e_updateBullets() {
        for(auto& bullet : bullets) {
            bullet.move();
        }
        bullets.erase(remove_if(bullets.begin(), bullets.end(),
                                                [](Bullet& b) { return !b.active; } ), bullets.end());
    }

void EnemyTank :: e_render(SDL_Renderer* renderer){

        if(tank_move) {
        if(type_move == "up") {
            SDL_RenderCopy(renderer, TextureManager["enemytankup"], &frame[up/10], &rect);
            up++;
            if(up / 10 >= 2) up = 0;
        }

        if(type_move == "down") {
            SDL_RenderCopy(renderer, TextureManager["enemytankdown"], &frame[down/10], &rect);
            down++;
            if(down/10 >= 2) down = 0;
        }

        if(type_move == "right") {
            SDL_RenderCopy(renderer, TextureManager["enemytankright"], &frame[right/10], &rect);
            right++;
            if(right / 10 >= 2) right = 0;
        }

        if(type_move == "left") {
            SDL_RenderCopy(renderer, TextureManager["enemytankleft"], &frame[left/10], &rect);
            left++;
            if(left/10 >= 2 ) left = 0;
        }
    } else {
        if(type_move == "up") {
            SDL_RenderCopy(renderer, TextureManager["enemytankup"], &frame[0], &rect);
        }

        if(type_move == "down") {
            SDL_RenderCopy(renderer, TextureManager["enemytankdown"], &frame[0], &rect);
        }

        if(type_move == "right") {
            SDL_RenderCopy(renderer, TextureManager["enemytankright"], &frame[0], &rect);
        }

        if(type_move == "left") {
            SDL_RenderCopy(renderer, TextureManager["enemytankleft"], &frame[0], &rect);
        }
    }

        for(auto &bullet : bullets) {
            bullet.render(renderer);
        }
    }
