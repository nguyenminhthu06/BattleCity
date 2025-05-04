#pragma once
#include<vector>
#include"Wall.h"
#include<SDL.h>
#include<SDL_image.h>
#include "Constants.h"
#include"Bullet.h"
class PlayerTank {
public:
    int x, y;
    int dirX, dirY;
    float speed;
    SDL_Rect rect;

    SDL_Texture* texture = nullptr;
    std::vector<Bullet> bullets;

    PlayerTank(int startX, int startY);
    ~PlayerTank();
    void setPosition(int x, int y);
    void setTexture(SDL_Texture* tex);
    void move(int dx, int dy, const std::vector<Wall>& walls);
    void shoot();
    void updateBullets();
    bool loadTexture(SDL_Renderer* renderer, const char* path);
    void render(SDL_Renderer* renderer);

    void setSpeed(float newSpeed);

};

