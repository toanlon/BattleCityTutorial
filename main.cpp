#include "CommonFuntion.h"
#include "Menu.h"
#include "ImpTimer.h"
map<string, SDL_Texture*> TextureManager;


void displayGameOver(SDL_Renderer* renderer, int score, Mix_Chunk* gameOverSound) {
    // Khởi tạo SDL_ttf nếu chưa khởi tạo
    if (TTF_WasInit() == 0) {
        if (TTF_Init() == -1) {
            cerr << "Failed to initialize TTF: " << TTF_GetError() << endl;
            return;
        }
    }

    // Tạo chuỗi điểm số
    stringstream ss;
    ss << "Score: " << score;
    string scoreText = ss.str();

    SDL_Color red = {255, 0, 0, 255};

    // Phát âm thanh game over (một lần)
    Mix_PlayChannel(-1, gameOverSound, 0);

    SDL_Rect gameoverBGR = {0, 0, SCREEN_WIDTH, SCREEN_HEIGHT};
    SDL_Rect stringGame_over = {SCREEN_WIDTH/2-100, 200, 200, 200};
    string final_scoreText = "Score: " + to_string(score);
    SDL_Color textColorScore = {255, 255, 255}; // màu trắng
    SDL_Rect renderScore = { SCREEN_WIDTH/2-50, 480, 100, 50 };

    // Vòng lặp chờ người dùng bấm phím (hoặc sự kiện QUIT) để thoát màn hình Game Over
    bool waiting = true;
    SDL_Event event;
    while (waiting) {
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT || event.type == SDL_MOUSEBUTTONDOWN) {
                waiting = false;
            }
        }
        SDL_RenderClear(renderer);
        SDL_RenderCopy(renderer, TextureManager["gameoverBRG"], NULL, &gameoverBGR);
        SDL_RenderCopy(renderer, TextureManager["gameoverSTR"], NULL, &stringGame_over);
        SDL_RenderCopy(renderer, LoadTTF(scoreFont, final_scoreText.c_str(), textColorScore) , NULL, &renderScore);
        SDL_RenderPresent(renderer);
        SDL_Delay(10); // Delay nhẹ để tránh vòng lặp bận
    }

}

