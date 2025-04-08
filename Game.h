#pragma once
#include "Tank.h"
#include "Bullet.h"
#include <vector>
#include"Wall.h"
#include"PlayerTank.h"
#include"EnemyTank.h"
class Game {
public:
    SDL_Window* window;
    SDL_Renderer* renderer;
    bool running;
    std::vector<Wall> walls;
    PlayerTank player;
    std::vector<EnemyTank> enemies;
    void spawnEnemies();
    SDL_Texture* playerTexture;
    SDL_Texture* enemyTexture;
    SDL_Texture* bulletTexture;
    SDL_Texture* wallTexture;
    SDL_Texture* backgroundTexture;

    Game();
    void generateWalls();

    void render();

    void run();

    void update();

    void handleEvents() ;

    int enemyNumber;

};

