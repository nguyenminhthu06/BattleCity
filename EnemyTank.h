#pragma once
#include <vector>
#include "Wall.h"
#include <SDL.h>
#include "Constants.h"
#include "Bullet.h"

class EnemyTank {
public:
    int x, y;
    int dirX, dirY;
    SDL_Rect rect;
    bool active;
    std::vector<Bullet> bullets;

    // Thêm các biến tốc độ
    float moveSpeed;
    int moveDelay;
    int shootDelay;
    float bulletSpeed;

    EnemyTank(int startX, int startY, float speed = 1.0f);
    void move(const std::vector<Wall>& walls);
    void shoot();
    void updateBullets();
    void render(SDL_Renderer* renderer) const;

    // Thêm phương thức điều chỉnh tốc độ
    void setSpeed(float newSpeed);
    void setBulletSpeed(float newSpeed);
};
