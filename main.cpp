#include "CommonFuntion.h"
#include "Menu.h"
#include "ImpTimer.h"
map<string, SDL_Texture*> TextureManager;


class Wall {
public:
    int x,y;
    SDL_Rect rect;
    string type;
    bool active;

    Wall(int X, int Y, string type_);

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

class EnemyTank {
public:
    int x, y;
     SDL_Rect frame[2] = {{0, 0, 4*TILE_SIZE, 4*TILE_SIZE},
                                     {48, 0, 4*TILE_SIZE, 4*TILE_SIZE}};
    string type_input;
    bool tank_move;
    int dirX, dirY;
    int moveDelay, shootDelay;
    SDL_Rect rect;
    bool active;
    vector<Bullet> bullets;

    EnemyTank(int startX, int startY) {
        moveDelay = 15;
        shootDelay = 5;
        x = startX;
        y = startY;
        rect = {x, y, 4*TILE_SIZE, 4*TILE_SIZE};
        dirX = 0;
        dirY = 1;
        active = true;
    }

    void false_active(SDL_Renderer* renderer) {
        for(int i = 0; i<2; i++) {
            SDL_RenderCopy(renderer, TextureManager["enemytnakfalse"], &frame[i], &rect);
            SDL_RenderPresent(renderer);
        }

    }
    void move(const vector<Wall>& walls, const vector<Wall>& rocks, int playerX, int playerY) {
        if(--moveDelay > 0) return;
        moveDelay = 15;

        if(rand() % 100 < 30) {
            int dx = playerX - x;
            int dy = playerY - y;
            if(abs(dx) > abs(dy)) {
                dirX = (dx > 0) ? 12: -12;
                dirY = 0;
            }
            else {
                dirX = 0;
                dirY = (dy > 0) ? 12 : -12;
            }
        } else {
            switch (rand() % 4) {
                case 0: dirX = 0; dirY = -12; break;
                case 1: dirX = 0; dirY = 12; break;
                case 2: dirX = -12; dirY = 0; break;
                case 3: dirX = 12; dirY = 0; break;
            }
        }

        int newX = x + dirX;
        int newY = y + dirY;
        SDL_Rect newRect = {newX, newY, 4*TILE_SIZE, 4*TILE_SIZE};

        bool check = false;
        for(const auto& wall: walls) {
            if(wall.active && SDL_HasIntersection(&newRect, &wall.rect)) {
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
        if(newX < TILE_SIZE*4 || newX > SCREEN_WIDTH - TILE_SIZE*8 ||
                newY < TILE_SIZE*2 || newY > SCREEN_HEIGHT - TILE_SIZE*6)  {
                    check = true;
        }


        if (!check) {
            x = newX;
            y = newY;
            rect.x = x;
            rect.y = y;
        }
    }

    void shoot() {
        if(-- shootDelay > 0) return;
        shootDelay = 5;
        bullets.push_back(Bullet(x+TILE_SIZE*2 -5, y+TILE_SIZE*2 - 5, this->dirX, this->dirY));
    }

    void updateBullets() {
        for(auto& bullet : bullets) {
            bullet.move();
        }
        bullets.erase(remove_if(bullets.begin(), bullets.end(),
                                                [](Bullet& b) { return !b.active; } ), bullets.end());
    }

    void render(SDL_Renderer* renderer){
        SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
        SDL_RenderFillRect(renderer, &rect);
        for(auto &bullet : bullets) {
            bullet.render(renderer);
        }
    }
};

class Eagle {
public:
    int x;
    int y;
    bool active = true;
    SDL_Rect rect;

    Eagle() {};

    Eagle(int X, int Y) {
        x = X;
        y = Y;
        rect = {x, y, 2*TILE_SIZE, 2*TILE_SIZE};
    }

    void render(SDL_Renderer* renderer) {
        SDL_RenderCopy(renderer, TextureManager["eagle"], NULL, &rect);
    }
};

class PlayerTank{
public:
    int x, y;
    string type_input;
    bool tank_move;
    int right = 0;
    int left = 0;
    int up = 0;
    int down = 0;
    SDL_Rect frame[2] = {{0, 0, 4*TILE_SIZE, 4*TILE_SIZE},
                                     {48, 0, 4*TILE_SIZE, 4*TILE_SIZE}};
    int dirX, dirY;                             //direction
    SDL_Rect rect;
    vector<Bullet> bullets;

    PlayerTank() {
        tank_move = false;
        type_input = "up";
        dirX = 0;
        dirY = -1;
    };
    PlayerTank(int startX, int startY) {
        tank_move = false;
        type_input = "up";
        x = startX;
        y = startY;
        rect = {x, y, 2*TILE_SIZE, 2*TILE_SIZE};
        dirX = 0;
        dirY = -1;                                              // mac dinh tank di len (truc y huong xuong)
    }

    void move(bool tank_move, string type_input,int dx, int dy, const vector<Wall>& walls, const vector<Wall> rocks, const vector<EnemyTank>& enemies, Eagle eagle);
    void shoot();
    void updateBullets();
    void render(SDL_Renderer* renderer);
};

class Game{
public:
    ImpTimer fps_timer;
    int healthPlayer = 3;
    bool running;
    vector<Wall> bricks;
    vector<Wall> rocks;
    PlayerTank player;
    Eagle eagle;
    int enemyNumber = 3;
    vector<EnemyTank> enemies;

    Game();
    void LoadMap(const string& path);
    void spawnEnemies();                                          //sinh xe tang dich
    void render(SDL_Renderer* renderer);
    void handleEvents();
    void update();        //kiem soat logic game
    void run();
    ~Game();
};

Game::Game() {
    running = true;

    TextureManager["brick"] = LoadTexture("map/brick.png", renderer);
    TextureManager["rock"] = LoadTexture("map/rock.png", renderer);
    TextureManager["playertank"] = LoadTexture("image/playertank.png", renderer);
    TextureManager["enemytank"] = LoadTexture("map/enemytank.png", renderer);
    TextureManager["eagle"] = LoadTexture("map/eagle.jpg", renderer);
    TextureManager["playertankup"] = LoadTexture("image/playertankup.png", renderer);
    TextureManager["playertankdown"] = LoadTexture("image/playertankdown.png", renderer);
    TextureManager["playertankright"] = LoadTexture("image/playertankright.png", renderer);
    TextureManager["playertankleft"] = LoadTexture("image/playertankleft.png", renderer);
    TextureManager["enemytnakfalse"] =  LoadTexture("image/enemytnakfalse.png", renderer);
    const string mapPath = "map/stage1.txt";
    LoadMap(mapPath);
    spawnEnemies();
}

void Game:: LoadMap(const string& path) {
    ifstream file(path);
    if(!file.is_open()) {
        cout << "Can't load file" << path << endl;
    }
    int MAP[60][58];
    for(int i = 0; i< MAP_HEIGHT; i++) {
        for(int j = 0; j< MAP_WIDTH; j++) {
            file >> MAP[i][j];
            cout << MAP[i][j] << " ";
            switch (MAP[i][j]) {
                case 1:
                    bricks.emplace_back(j*TILE_SIZE, i*TILE_SIZE, "brick");
                    break;
                case 4:
                    rocks.emplace_back(j*TILE_SIZE, i*TILE_SIZE, "rock");
                    break;
                case 8:
                    if(MAP[i-3][j-3] == 8) {
                        player.x = (j-3)*TILE_SIZE;
                        player.y = (i-3)*TILE_SIZE;
                        player.rect = {(j-3)*TILE_SIZE, (i-3)*TILE_SIZE, 4*TILE_SIZE, 4*TILE_SIZE};
                    }
                    break;
                case 5:
                    if(MAP[i-3][j-3] == 5) {
                        eagle.x = (j-3)*TILE_SIZE;
                        eagle.y = (i-3)*TILE_SIZE;
                        eagle.rect = {(j-3)*TILE_SIZE, (i-3)*TILE_SIZE, 4*TILE_SIZE, 4*TILE_SIZE};
                    }
                default:
                    break;
            }
        }
        cout << endl
;
    }
}

void Game::render(SDL_Renderer* renderer) {
    // ve nen
    //fps_timer.start();
    cout << "render run" << endl;
    SDL_SetRenderDrawColor(renderer, 128 , 128, 128, 255);      // nen xam
    SDL_RenderClear(renderer);                                                   // xoa nen va ve mau xam da chon len

    SDL_SetRenderDrawColor(renderer, 0, 0, 0 , 255);                // nen den
    for(int i = 2; i < MAP_HEIGHT-2; i++) {
        for(int j = 4; j< MAP_WIDTH-4; j++) {
            SDL_Rect tile = { j*TILE_SIZE, i*TILE_SIZE, TILE_SIZE, TILE_SIZE };         //
            SDL_RenderFillRect(renderer, &tile);                             // to mau cac hinh chu nhat
        }
    }
    // ve wall
    for(int i = 0; i<(int)bricks.size(); i++) {
        bricks[i].render(renderer);
    }
    for(int i = 0; i<(int)rocks.size(); i++) {
        rocks[i].render(renderer);
    }
    player.render(renderer);
    eagle.render(renderer);

    for(auto& enemy : enemies) {
        enemy.render(renderer);
    }

    SDL_RenderPresent(renderer);                                               // hien thi len man hinh
}

void Game::handleEvents() {
    SDL_Event event;
    while(SDL_PollEvent(&event)) {
        if(event.type == SDL_QUIT) {
            running =false;
        }   else if(event.type == SDL_KEYDOWN) {
            switch(event.key.keysym.sym) {
                case SDLK_UP: player.move(true, "up", 0, -8, bricks, rocks, enemies, eagle); break;
                case SDLK_DOWN: player.move(true, "down",0, 8, bricks, rocks, enemies, eagle); break;
                case SDLK_LEFT: player.move(true, "left" , -8, 0, bricks, rocks, enemies, eagle); break;
                case SDLK_RIGHT: player.move(true, "right", 8, 0, bricks, rocks, enemies, eagle); break;
                case SDLK_SPACE: player.shoot() ; break;
            }
        } else if(event.type == SDL_KEYUP) {
                switch(event.key.keysym.sym) {
                    case SDLK_UP: player.move(false, "up", 0, 0, bricks, rocks, enemies, eagle); break;
                    case SDLK_DOWN: player.move(false, "down",0, 0, bricks, rocks, enemies, eagle); break;
                    case SDLK_LEFT: player.move(false, "left" , 0, 0, bricks, rocks, enemies, eagle); break;
                    case SDLK_RIGHT: player.move(false, "right", 0, 0, bricks, rocks, enemies, eagle); break;
                    case SDLK_SPACE: player.shoot() ; break;
                    }
            }
    }
}
void Game::run() {
    while(running) {
        //render(renderer);
        fps_timer.start();
        handleEvents();
        update();
        render(renderer);

        int real_imp_time = fps_timer.get_ticks();
        int time_one_frame = 1000/FRAME_PER_SECOND; //ms

        cout << real_imp_time << " " << time_one_frame  << endl;

        if(real_imp_time < time_one_frame) {
            int delay_time = time_one_frame - real_imp_time;
            cout << delay_time << endl;
            if(delay_time >= 0) SDL_Delay(delay_time);
        }
        //SDL_Delay(16);
    }
}

Wall::Wall(int X, int Y, string type_)
{
    type = type_;
    x = X;
    y = Y;
    active = true;
    rect = {x, y, TILE_SIZE, TILE_SIZE};
}

void Wall::render( SDL_Renderer* renderer){
    if(active) {
        if(type == "brick") SDL_RenderCopy(renderer, TextureManager["brick"], NULL, &rect);
        if(type == "rock") SDL_RenderCopy(renderer, TextureManager["rock"], NULL, &rect);
    }
}

void Game::spawnEnemies(){
    enemies.clear();
    for(int i = 0; i<enemyNumber; ++i) {
        int ex, ey;
        bool validPosition = false;
        while(!validPosition) {
            int randEne = rand() % 3;
            switch(randEne) {
                case 0:
                    ex =4 * TILE_SIZE;
                    ey = 2*TILE_SIZE;
                    break;
                case 1:
                    ex =28 * TILE_SIZE;
                    ey = 2*TILE_SIZE;
                    break;
                case 2:
                    ex =52 * TILE_SIZE;
                    ey = 2*TILE_SIZE;
                    break;
            }
            validPosition = true;
            for(const auto& brick : bricks) {
                if(brick.active && brick.x == ex && brick.y == ey) {
                    validPosition = false;
                    break;
                }
            }

            for(const auto& enemy : enemies) {
                if(enemy.active && enemy.x == ex && enemy.y == ey) {
                    validPosition = false;
                    break;
                }
            }
        }
        enemies.push_back(EnemyTank(ex, ey));

    }
}

void Bullet::move()
{
    x += dx;
    y += dy;
    rect.x = x;
    rect.y = y;
    if(x < TILE_SIZE*4 || x>= SCREEN_WIDTH - TILE_SIZE*4 ||
       y < TILE_SIZE*2 || y > SCREEN_HEIGHT - TILE_SIZE*2)  {
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
    bullets.push_back(Bullet(x + TILE_SIZE*2-5, y+ TILE_SIZE*2-5,this->dirX, this->dirY));
}

void PlayerTank::updateBullets() {
    for(auto& bullet :  bullets) {
        bullet.move();
    }
    bullets.erase(remove_if(bullets.begin(), bullets.end(),                 // di chuyen cac vien dan xuong cuoi mang roi xoa
                            [](Bullet& b) { return !b.active; }) , bullets.end());
}

void PlayerTank::render(SDL_Renderer* renderer) {
//    SDL_SetRenderDrawColor(renderer, 255, 255, 0, 255);
//    SDL_RenderFillRect(renderer, &rect);
    if(tank_move) {
        if(type_input == "up") {
            SDL_RenderCopy(renderer, TextureManager["playertankup"], &frame[up], &rect);
            up++;
            if(up > 1) up = 0;
        }

        if(type_input == "down") {
            SDL_RenderCopy(renderer, TextureManager["playertankdown"], &frame[down], &rect);
            down++;
            if(down > 1) down = 0;
        }

        if(type_input == "right") {
            SDL_RenderCopy(renderer, TextureManager["playertankright"], &frame[right], &rect);
            right++;
            if(right > 1) right = 0;
        }

        if(type_input == "left") {
            SDL_RenderCopy(renderer, TextureManager["playertankleft"], &frame[left], &rect);
            left++;
            if(left > 1) left = 0;
        }
    } else {
        if(type_input == "up") {
            SDL_RenderCopy(renderer, TextureManager["playertankup"], &frame[0], &rect);
        }

        if(type_input == "down") {
            SDL_RenderCopy(renderer, TextureManager["playertankdown"], &frame[0], &rect);
        }

        if(type_input == "right") {
            SDL_RenderCopy(renderer, TextureManager["playertankright"], &frame[0], &rect);
        }

        if(type_input == "left") {
            SDL_RenderCopy(renderer, TextureManager["playertankleft"], &frame[0], &rect);
        }
    }
    for(auto &bullet: bullets) {
        bullet.render(renderer);
    }
}

void PlayerTank::move(bool tank_move, string type_input, int dx, int dy, const vector<Wall>& bricks, const vector<Wall> rocks, const vector<EnemyTank>& enemies, Eagle eagle) {
    this->type_input = type_input;
    this->tank_move = tank_move;
    if(!tank_move) return;
    int newX = x+dx;
    int newY = y+dy;
    this->dirX = dx;
    this->dirY = dy;

    SDL_Rect newRect = {newX, newY, 4*TILE_SIZE, 4*TILE_SIZE};
    for(int i = 0; i<bricks.size(); i++) {
        if(bricks[i].active && SDL_HasIntersection(&newRect, &bricks[i].rect))            //kiem tra su va cham voi tuong
            return;
    }

    for(int i = 0; i<rocks.size(); i++) {
        if(rocks[i].active && SDL_HasIntersection(&newRect, &rocks[i].rect))            //kiem tra su va cham voi tuong
            return;
    }

    for(int i = 0; i<enemies.size(); i++) {
        if(rocks[i].active && SDL_HasIntersection(&newRect, &enemies[i].rect))            //kiem tra su va cham voi tuong
            return;
    }

    if(SDL_HasIntersection(&newRect, &eagle.rect)) return;

    if(newX >= TILE_SIZE*4 && newX <= SCREEN_WIDTH - TILE_SIZE*8 &&
       newY >= TILE_SIZE*2 && newY <= SCREEN_HEIGHT - TILE_SIZE*6)  {
        x = newX;
        y = newY;
        rect.x = x;
        rect.y = y;
       }

}

void Game::update() {
    player.updateBullets();

    for(auto& enemy:enemies) {
        enemy.move(bricks, rocks, player.x, player.y);
        enemy.updateBullets();
        if(rand() % 100 < 10) {
            enemy.shoot();
        }
    }

    for(auto& enemy: enemies) {
        for(auto& bullet : enemy.bullets) {
            for(auto& brick : bricks) {
                if(brick.active && SDL_HasIntersection(&bullet.rect, &brick.rect)) {
                    brick.active = false;
                    bullet.active = false;
                    break;
                }
            }
        }
    }

    for(auto& enemy: enemies) {
        for(auto& bullet : enemy.bullets) {
            for(auto& bulletp : player.bullets) {
                if(bulletp.active && bullet.active && SDL_HasIntersection(&bullet.rect, &bulletp.rect)) {
                    bulletp.active = false;
                    bullet.active = false;
                    break;
                }
            }
        }
    }

     for(auto& enemy: enemies) {
        for(auto& bullet : enemy.bullets) {
            for(auto& rock : rocks) {
                if(rock.active && SDL_HasIntersection(&bullet.rect, &rock.rect)) {
                    bullet.active = false;
                    break;
                }
            }
        }
    }

    for(auto& bullet : player.bullets) {
        for(auto& brick : bricks) {
            if(brick.active && SDL_HasIntersection(&bullet.rect, & brick.rect)) {
                brick.active = false;
                bullet.active = false;
                break;
            }
        }
    }

    for(auto& bullet : player.bullets) {
        for(auto& rock : rocks) {
            if(rock.active && SDL_HasIntersection(&bullet.rect, & rock.rect)) {
                bullet.active = false;
                break;
            }
        }
    }

    for(auto& bullet : player.bullets) {
        for(auto& enemy : enemies) {
            if(enemy.active && SDL_HasIntersection(&bullet.rect, &enemy.rect)) {
                enemy.active = false;
                enemy.false_active(renderer);
                bullet.active = false;
            }
        }
    }

    for(auto& enemy : enemies) {
        for(auto& bullet : enemy.bullets) {
            if(SDL_HasIntersection(&bullet.rect, &eagle.rect)) {
                running = false;
                return;
            }
        }
    }

    for(auto& bullet : player.bullets) {
            if(SDL_HasIntersection(&bullet.rect, & eagle.rect)) {
                running = false;
                break;
            }
    }

    for(auto& enemy : enemies) {
        for(auto& bullet : enemy.bullets) {
            if(SDL_HasIntersection(&bullet.rect, &player.rect)) {
                bullet.active = false;
                healthPlayer--;
                if(healthPlayer == 0) running = false;
                player.x = 20*TILE_SIZE;
                player.y = 50*TILE_SIZE;
                player.rect.x = player.x;
                player.rect.y = player.y;
                player.type_input = "up";
                return;
            }
        }
    }

    enemies.erase(remove_if(enemies.begin(), enemies.end(),
                            [] (EnemyTank &e) {return !e.active;}), enemies.end());
    if(enemies.empty() ) {
        running = false;
    }
}

Game::~Game() {
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
}

int main(int argc, char* argv[])
{
    srand(time(0));

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

    Menu menu(renderer);
    Menu::MenuResult choose = menu.chooseMenu();
    if(choose == 0) {
        Game game;
        if(game.running) {
            game.run();
        }
    }

    return 0;
}
