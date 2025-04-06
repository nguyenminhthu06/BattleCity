#pragma once
#include <SDL.h>
#include<vector>
#include<Bullet.h>
class Tank {
public:
    int x, y;
    int dirX, dirY;
    SDL_Rect rect;
    std::vector<Bullet> bullets;
    Tank(int startX, int startY);
    void shoot();
    void updateBullets();
};
