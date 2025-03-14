#include <SDL.h>
#include <iostream>
#include <vector>
#include <algorithm>
using namespace std;

const int SCREEN_WIDTH = 800;
const int SCREEN_HEIGHT = 600;
const int TILE_SIZE = 40;
const int MAP_WIDTH =  SCREEN_WIDTH / TILE_SIZE;
const int MAP_HEIGHT =  SCREEN_HEIGHT/ TILE_SIZE;

class Wall {
public:
    int x,y;
    SDL_Rect rect;
    bool active;

    Wall(int X, int Y);

    void render(SDL_Renderer* renderer);
};

class Bullet {
public:
    int x, y;
    int dx, dy;
    SDL_Rect rect;
    bool active;

    Bullet(int startX, int startY, int dirX, int dirY) {
        x = startX;
        y = startY;
        dx = dirX;
        dy = dirY;
        active = true;
        rect = {x, y, 10, 10};              // dan hinh vuong;
    }

    void move();
    void render(SDL_Renderer* renderer);
};

class PlayerTank{
public:
    int x, y;
    int dirX, dirY;                             //direction
    SDL_Rect rect;
    vector<Bullet> bullets;

    PlayerTank(int startX, int startY) {
        x = startX;
        y = startY;
        rect = {x, y, TILE_SIZE, TILE_SIZE};
        dirX = 0;
        dirY = -1;                                              // mac dinh tank di len (truc y huong xuong)
    }

    void move(int dx, int dy, const vector<Wall>& walls);
    void shoot();
    void updateBullets();
    void render(SDL_Renderer* renderer);
};

class Game{
public:
    SDL_Window* window;
    SDL_Renderer* renderer;
    bool running;
    vector<Wall> walls;
    PlayerTank player{((MAP_WIDTH -1)/2)*TILE_SIZE, (MAP_HEIGHT - 2)*TILE_SIZE};

    Game();
    void generateWalls();
    void render(SDL_Renderer* renderer);
    void handleEvents();
    void update();        //kiem soat logic game
    void run();
    ~Game();
};

Game::Game() {
    running = true;
    if(SDL_Init(SDL_INIT_VIDEO) < 0) {
        cerr << "SDL could not initialize! SDL_Eror: " << SDL_GetError() << endl;
        running = false;
    }
    window = SDL_CreateWindow("Battle City", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
    if(!window) {
        cerr << "Window could not be created! SDL_Error: " << SDL_GetError() << endl;
        running = false;
    }
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if(!renderer) {
        cerr << "Renderer could not be created! SDL_Error: " << SDL_GetError() << endl;
        running = false;
    }
    generateWalls();
}

void Game::render(SDL_Renderer* renderer) {
    // ve nen
    SDL_SetRenderDrawColor(renderer, 128 , 128, 128, 255);      // nen xam
    SDL_RenderClear(renderer);                                                   // xoa nen va ve mau xam da chon len

    SDL_SetRenderDrawColor(renderer, 0, 0, 0 , 255);                // nen den
    for(int i = 1; i < MAP_HEIGHT-1; i++) {
        for(int j = 1; j< MAP_WIDTH-1; j++) {
            SDL_Rect tile = { j*TILE_SIZE, i*TILE_SIZE, TILE_SIZE, TILE_SIZE };         //
            SDL_RenderFillRect(renderer, &tile);                             // to mau cac hinh chu nhat
        }
    }
    // ve wall
    for(int i = 0; i<(int)walls.size(); i++) {
        walls[i].render(renderer);
    }
    player.render(renderer);                    // ve xe tang cua nguoi choi

    SDL_RenderPresent(renderer);                                               // hien thi len man hinh
}

void Game::handleEvents() {
    SDL_Event event;
    while(SDL_PollEvent(&event)) {
        if(event.type == SDL_QUIT) {
            running =false;
        }   else if(event.type = SDL_KEYDOWN) {
            switch(event.key.keysym.sym) {
                case SDLK_UP: player.move(0, -5, walls); break;
                case SDLK_DOWN: player.move(0, 5, walls); break;
                case SDLK_LEFT: player.move(-5, 0, walls); break;
                case SDLK_RIGHT: player.move(5, 0, walls); break;
                case SDLK_SPACE: player.shoot() ; break;
            }

        }
    }
}

void Game::run() {
    while(running) {
        handleEvents();
        update();
        render(renderer);
        SDL_Delay(16);
    }
}

Wall::Wall(int X, int Y)
{
    x = X;
    y = Y;
    active = true;
    rect = {x, y, TILE_SIZE, TILE_SIZE};
}

void Wall::render( SDL_Renderer* renderer){
    if(active) {
        SDL_SetRenderDrawColor(renderer, 150, 75, 0 , 255);
        SDL_RenderFillRect(renderer, &rect);
    }
}

void Game::generateWalls() {
    for(int i = 3; i<MAP_HEIGHT - 2; i+= 2) {
        for(int j = 3; j< MAP_WIDTH-3; j+=2) {
            Wall w = Wall(j*TILE_SIZE, i*TILE_SIZE);
            walls.push_back(w);
        }
    }
}

void Bullet::move()
{
    x += dx;
    y += dy;
    rect.x = x;
    rect.y = y;
    if(x < TILE_SIZE || x>= SCREEN_WIDTH - TILE_SIZE ||
       y < TILE_SIZE || y > SCREEN_HEIGHT - TILE_SIZE)    {
                active = false;
       }
}

void Bullet::render(SDL_Renderer* renderer) {
    if(active) {
        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);       // dan mau trang
        SDL_RenderFillRect(renderer, &rect);
    }
}

void PlayerTank::shoot() {
    bullets.push_back(Bullet(x + TILE_SIZE/2 -5, y+ TILE_SIZE/2 -5,this->dirX, this->dirY));
}

void PlayerTank::updateBullets() {
    for(auto& bullet :  bullets) {
        bullet.move();
    }
    bullets.erase(remove_if(bullets.begin(), bullets.end(),                 // di chuyen cac vien dan xuong cuoi mang roi xoa
                            [](Bullet& b) { return !b.active; }) , bullets.end());
}

void PlayerTank::render(SDL_Renderer* renderer){
    SDL_SetRenderDrawColor(renderer, 255, 255, 0, 255);
    SDL_RenderFillRect(renderer, &rect);
    for(auto &bullet: bullets) {
        bullet.render(renderer);
    }
}

void PlayerTank::move(int dx, int dy, const vector<Wall>& walls) {
    int newX = x+dx;
    int newY = y+dy;
    this->dirX = dx;
    this->dirY = dy;

    SDL_Rect newRect = {newX, newY, TILE_SIZE, TILE_SIZE};
    for(int i = 0; i<walls.size(); i++) {
        if(walls[i].active && SDL_HasIntersection(&newRect, &walls[i].rect))            //kiem tra su va cham voi tuong
            return;
    }

    if(newX >= TILE_SIZE && newX <= SCREEN_WIDTH - TILE_SIZE*2 &&
       newY >= TILE_SIZE && newY <= SCREEN_HEIGHT - TILE_SIZE*2)  {
        x = newX;
        y = newY;
        rect.x = x;
        rect.y = y;
       }
}

void Game::update() {
    player.updateBullets();
    for(auto& bullet : player.bullets) {
        for(auto& wall : walls) {
            if(wall.active && SDL_HasIntersection(&bullet.rect, & wall.rect)) {
                wall.active = false;
                bullet.active = false;
                break;
            }
        }
    }
}

Game::~Game() {
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
}

int main(int argc, char* argv[])
{
    Game game;
    if(game.running) {
        game.run();
    }
    return 0;
}
