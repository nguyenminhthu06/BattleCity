#pragma once
#include<vector>
#include"Wall.h"
#include<SDL.h>
#include "Constants.h"
#include"Bullet.h"
class PlayerTank{
public:
    int x, y;
    int dirX,dirY;
    SDL_Rect rect;
    std::vector<Bullet> bullets;
    PlayerTank (int startX, int startY);
    void move(int dx, int dy, const std::vector<Wall> &walls);
    void shoot();
    void updateBullets();
    void render(SDL_Renderer* renderer);
};