void displayYouWin(SDL_Renderer* renderer, Mix_Chunk* gameOverSound) {

    Mix_PlayChannel(-1, gameOverSound, 0);

    SDL_Rect youWinRect = {0, 0, SCREEN_WIDTH, SCREEN_HEIGHT};
    bool waiting = true;
    SDL_Event event;
    while (waiting) {
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT || event.type == SDL_MOUSEBUTTONDOWN) {
                waiting = false;
            }
        }
        SDL_RenderClear(renderer);
        SDL_RenderCopy(renderer, TextureManager["win"], NULL, &youWinRect);
        SDL_RenderPresent(renderer);
        SDL_Delay(10); // Delay nhẹ để tránh vòng lặp bận
    }

}


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

    EnemyTank() {
        type_move = "down";
        tank_move = false;
    }

    EnemyTank(int startX, int startY) {
        type_move = "down";
        tank_move = false;
        moveDelay = 8;
        shootDelay = 5;
        x = startX;
        y = startY;
        rect = {x, y+8, 2*TILE_SIZE-8, 2*TILE_SIZE-8};
        dirX = 0;
        dirY = 12;
        active = true;
    }

    void false_active(SDL_Renderer* renderer) {
        for(int i = 0; i<2; i++) {
            SDL_RenderCopy(renderer, TextureManager["enemytankfalse"], &frame[i], &rect);
            SDL_RenderPresent(renderer);
            SDL_Delay(60);
        }
    }
    void move(const vector<Wall>& bricks, const vector<Wall>& rocks, int playerX, int playerY) {
        if(--moveDelay > 0) return;
        moveDelay = 12;

        if(rand() % 100 < 36) {
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

    void shoot() {
        if(-- shootDelay > 0) return;
        shootDelay = 5;
        bullets.push_back(Bullet(x+TILE_SIZE -10, y+TILE_SIZE - 5, this->dirX/2, this->dirY/2));
    }

    void updateBullets() {
        for(auto& bullet : bullets) {
            bullet.move();
        }
        bullets.erase(remove_if(bullets.begin(), bullets.end(),
                                                [](Bullet& b) { return !b.active; } ), bullets.end());
    }

    void render(SDL_Renderer* renderer){

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
};

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

    PlayerTank() {
        isInvincible = true;
        tank_move = false;
        type_input = "up";
        dirX = 0;
        dirY = -12;
    };
    PlayerTank(int startX, int startY) {
        isInvincible = true;
        tank_move = false;
        type_input = "up";
        x = startX;
        y = startY;
        rect = {x+8, y+8, 2*TILE_SIZE-8, 2*TILE_SIZE-8};
        dirX = 0;
        dirY = -1;                                              // mac dinh tank di len (truc y huong xuong)
    }

    void setInvincible(Uint32 currentTime) {
        isInvincible = true;
        isInvincibleStart = currentTime;
    }
    void move(bool tank_move, string type_input,int dx, int dy, const vector<Wall>& walls, const vector<Wall> rocks, const vector<EnemyTank>& enemies);
    void shoot();
    void updateBullets();
    void render(SDL_Renderer* renderer);
};

class Game{
public:
    Mix_Chunk* bulletHitSound;

    ImpTimer fps_timer;
    // Biến toàn cục hoặc thành viên của lớp Game để lưu thời gian spawn cuối cùng
    Uint32 lastSpawnTime = 0;
    // Đặt khoảng delay mong muốn (ví dụ: 5000ms = 5 giây)
    const Uint32 spawnDelay = 5000;

    // --- THÊM BIẾN PAUSE:
    Menu* pauseMenu = new Menu(renderer);
    bool isPaused = false;       // trạng thái game pause
    ImpTimer pauseTimer;         // để đo thời gian bị pause

    int healthPlayer = 3;
    int score = 0;
    bool running;
    bool WIN = false;
    bool isReturnToMenu = false;
    vector<Wall> bricks;
    vector<Wall> rocks;
    PlayerTank player;
    int enemyNumber = 10;
    int enemyKill = 0;
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

    // - bulletHitSound: Hiệu ứng âm thanh khi đạn trúng, được tải từ file WAV.
    bulletHitSound = Mix_LoadWAV("sound/bullethit.wav");
    if(!bulletHitSound) {
        cerr << "Failed to load bullethit.wav! SDL_mixer Error: " << Mix_GetError() << endl;
    }

    const string mapPath = "map/test.txt";
    LoadMap(mapPath);

//    Uint32 currentTime = SDL_GetTicks();
//    if(currentTime - lastSpawnTime >= spawnDelay) {
    spawnEnemies();      // Hàm spawn enemy của bạn

    // Thiết lập lastSpawnTime ban đầu
    lastSpawnTime = SDL_GetTicks();
//        lastSpawnTime = currentTime;
//    }
}

void Game:: LoadMap(const string& path) {
    ifstream file(path);
    if(!file.is_open()) {
        cout << "Can't load file" << path << endl;
    }
    int MAP[60][60];
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
                    if(MAP[i-1][j-1] == 8) {
                        player.x = (j-1)*TILE_SIZE;
                        player.y = (i-1)*TILE_SIZE;
                        player.rect = {(j-1)*TILE_SIZE, (i-1)*TILE_SIZE, 2*TILE_SIZE, 2*TILE_SIZE};
                    }
                    break;
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
    SDL_SetRenderDrawColor(renderer, 128 , 128, 128, 255);      // nen xam
    SDL_RenderClear(renderer);                                                   // xoa nen va ve mau xam da chon len

    SDL_SetRenderDrawColor(renderer, 0, 0, 0 , 255);                // nen den
    for(int i = 1; i < MAP_HEIGHT-1; i++) {
        for(int j = 2; j< MAP_WIDTH-6; j++) {
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

    for(auto& enemy : enemies) {
        enemy.render(renderer);
    }

    SDL_Rect buttonPauseR = {700, 50, 50, 50};
    SDL_RenderCopy(renderer, TextureManager["pauseButton"], NULL, &buttonPauseR);

    string scoreText = "Score: " + to_string(score);
    SDL_Color textColorScore = {255, 255, 255}; // màu trắng
    SDL_Rect renderScore = { SCREEN_WIDTH - 125, 100, 100, 50 };
    SDL_RenderCopy(renderer, LoadTTF(scoreFont, scoreText.c_str(), textColorScore) , NULL, &renderScore);

    string healthPlayerText = "Health: " + to_string(healthPlayer);
    SDL_Color textColorHealth = {255, 0, 0, 255}; // màu trắng
    SDL_Rect renderHealth = { SCREEN_WIDTH - 125, 200, 100, 50 };
    SDL_RenderCopy(renderer, LoadTTF(scoreFont, healthPlayerText.c_str(), textColorHealth) , NULL, &renderHealth);

    SDL_RenderPresent(renderer);                                               // hien thi len man hinh
}

void Game::handleEvents() {
    SDL_Event event;
    while(SDL_PollEvent(&event)) {
        if(event.type == SDL_QUIT) {
            running =false;
        } else if(event.type == SDL_MOUSEBUTTONDOWN) {
            // Giả sử nút pause nằm ở góc trên bên phải, ví dụ: (SCREEN_WIDTH-100, 0) với kích thước 100x50
            int mx = event.button.x;
            int my = event.button.y;
            SDL_Rect buttonPauseR = {700, 50, 50, 50};
            if(mx >= buttonPauseR.x && mx <= buttonPauseR.x + buttonPauseR.w &&
               my >= buttonPauseR.y && my <= buttonPauseR.y + buttonPauseR.h) {
                // Toggle trạng thái pause
                if(!isPaused) {
                    isPaused = true;
                    pauseTimer.start();
                } else {
                    // Khi resume, điều chỉnh lastSpawnTime để bù thời gian dừng
                    Uint32 pauseDuration = pauseTimer.get_ticks();
                    lastSpawnTime += pauseDuration;
                    player.isInvincibleStart += pauseDuration;
                    isPaused = false;
                }
            }
        } else if(event.type == SDL_KEYDOWN) {
            if(!isPaused) { // Chỉ xử lý input khi không pause
                switch(event.key.keysym.sym) {
                    case SDLK_UP: player.move(true, "up", 0, -8, bricks, rocks, enemies); break;
                    case SDLK_DOWN: player.move(true, "down",0, 8, bricks, rocks, enemies); break;
                    case SDLK_LEFT: player.move(true, "left" , -8, 0, bricks, rocks, enemies); break;
                    case SDLK_RIGHT: player.move(true, "right", 8, 0, bricks, rocks, enemies); break;
                    case SDLK_SPACE: player.shoot() ; break;
                }
            }

        } else if(event.type == SDL_KEYUP) {
                switch(event.key.keysym.sym) {
                    case SDLK_UP: player.move(false, "up", 0, 0, bricks, rocks, enemies); break;
                    case SDLK_DOWN: player.move(false, "down",0, 0, bricks, rocks, enemies); break;
                    case SDLK_LEFT: player.move(false, "left" , 0, 0, bricks, rocks, enemies); break;
                    case SDLK_RIGHT: player.move(false, "right", 0, 0, bricks, rocks, enemies); break;
                    case SDLK_SPACE: player.shoot() ; break;
                    }
            }
    }
}
void Game::run() {
    while(running) {
       // cout <<
        //render(renderer);
        fps_timer.start();
        handleEvents();

        if(isPaused) {
            pauseMenu->renderPauseMenu(renderer);  // hàm tự bạn viết để render pause menu

            // Chờ người dùng bấm vào các nút trong pause menu
            // Bạn có thể dùng một vòng lặp nhỏ riêng để xử lý sự kiện pause menu
            bool pauseMenuActive = true;
            SDL_Event event;
            while(pauseMenuActive) {
                while(SDL_PollEvent(&event)) {
                    if(event.type == SDL_QUIT) {
                        running = false;
                        pauseMenuActive = false;
                    }
                    else if(event.type == SDL_MOUSEBUTTONDOWN) {
                        int mx = event.button.x;
                        int my = event.button.y;
                        SDL_Rect resumeRect = {SCREEN_WIDTH/2 - 100, SCREEN_HEIGHT/2 - 60, 200, 50};
                        SDL_Rect menuRect = {SCREEN_WIDTH/2 - 100, SCREEN_HEIGHT/2 + 10, 200, 50};
                        if(mx >= resumeRect.x && mx <= resumeRect.x + resumeRect.w &&
                           my >= resumeRect.y && my <= resumeRect.y + resumeRect.h) {
                            // Resume game: điều chỉnh lastSpawnTime và thoát vòng lặp pause menu
                            Uint32 pauseDuration = pauseTimer.get_ticks();
                            lastSpawnTime += pauseDuration;
                            player.isInvincibleStart += pauseDuration;
                            pauseMenuActive = false;
                            isPaused = false;
                        }
                        else if(mx >= menuRect.x && mx <= menuRect.x + menuRect.w &&
                                my >= menuRect.y && my <= menuRect.y + menuRect.h) {
                            // Quay về menu chính: có thể đặt running = false hoặc chuyển trạng thái menu
                            isReturnToMenu = true;
                            pauseMenuActive = false;
                            running = false;
                        }
                    }
                }
                SDL_Delay(10); // Delay nhẹ để tránh vòng lặp
        }
            if(isReturnToMenu) break;

            continue;  // Bỏ qua update và render game chính
        }

        update();
        render(renderer);

        int real_imp_time = fps_timer.get_ticks();
        int time_one_frame = 1000/FRAME_PER_SECOND; //ms

        if(real_imp_time < time_one_frame) {
            int delay_time = time_one_frame - real_imp_time;
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
    //enemies.clear();
//    for(int i = 0; i<enemyNumber; ++i) {
        enemyNumber--;
        if(enemyNumber == 0) return;
        int ex, ey;
        bool validPosition = false;
        while(!validPosition) {
            int randEne = rand() % 3;
            switch(randEne) {
                case 0:
                    ex =2 * TILE_SIZE;
                    ey = 1*TILE_SIZE;
                    break;
                case 1:
                    ex =  14 * TILE_SIZE;
                    ey = 1*TILE_SIZE;
                    break;
                case 2:
                    ex =26 * TILE_SIZE;
                    ey = 2*TILE_SIZE;
                    break;
            }
            SDL_Rect rand_enemy = {ex, ey, 2*TILE_SIZE, 2*TILE_SIZE};

            validPosition = true;
            for(const auto& brick : bricks) {
                if(brick.active && SDL_HasIntersection(&rand_enemy, &brick.rect)) {
                    validPosition = false;
                    break;
                }
            }

            if(!validPosition) continue;
            for(const auto& rock : rocks) {
                if(rock.active && SDL_HasIntersection(&rand_enemy, &rock.rect)) {
                    validPosition = false;
                    break;
                }
            }

            if(!validPosition) continue;
            if(SDL_HasIntersection(&rand_enemy, &player.rect)) {
                validPosition = false;
                continue;
            }

            for(const auto& enemy : enemies) {
                if(enemy.active &&  SDL_HasIntersection(&rand_enemy, &enemy.rect)) {
                    validPosition = false;
                    break;
                }
            }
        }
        enemies.push_back(EnemyTank(ex+3, ey));

    //}
}

void Bullet::move()
{
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

void PlayerTank::shoot() {
    bullets.push_back(Bullet(x + TILE_SIZE-5, y+ TILE_SIZE-5,this->dirX, this->dirY));
}

void PlayerTank::updateBullets() {
    for(auto& bullet :  bullets) {
        bullet.move();
    }
    bullets.erase(remove_if(bullets.begin(), bullets.end(),                 // di chuyen cac vien dan xuong cuoi mang roi xoa
                            [](Bullet& b) { return !b.active; }) , bullets.end());
}

void PlayerTank::render(SDL_Renderer* renderer) {
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

    if(isInvincible) {
        SDL_Rect helmetClips[2];
        for(int i = 0; i<2; i++) {
            helmetClips[i].x = i * 16;
            helmetClips[i].y = 0;
            helmetClips[i].w = 16;
            helmetClips[i].h = 16;
            SDL_RenderCopy(renderer, TextureManager["helmet"], &helmetClips[invincible_Delay/10], &rect);
            invincible_Delay++;
            if(invincible_Delay/10 >= 2) invincible_Delay  = 0;
        }

    }

    for(auto &bullet: bullets) {
        bullet.render(renderer);
    }
}

 void PlayerTank::move(bool tank_move, string type_input, int dx, int dy, const vector<Wall>& bricks, const vector<Wall> rocks, const vector<EnemyTank>& enemies) {
    this->type_input = type_input;
    this->tank_move = tank_move;
    if(!tank_move) return;
    int newX = x+dx;
    int newY = y+dy;
    this->dirX = dx;
    this->dirY = dy;

    SDL_Rect newRect = {newX, newY, 2*TILE_SIZE, 2*TILE_SIZE};
    for(int i = 0; i<bricks.size(); i++) {
        if(bricks[i].active && SDL_HasIntersection(&newRect, &bricks[i].rect))            //kiem tra su va cham voi tuong
            return;
    }

    for(int i = 0; i<rocks.size(); i++) {
        if(rocks[i].active && SDL_HasIntersection(&newRect, &rocks[i].rect))            //kiem tra su va cham voi tuong
            return;
    }

    for(int i = 0; i<enemies.size(); i++) {
        if(enemies[i].active && SDL_HasIntersection(&newRect, &enemies[i].rect))            //kiem tra su va cham voi tuong
            return;
    }

    if(newX >= TILE_SIZE*2 && newX <= SCREEN_WIDTH - TILE_SIZE*8 &&
       newY >= TILE_SIZE*1 && newY <= SCREEN_HEIGHT - TILE_SIZE*3)  {
        x = newX;
        y = newY;
        rect.x = x;
        rect.y = y;
       }

}

void Game::update() {

   Uint32 currentTime = SDL_GetTicks();
   if(currentTime - lastSpawnTime >= spawnDelay) {
        spawnEnemies();      // Hàm spawn enemy của bạn
        lastSpawnTime = currentTime;
    }

    player.updateBullets();

    if(player.isInvincible && currentTime - player.isInvincibleStart >= player.invincible_Time) {
        player.isInvincible = false;
    }

    for(auto& enemy:enemies) {
        enemy.move(bricks, rocks, player.x, player.y);
        enemy.updateBullets();
          if(rand() % 100 < 15) {
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
                enemyKill++;
                score++;
                cout << "kill_enemy" <<endl;
                Mix_PlayChannel(-1, bulletHitSound, 0);
                enemy.false_active(renderer);
                bullet.active = false;
            }
        }
    }

    for(auto& enemy : enemies) {
        for(auto& bullet : enemy.bullets) {
            if(SDL_HasIntersection(&bullet.rect, &player.rect)) {
                bullet.active = false;
                if(player.isInvincible) continue;
                else{
                healthPlayer--;
                player.setInvincible(SDL_GetTicks());
                if(healthPlayer == 0) {
//                    SDL_Rect gameoverBGR = {0, 0, SCREEN_WIDTH, SCREEN_HEIGHT};
//                    SDL_RenderCopy(renderer, TextureManager["gameoverBRG"], NULL, &gameoverBGR);
//                    SDL_Rect stringGame_over = {200, 200, 200, 200};
//                    SDL_RenderCopy(renderer, TextureManager["gameoverSTR"], NULL, &gameoverBGR);

                    running = false;
                }
                player.x = 10*TILE_SIZE;
                player.y = 24*TILE_SIZE;
                player.rect.x = player.x;
                player.rect.y = player.y;
                player.type_input = "up";
                return;
                }
            }
        }
    }

    enemies.erase(remove_if(enemies.begin(), enemies.end(),
                            [] (EnemyTank &e) {return !e.active;}), enemies.end());
    if(enemyKill == 11) {
        WIN = true;
        running = false;
    }
}

Game::~Game() {

}

int main(int argc, char* argv[])
{
    srand(time(0));

    if(SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) < 0) {
        cerr << "SDL could not initialize! SDL_Eror: " << SDL_GetError() << endl;
        running = false;
    }

    // Khởi tạo SDL_ttf
    if (TTF_Init() == -1) {
        cerr << "SDL_ttf could not initialize! TTF_Error: " << TTF_GetError() << endl;
        SDL_Quit();
        return 1;
    }

    scoreFont = TTF_OpenFont("font/score.ttf", 24);
    if(scoreFont == NULL) {
        cerr << "Failed to load font! TTF_Error: " << TTF_GetError() << endl;
    }

    // Khởi tạo SDL_mixer với tần số 44100 Hz, định dạng mặc định, 2 kênh (stereo) và kích thước buffer 2048 byte
    if(Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0) {
        cerr << "SDL_mixer could not initialize! SDL_mixwe Error: " << Mix_GetError() << endl;
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

    Mix_Music* introMenuMusic = Mix_LoadMUS("sound/introMenu.mp3");
    if(!introMenuMusic) {
        cerr << "Failed to load introMenu.mp3! SDL_mixer Error: " << Mix_GetError() << endl;
    }
    // - introGameMusic: Nhạc nền intro game, được tải từ file WAV.
    Mix_Music* introGameMusic = Mix_LoadMUS("sound/introGame.wav");
    if(!introGameMusic) {
        cerr << "Failed to load introGame.wav! SDL_mixer Error: " << Mix_GetError() << endl;
    }
//    // - bulletHitSound: Hiệu ứng âm thanh khi đạn trúng, được tải từ file WAV.
//    Mix_Chunk* bulletHitSound = Mix_LoadWAV("sound/bullethit.wav");
//    if(!bulletHitSound) {
//        cerr << "Failed to load bullethit.wav! SDL_mixer Error: " << Mix_GetError() << endl;
//    }
    // - gameOverSound: Hiệu ứng game over, được tải từ file WAV.
    Mix_Chunk* gameOverSound = Mix_LoadWAV("sound/gameover.wav");
    if(!gameOverSound) {
        cerr << "Failed to load gameover.wav! SDL_mixer Error: " << Mix_GetError() << endl;
    }

    Mix_Chunk* youWinSound = Mix_LoadWAV("sound/win.mp3");
    if(!youWinSound) {
        cerr << "Failed to load sound/win.mp3! SDL_mixer Error: " << Mix_GetError() << endl;
    }

    TextureManager["pauseButton"] = LoadTexture("image/pauseButton.png", renderer);
    TextureManager["brick"] = LoadTexture("map/brick.png", renderer);    TextureManager["rock"] = LoadTexture("map/rock.png", renderer);
    TextureManager["playertank"] = LoadTexture("image/playertank.png", renderer);
    TextureManager["enemytank"] = LoadTexture("map/enemytank.png", renderer);
    TextureManager["playertankup"] = LoadTexture("image/playertankup.png", renderer);
    TextureManager["playertankdown"] = LoadTexture("image/playertankdown.png", renderer);
    TextureManager["playertankright"] = LoadTexture("image/playertankright.png", renderer);
    TextureManager["playertankleft"] = LoadTexture("image/playertankleft.png", renderer);
    TextureManager["helmet"] = LoadTexture("image/helmet.png", renderer);
    TextureManager["enemytankfalse"] =  LoadTexture("image/enemytankfalse.png", renderer);
    TextureManager["enemytankleft"] =  LoadTexture("image/enemytankleft.png", renderer);
    TextureManager["enemytankright"] =  LoadTexture("image/enemytankright.png", renderer);
    TextureManager["enemytankup"] =  LoadTexture("image/enemytankup.png", renderer);
    TextureManager["enemytankdown"] =  LoadTexture("image/enemytankdown.png", renderer);
    TextureManager["gameoverBRG"] = LoadTexture("image/menubackgr.jpg", renderer);
    TextureManager["gameoverSTR"] = LoadTexture("image/gameover.png", renderer);
    TextureManager["win"] =  LoadTexture("image/win.jpg", renderer);
    // Phát nhạc nền intro menu, với tham số -1 để lặp vô hạn
    Mix_PlayMusic(introMenuMusic, -1);

    bool exitGame = false;
    while(!exitGame) {
        Menu menu(renderer);
        Menu::MenuResult choose = menu.chooseMenu();
        if(choose == 0) {

            // Khi chế độ đã chọn, dừng nhạc intro menu
            Mix_HaltMusic();
            // Phát nhạc intro game một lần (tham số 1)
            Mix_PlayMusic(introGameMusic, 1);

            Game game;
            game.run();
            if(game.WIN) displayYouWin(renderer, youWinSound);
            else displayGameOver(renderer, game.enemyKill, gameOverSound);
            if(game.isReturnToMenu) {
                Mix_PlayMusic(introMenuMusic, -1);
                continue;
            } else {
                exitGame = true;
            }
        } else exitGame = true;
    }

    // Sau khi game kết thúc, giải phóng các tài nguyên âm thanh
    Mix_FreeMusic(introMenuMusic);
    Mix_FreeMusic(introGameMusic);
   // Mix_FreeChunk(bulletHitSound);
    Mix_FreeChunk(gameOverSound);
    // Đóng SDL_mixer
    Mix_CloseAudio();

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}
