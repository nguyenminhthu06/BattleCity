#pragma once
#include <vector>
#include "Wall.h"
#include <SDL.h>
#include <SDL_image.h>
#include "Constants.h"
#include "Bullet.h"
class EnemyTank {
public:
    int x, y;
    int dirX, dirY;
    int moveDelay, shootDelay;
    bool active;
    SDL_Rect rect;
    std::vector<Bullet> bullets;
    EnemyTank(int startX, int startY);

    void move(const std::vector<Wall>& walls);
    void shoot();
    void updateBullets();
    void render(SDL_Renderer* renderer) const;
};
