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
    float moveSpeed;
    bool active;
    SDL_Rect rect;
    SDL_Texture* texture;
    std::vector<Bullet> bullets;
    EnemyTank(int startX, int startY);

    void setTexture(SDL_Texture* tex);
    void setSpeed(float speed);
    void move(const std::vector<Wall>& walls);
    void shoot();
    void updateBullets();
    void render(SDL_Renderer* renderer) const;
    void update(float deltaTime, const std::vector<Wall>& walls);

};



