#ifndef GAME_H_
#define GAME_H_

#include "PlayerTank.h"
#include "EnemyTank.h"
#include "Wall.h"
#include "Menu.h"
#include "ImpTimer.h"

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
    int enemyNumber = 18;
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


#endif // GAME_H_
