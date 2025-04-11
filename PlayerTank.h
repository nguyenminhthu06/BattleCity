#pragma once
#include<vector>
#include"Wall.h"
#include<SDL.h>
#include "Constants.h"

class PlayerTank{
public:
    int x, y;
    int dirX,dirY;
    SDL_Rect rect;

    PlayerTank (int startX, int startY);
    void move(int dx, int dy, const std::vector<Wall> &walls);
    void render(SDL_Renderer* renderer);
};
