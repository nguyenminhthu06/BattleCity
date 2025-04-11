#pragma once
#include <SDL.h>
#include <vector>
#include "PlayerTank.h"
#include "Wall.h"
#include "Constants.h"
#include"EnemyTank.h"

class Game {
public:
    SDL_Window* window;
    SDL_Renderer* renderer;
    bool running;
    std::vector<Wall> walls;
    PlayerTank player;
    int enemyNumber = 3;
    std::vector<EnemyTank> enemies;
    Game();
    ~Game();
    void generateWalls();
    void handleEvents();
    void spawnEnemies();
    void render();
    void update();
    void run();
};
