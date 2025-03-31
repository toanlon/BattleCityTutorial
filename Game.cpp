#include "Game.h"

Game::Game() {
    running = true;

    // - bulletHitSound: Hiệu ứng âm thanh khi đạn trúng, được tải từ file WAV.
    bulletHitSound = Mix_LoadWAV("sound/bullethit.wav");
    if(!bulletHitSound) {
        cerr << "Failed to load bullethit.wav! SDL_mixer Error: " << Mix_GetError() << endl;
    }

    const string mapPath = "map/test.txt";
    LoadMap(mapPath);

    spawnEnemies();      // Hàm spawn enemy của bạn

    // Thiết lập lastSpawnTime ban đầu
    lastSpawnTime = SDL_GetTicks();
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
    player.p_render(renderer);

    for(auto& enemy : enemies) {
        enemy.e_render(renderer);
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
                    case SDLK_UP: player.p_move(true, "up", 0, -8, bricks, rocks, enemies); break;
                    case SDLK_DOWN: player.p_move(true, "down",0, 8, bricks, rocks, enemies); break;
                    case SDLK_LEFT: player.p_move(true, "left" , -8, 0, bricks, rocks, enemies); break;
                    case SDLK_RIGHT: player.p_move(true, "right", 8, 0, bricks, rocks, enemies); break;
                    case SDLK_SPACE: player.p_shoot() ; break;
                }
            }

        } else if(event.type == SDL_KEYUP) {
                switch(event.key.keysym.sym) {
                    case SDLK_UP: player.p_move(false, "up", 0, 0, bricks, rocks, enemies); break;
                    case SDLK_DOWN: player.p_move(false, "down",0, 0, bricks, rocks, enemies); break;
                    case SDLK_LEFT: player.p_move(false, "left" , 0, 0, bricks, rocks, enemies); break;
                    case SDLK_RIGHT: player.p_move(false, "right", 0, 0, bricks, rocks, enemies); break;
                    case SDLK_SPACE: player.p_shoot() ; break;
                    }
            }
    }
}
void Game::run() {
    while(running) {
        fps_timer.start();
        handleEvents();

        if(isPaused) {
            pauseMenu->renderPauseMenu(renderer);

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
    }
}

void Game::spawnEnemies(){
        enemyNumber--;
        if(enemyNumber == 0) return;
        int ex, ey;
        int dirX = 0, dirY = 0;
        bool validPosition = false;
        while(!validPosition) {
            int randEne = rand() % 5;
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
                    ey = 1*TILE_SIZE;
                    break;
                case 3:
                    ex = 26*TILE_SIZE;
                    ey = 16*TILE_SIZE;
                    dirX = -12;
                    dirY = 0;
                    break;
                case 4:
                    ex = 2*TILE_SIZE;
                    ey = 16*TILE_SIZE;
                    dirX = 12;
                    dirY = 0;
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
        if(dirX == 0 && dirY == 0) enemies.push_back(EnemyTank(ex, ey));
        else  enemies.push_back(EnemyTank(ex, ey, dirX, dirY));
}



void Game::update() {

   Uint32 currentTime = SDL_GetTicks();
   if(currentTime - lastSpawnTime >= spawnDelay) {
        spawnEnemies();      // Hàm spawn enemy của bạn
        lastSpawnTime = currentTime;
    }

    player.p_updateBullets();

    if(player.isInvincible && currentTime - player.isInvincibleStart >= player.invincible_Time) {
        player.isInvincible = false;
    }

    for(auto& enemy:enemies) {
        enemy.e_move(bricks, rocks, player.x, player.y);
        enemy.e_updateBullets();
          if(rand() % 100 < 15) {
            enemy.e_shoot();
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
    if(enemyKill == 19) {
        WIN = true;
        running = false;
    }
}

Game::~Game() {

}
