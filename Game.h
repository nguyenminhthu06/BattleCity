#pragma once
#include <SDL.h>
#include <vector>
#include "PlayerTank.h"
#include "Wall.h"
#include "Constants.h"

class Game {
public:
    SDL_Window* window;
    SDL_Renderer* renderer;
    bool running;
    std::vector<Wall> walls;
    PlayerTank player;

    Game();
    ~Game();
    void generateWalls();
    void handleEvents();
    void render();
    void run();
};
