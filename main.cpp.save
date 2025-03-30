#include "CommonFuntion.h"
#include "Menu.h"
#include "Wall.h"
#include "EnemyTank.h"
#include "Bullet.h"
#include "PlayerTank.h"
#include "Game.h"

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
