#pragma once
#include <vector>
#include "Wall.h"
#include <SDL.h>
#include "Constants.h"
#include "Bullet.h"

class PlayerTank {
public:
    int x, y;
    int dirX, dirY;
    float speed;
    SDL_Rect rect;
    SDL_Texture* texture;
    std::vector<Bullet> bullets;

    PlayerTank(int startX, int startY, SDL_Renderer* renderer);
    ~PlayerTank();

    void move(int dx, int dy, const std::vector<Wall>& walls);
    void shoot();
    void updateBullets();
    void render(SDL_Renderer* renderer);
    void setSpeed(float newSpeed);
    void loadTexture(SDL_Renderer* renderer, const char* path);
};
